//===-- llc.cpp - Implement the LLVM Native Code Generator ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This is the llc code generator driver. It provides a convenient
// command-line interface for generating native assembly-language code
// or C code, given LLVM bitcode.
//
//===----------------------------------------------------------------------===//


#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/CodeGen/CommandFlags.inc"
//#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Pass.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Target/TargetMachine.h"
//#include "llvm/Target/TargetSubtargetInfo.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include <memory>
#include <iostream>
#include "llcapi.h"

using namespace llvm;
/** Funguje jen první průchod, další hodí segfault, neumím to zprovoznit.
 * Lépe bude použít přímo llc jako příkaz. To je sice další závislost, ale nedá se nic dělat,
 * LLVM je potřeba tak jako tak.
 * */
// main - Entry point for the llc compiler.
//
// General options for llc.  Other pass-specific options are specified
// within the corresponding llc passes, and target-specific options
// and back-end code generation options are specified with the target machine.
//
static std::string InputFilename;
static std::string OutputFilename;
//static unsigned TimeCompilations = 1;
static bool Binary = false;

static int compileModule(const char *, LLVMContext &, LLVMTypeMachine f);
/// rozhrani
int CompileLLtoASFile (const char * infile, const char* outfile, LLVMTypeMachine f) {
  InputFilename  = infile;
  OutputFilename = outfile;
  if (f == MachineTypeWasm) Binary = true;

  // Enable debug stream buffering.
  // EnableDebugBuffering = true;

  LLVMContext Context;// = getGlobalContext();
  llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
  
    // Initialize targets first, so that --version shows registered targets.
    InitializeAllTargets    ();
    InitializeAllTargetMCs  ();
    InitializeAllAsmPrinters();
    InitializeAllAsmParsers ();

    // Initialize codegen and IR passes used by llc so that the -print-after,
    // -print-before, and -stop-after options work.
    PassRegistry * Registry = PassRegistry::getPassRegistry();
    initializeCore                     (*Registry);
    initializeCodeGen                  (*Registry);
    initializeLoopStrengthReducePass   (*Registry);
    initializeLowerIntrinsicsPass      (*Registry);
    //initializeUnreachableBlockElimPass (*Registry);
    initializeUnreachableBlockElimLegacyPassPass(*Registry);
    
  // Register the target printer for --version.
  // cl::AddExtraVersionPrinter(TargetRegistry::printRegisteredTargetsForVersion);
  /*
  const char ** argv = linecmd [f];
  const int     argc = cmdlen(f);
  */
  // cl::ParseCommandLineOptions(argc, argv/*, "llvm test compiler\n"*/);
  // Compile the module TimeCompilations times to give better compile time
  // metrics.
  //for (unsigned I = TimeCompilations; I; --I)
  int RetVal = compileModule("llcapi", Context, f);

  return RetVal;
}

static bool NoIntegratedAssembler = false;
// Determine optimization level.
static char OptLevel = '3';
static std::string TargetTriple = "";
static bool NoVerify = false;
//static bool DisableSimplifyLibCalls = false;
static bool ShowMCEncoding = false;
static bool EnableDwarfDirectory = false;
static bool AsmVerbose = true;
//static bool CompileTwice = false;

static std::unique_ptr<ToolOutputFile>
GetOutputStream(const char *TargetName, Triple::OSType OS,
                const char *ProgName) {
  // If we don't yet have an output filename, make one.
  if (OutputFilename.empty()) {
    if (InputFilename == "-") OutputFilename = "-";
    else {
      // If InputFilename ends in .bc or .ll, remove it.
      StringRef IFN = InputFilename;
      if (IFN.endswith(".bc") || IFN.endswith(".ll"))
        OutputFilename = IFN.drop_back(3);
      else if (IFN.endswith(".mir"))
        OutputFilename = IFN.drop_back(4);
      else
        OutputFilename = IFN;

      switch (FileType) {
      case CGFT_AssemblyFile:
        if (TargetName[0] == 'c') {
          if (TargetName[1] == 0)
            OutputFilename += ".cbe.c";
          else if (TargetName[1] == 'p' && TargetName[2] == 'p')
            OutputFilename += ".cpp";
          else
            OutputFilename += ".s";
        } else
          OutputFilename += ".s";
        break;
      case CGFT_ObjectFile:
        if (OS == Triple::Win32)
          OutputFilename += ".obj";
        else
          OutputFilename += ".o";
        break;
      case CGFT_Null:
        OutputFilename += ".null";
        break;
      }
    }
  }
  if (Binary) FileType = CGFT_ObjectFile;
  // Decide if we need "binary" output.
  switch (FileType) {
  case CGFT_AssemblyFile:
    break;
  case CGFT_ObjectFile:
  case CGFT_Null:
    Binary = true;
    break;
  }

  // Open the file.
  std::error_code EC;
  sys::fs::OpenFlags OpenFlags = sys::fs::F_None;
  if (!Binary) OpenFlags |= sys::fs::F_Text;
  auto FDOut = std::make_unique<ToolOutputFile>(OutputFilename, EC, OpenFlags);
  if (EC) {
    errs() << EC.message() << '\n';
    return nullptr;
  }

  return FDOut;
}

static int compileModule(const char * argvn, LLVMContext &Context, LLVMTypeMachine mp) {
  TargetMachine * mTarget = nullptr;
  // Load the module to be compiled...
  SMDiagnostic Err;
  std::unique_ptr<Module> M;
  std::unique_ptr<MIRParser> MIR;
  Triple TheTriple;

  bool SkipModule = MCPU == "help" ||
                    (!MAttrs.empty() && MAttrs.front() == "help");

  // If user just wants to list available options, skip module loading
  if (!SkipModule) {
    if (StringRef(InputFilename).endswith_lower(".mir")) {
      MIR = createMIRParserFromFile(InputFilename, Err, Context);
      if (MIR) {
        M = MIR->parseIRModule();
        assert(M && "parseLLVMModule should exit on failure");
      }
    } else
      M = parseIRFile(InputFilename, Err, Context);
    if (!M) {
      Err.print(argvn, errs());
      return 1;
    }

    // Verify module immediately to catch problems before doInitialization() is
    // called on any passes.
    if (!NoVerify && verifyModule(*M, &errs())) {
      errs() << argvn << ": " << InputFilename
             << ": error: input module is broken!\n";
      return 1;
    }

    // If we are supposed to override the target triple, do so now.
    if (!TargetTriple.empty())
      M->setTargetTriple(Triple::normalize(TargetTriple));
    TheTriple = Triple(M->getTargetTriple());
  } else {
    TheTriple = Triple(Triple::normalize(TargetTriple));
  }

  if (TheTriple.getTriple().empty())
    TheTriple.setTriple(sys::getDefaultTargetTriple());

  // Get the target specific parser.
  std::string Error;
  const Target * TheTarget = TargetRegistry::lookupTarget(MArch, TheTriple, Error);
  if (!TheTarget) {
    errs() << argvn << ": " << Error;
    return 1;
  }

  std::string CPUStr = getCPUStr(), FeaturesStr = getFeaturesStr();

  CodeGenOpt::Level OLvl = CodeGenOpt::Default;
  switch (OptLevel) {
  default:
    errs() << argvn << ": invalid optimization level.\n";
    return 1;
  case ' ': break;
  case '0': OLvl = CodeGenOpt::None; break;
  case '1': OLvl = CodeGenOpt::Less; break;
  case '2': OLvl = CodeGenOpt::Default; break;
  case '3': OLvl = CodeGenOpt::Aggressive; break;
  }

  TargetOptions Options = InitTargetOptionsFromCodeGenFlags();
  Options.DisableIntegratedAS = NoIntegratedAssembler;
  Options.MCOptions.ShowMCEncoding = ShowMCEncoding;
  Options.MCOptions.MCUseDwarfDirectory = EnableDwarfDirectory;
  Options.MCOptions.AsmVerbose = AsmVerbose;
  
  Options.DataSections     = true;
  Options.FunctionSections = true;
  Reloc::Model mrel = Reloc::Static;
  switch (mp) {
    case MachineTypeCortexM3:
      TheTriple.setTriple ("thumbv7m--");
      CPUStr = "cortex-m3";
      break;
    case MachineTypeCortexM4F:
      TheTriple.setTriple ("thumbv7em--");
      CPUStr = "cortex-m4";
      FeaturesStr = "+fp-only-sp,+d16";
      break;
    default:
      //Options.PositionIndependentExecutable = true;
      mrel = Reloc::PIC_;
      break;
  }

  std::cout << "Triple: " << TheTriple.getTriple() << " CPUStr: " << CPUStr
  << " FeaturesStr: " << FeaturesStr << " RelocModel: " << mrel << "\n";

  mTarget = TheTarget->createTargetMachine(TheTriple.getTriple(), CPUStr, FeaturesStr,
                                     Options, mrel, getCodeModel(), OLvl);

  assert(mTarget && "Could not allocate target machine!");

  // If we don't have a module then just exit now. We do this down
  // here since the CPU/Feature help is underneath the target machine
  // creation.
  if (SkipModule)
    return 0;

  assert(M && "Should have exited if we didn't have a module!");
  if (FloatABIForCalls != FloatABI::Default)
    Options.FloatABIType = FloatABIForCalls;

  // Figure out where we are going to send the output.
  std::unique_ptr<ToolOutputFile> Out =
      GetOutputStream(TheTarget->getName(), TheTriple.getOS(), argvn);
  if (!Out) return 1;

  // Build up all of the passes that we want to do to the module.
  legacy::PassManager PM;
  // Add the target data from the target machine, if it exists, or the module.
  M->setDataLayout(mTarget->createDataLayout());

  // Override function attributes based on CPUStr, FeaturesStr, and command line
  // flags.
  setFunctionAttributes(CPUStr, FeaturesStr, *M);
  
  raw_pwrite_stream *OS = &Out->os();

  // Manually do the buffering rather than using buffer_ostream,
  // so we can memcmp the contents in CompileTwice mode
  SmallVector<char, 0> Buffer;
  std::unique_ptr<raw_svector_ostream> BOS;
  
  if (mTarget->addPassesToEmitFile(PM, *OS, nullptr, FileType, NoVerify)) {
    errs() << argvn << ": target does not support generation of this"
            << " file type!\n";
    return 1;
  }
  // Before executing passes, print the final values of the LLVM options.
  cl::PrintOptionValues();
  PM.run(*M);
  if (BOS) {
    Out->os() << Buffer;
  }

  // Declare success.
  Out->keep();
/*
  delete mTarget;
  mTarget = nullptr;
*/
  return 0;
}
