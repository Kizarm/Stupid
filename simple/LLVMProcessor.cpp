#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "inc/Znak.h"
//#include "inc/list.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/LLVMProcessor.h"
#include "inc/firmware.h"
#include "inc/fce.h"
#include "inc/chyby.h"

using namespace std;

enum OperationsTypes {
  WordOp=0, RealOp,
  MaxOperationsType
};
//#define trace(...)
static void trace (const char * fmt, ...) {
  const int buflen = 0x400;
  char buf [buflen];
  va_list arglist;
  va_start (arglist, fmt);
  vsnprintf (buf, buflen, fmt, arglist);
  va_end (arglist);
  
  printf  (     "%s", buf);
  fprintf (mf, ";%s", buf);
  *currentFunction << ";" << buf;
}

static OperationCodeTexts const OpCTextsCommon [MaxOperationsType] = {
  { 2, "Word","i16"  , {  "add", "sub", "mul","udiv","sdiv", NULL }},
  { 4, "Real","float", { "fadd","fsub","fmul","fdiv", NULL  }}
};
void LLVMProcessor::PreDefinitions (void) {
  PreDefinition (OpCTextsCommon[WordOp]);
  PreDefinition (OpCTextsCommon[RealOp]);
}
// funkce jsou static inline
void LLVMProcessor::PreDefinition (const OperationCodeTexts & text) {
  const int m = (int) MaxOperationsCodeNumber;
  for (int i=0; i<m; i++) {
    if (!text.OpName[i]) break;
    fprintf(mf, "\n; Function Attrs: nounwind inline\n");
    fprintf(mf, "define internal void @%s%s(%s* nocapture %%a, %s* nocapture readonly %%b) #0 {\n",
      text.OpName[i], text.Suffix, text.OpType,  text.OpType);
    fprintf(mf, "  %%1 = load %s, %s* %%b, align %d\n", text.OpType, text.OpType, text.align);
    fprintf(mf, "  %%2 = load %s, %s* %%a, align %d\n", text.OpType, text.OpType, text.align);
    fprintf(mf, "  %%3 = %s %s %%2, %%1\n", text.OpName[i], text.OpType);
    fprintf(mf, "  store %s %%3, %s* %%a, align %d\n", text.OpType, text.OpType, text.align);
    fprintf(mf, "  ret void\n}\n");
  }
}

extern char   InpF[80];
extern char   MacF[80];
extern char   SymF[80];
extern char   ObjF [80];
extern char   HexF[80];
extern char   DnlF[80];
extern char   SupF[80];
extern int    ListF,MCodeF,SrcF,VerbF;
extern FILE  * of;

static  const char * vardef = "%Variables";
static  const char * ramdef = "%union.RamDef_u";

struct LLVMTexts {
  const char * prefix;
  const char * suffix;
};
static const LLVMTexts pst[LLVMTypesMax] = {
  {
    "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\ntarget triple = \"x86_64-pc-linux-gnu\"\n",
    
    "attributes #0 = { nounwind uwtable \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"false\" \"no-infs-fp-math\"=\"false\""
    "\"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" \"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n"
    "attributes #1 = { \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"false\" \"no-infs-fp-math\"=\"false\" "
    "\"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" \"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n"
    "attributes #2 = { nounwind }"
  },
  {
    "target datalayout = \"e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-f64:32:64-v64:32:64-v128:32:128-a:0:32-n32-S32\"\n"
    "target triple = \"thumbv7m--\"\n",
    
    "attributes #0 = { nounwind \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"true\""
    " \"no-frame-pointer-elim-non-leaf\" \"no-infs-fp-math\"=\"false\" \"no-nans-fp-math\"=\"false\" "
    "\"stack-protector-buffer-size\"=\"8\" \"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"true\" }\n"
    "attributes #1 = { \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"true\" \"no-frame-pointer-elim-non-leaf\" "
    "\"no-infs-fp-math\"=\"false\" \"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" "
    "\"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"true\" }\n"
  },
  {
    "target datalayout = \"e-m:e-p:32:32-i64:64-v128:64:128-a:0:32-n32-S64\"\n"
    "target triple = \"thumbv7em--\"\n",
    
    "attributes #0 = { norecurse nounwind \"disable-tail-calls\"=\"false\" \"less-precise-fpmad\"=\"false\""
    " \"no-frame-pointer-elim\"=\"true\" \"no-frame-pointer-elim-non-leaf\" \"no-infs-fp-math\"=\"false\""
    " \"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" \"target-cpu\"=\"cortex-m4\""
    " \"target-features\"=\"+d16,+dsp,+fp-only-sp,+hwdiv,+strict-align,+vfp4,-crypto,-fp-armv8,-neon\" "
    "\"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n"
    "attributes #1 = { argmemonly nounwind }\n"
  }
};
static const LLVMTexts CommonTexts = {
  "@RamBasePtr = external global %union.RamDef_u*\n"
  "\n; Function Attrs: nounwind\n"
  "define void @Simple() #0 {\n"
  "  %carry = alloca i16, align 2\n"    // pro lepsi optimalizaci bude lepsi mit carry jako lokalni
  "  %RegHL = alloca i16, align 2\n"    // a RegHL
  "  %RegRE = alloca float, align 4\n"  // a RegRE
  ,
  "declare zeroext i16 @WgetBit(i32) #1\n"
  "declare void @WsetBit(i32, i16 zeroext) #1\n"
  "declare void @WcplBit(i32) #1\n"
  "declare zeroext i16 @TestPole(i16 zeroext,i16 zeroext) #1\n"
  "declare float @fabsf(float %Val)\n"
  "declare float @truncf(float %Val)\n"
  "declare void @WordChangeWrap(i16* %ptr, i16 %data)\n"
};

LLVMProcessor::LLVMProcessor (LLVMTypeMachine t) : BaseProcessor(),
  m_Type (t), maxstrs(256), mStack() {
  iFmt = NULL;
  ptrSize = 4;
  wpSize  = 4;
  switch (m_Type) {
    case MachineTypeLinux64:
      ptrSize = 8;
      wpSize  = 8;
      break;
    case MachineTypeCortexM3:
    case MachineTypeCortexM4F:
      break;
    default:
      break;
  }
  char buf [80];
  snprintf(buf, 80, "i%d", ptrSize * 8);
  iFmt = strdup (buf);
  fromField = false;
  
  mainfunc = new llfnc ("Simple");
}
LLVMProcessor::~LLVMProcessor() {
  if (iFmt) free (iFmt);
  delete mainfunc;
}
void LLVMProcessor::loadRamPtr (void) {
  fprintf(mf,
    "  %s = load %s*, %s** @RamBasePtr, align %d\n\n",
    vardef, ramdef, ramdef, ptrSize);
}
/**
 * Pravidlo:
 * za kazde "  %%%ld = cosi..." se musi zvysit citac PerNum o 1.
 * */
void LLVMProcessor::getElementPtr (long unsigned int adr) {
  fprintf(mf,
    "  %%%ld = getelementptr inbounds %s, %s* %s, %s 0, i32 0, %s %ld\n",
    PerNum, ramdef, ramdef, vardef, iFmt, iFmt, adr);
  PerNum += 1;
}
void LLVMProcessor::c_PragmaLj() {
  fprintf (stdout, "Funkce:%s - unused\n", __FUNCTION__);
}
void LLVMProcessor::c_PragmaSj() {
  fprintf (stdout, "Funkce:%s - unused\n", __FUNCTION__);
}
void LLVMProcessor::PopLbl (void) {
  fprintf (mf,"\nL%04lX:\n", (unsigned long) SemPop());
}

void LLVMProcessor::c_if() {
  // provede skok na adresu ++LblNum, pokud je carry
  LblNum++;
  //-fprintf (stdout, "Funkce:!%s\t->L%04lX\n", __FUNCTION__, LblNum);
  trace ("\t#IF ->L%04lX\n", LblNum);
  fprintf (mf,"  %%%ld = load i16, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
  fprintf (mf,"  %%%ld = and i16 %%%ld, 1\n", PerNum, PerNum-1);
  fprintf (mf,"  %%%ld = icmp eq i16 %%%ld, 0\n", PerNum+1, PerNum);
  fprintf (mf,"  br i1 %%%ld, label %%L%04lX, label %%D%04lX", PerNum+1, LblNum, LblNum);
  fprintf (mf,"\nD%04lX:\n", LblNum);
  SemPush (LblNum);
  PerNum += 2;
  // a pak jeste nejak obnovi carry
  lle e1 = mainfunc->carry.load ();
  lle e2 = e1.binaryOp ("and", 1);
  lle e3 = e2.binaryOp ("icmp eq", 0);
}
void LLVMProcessor::c_else() {
  LblNum++;
  //-fprintf (stdout, "Funkce:%s\t->L%04lX\n", __FUNCTION__, LblNum);
  trace ("\t#ELSE ->L%04lX\n", LblNum);
  fprintf (mf,"  br label %%L%04lX\n", LblNum);
  PopLbl();
  SemPush (LblNum);
}
void LLVMProcessor::c_endif() {
  // placne tam label, schovanou v c_if
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned long tmp = (unsigned long) SemPop();
  trace ("\t#ENDIF ->L%04lX\n", tmp);
  fprintf (mf,"  br label %%L%04lX\n", tmp);      // llvm musi skocit za sebe !
  fprintf (mf,"\nL%04lX:\n", tmp);
}

void LLVMProcessor::c_procinit() {
  //-fprintf (stdout, "\nFunkce:%s\n", __FUNCTION__);

  mf=of;
  LblNum = 0;
  PerNum = 1;
  TOC    = 0;
  TOCFL  = 0UL;
  
  ProcNum = 0;
  FuncNum = 0;

  if (!RomBegin) {
    RomBegin = ROMORG;
  }
  if (!RamBegin) {
    RamBegin = RAMORG;
  }
  procFile = fopen ("data_proc_tmp.txt","w+");
  if (!procFile) {
    fprintf (stderr, "tmp file cannot be open\n");
    exit(-1);
  }

  RamPtr=RamBegin;

  AddInfo ("\nImplementace pro LLVM, Kizarm - testing\n");

  fprintf (mf,"%s\n", pst[m_Type].prefix);
  fprintf (mf,"%s = type { [1024 x i16] }\n", ramdef);
///  PreDefinitions();
  mStack.Declare();
  fprintf (mf,"%s", CommonTexts.prefix);
  mStack.Define(16, iFmt);
  loadRamPtr();
  
  const int bufmax = 0x400;
  char buffer [bufmax];
  int count = 0;
  count += snprintf (buffer + count, bufmax - count, "%s\n", pst[m_Type].prefix);
  count += snprintf (buffer + count, bufmax - count, "%s = type { [1024 x i16] }\n", ramdef);
  currentFunction = mainfunc;
  currentFunction->prefix (buffer);
}
void LLVMProcessor::c_procend() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  long int a,b;
//   char c;
  // copy procFile -> mf
  fprintf (mf,"\nret void\n}\n");
  insertAt(mf, procFile);
  fprintf (mf,"%s\n", CommonTexts.suffix);
  fprintf (mf,"%s\n", pst[m_Type].suffix);
  fclose (mf);
  char * copy = new char [1024];
  strncpy (copy, MacF, 1024);
  strcat (copy, ".ll");
  rename (MacF, copy);
  delete [] copy;

  a=RamPtr>>3;
  b=RamBegin>>3;
  if (RamPtr&7) {
    a++;
  }

  AddInfo ("\n");
  AddInfo ("\nAdresa stanice    ... %d",NetAdr);
  AddInfo ("\nProgram           ... %04LXH",RomBegin);
  AddInfo ("\nPromenne          ... %04LXH",b);
  AddInfo (" - %04LXH\n",a);
  AddInfo ("\tTOC = %d\n", TOC);

  fclose (procFile);
  remove ("data_proc_tmp.txt");
  
  FILE * testfile = fopen ("test.cc.ll","w");
  currentFunction->exit();
  *currentFunction << CommonTexts.suffix << "\n" << pst[m_Type].suffix << "\n";
  currentFunction->write (testfile);
  fclose (testfile);
}
void LLVMProcessor::insertAt (FILE * dst, FILE * src) {
  fseek (src, 0l, SEEK_SET);
  const size_t max = 1024;
  char buf [max];
  size_t copied = 0;
  for (;;) {
    size_t n = fread (buf, 1, max, src);
    if (n == 0)  break;
    size_t m = fwrite(buf, 1, n,   dst);
    copied += m;
    if (m == 0)  break;
    if (n < max) break;
  }
  trace ("Zkopirovano : %ld bytes\n", copied);
}

/** *************************************************************************/
void LLVMProcessor::AllocaCNew (void) {
  if (TOC >= 8 * (int) sizeof (unsigned long)) fprintf (stderr, "Preteceni zasobniku Carry\n");
  if (((1UL << TOC) & TOCFL) == 0UL) {
    TOCFL |= 1UL << TOC;
    fprintf(mf, "  %%T%dOC = alloca i16, align 2\n", TOC);
  }
}

void LLVMProcessor::c_pushl() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace   ("\tPUSH CARRY; TOC++\n");
  fprintf (mf, "  %%%ld = load i16, i16* %%carry, align 2\n", PerNum);
  AllocaCNew();
  fprintf (mf, "  store i16 %%%ld, i16* %%T%dOC, align 2\n", PerNum, TOC);
  PerNum += 1;
  TOC    += 1;
}
void LLVMProcessor::c_popl() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace   ("\tPOP CARRY [->last %%var, nemeni %%carry]; --TOC\n");
  TOC    -= 1;
  fprintf (mf, "  %%%ld = load i16, i16* %%T%dOC, align 2\n", PerNum, TOC);
  PerNum += 1;
}
/*---------------------------------------------------------------------------*/
/*   Bitove operace                                                          */
/*---------------------------------------------------------------------------*/

void LLVMProcessor::c_setbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  trace  ("\tBIT[%04lX]=%04lX.%lX SET DIRECT\n", n, n>>3, n&7);
  fprintf (mf, "  tail call void @WsetBit(i32 %ld, i16 zeroext 1) #2\n", n);
}
void LLVMProcessor::c_resbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  trace  ("\tBIT[%04lX]=%04lX.%lX RESET DIRECT\n", n, n>>3, n&7);
  fprintf (mf, "  tail call void @WsetBit(i32 %ld, i16 zeroext 0) #2\n", n);
}
void LLVMProcessor::c_invbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  trace  ("\tBIT[%04lX]=%04lX.%lX CPL DIRECT\n", n, n>>3, n&7);
  fprintf (mf, "  tail call void @WcplBit(i32 %ld) #2\n", n);
}
void LLVMProcessor::c_ldbd (unsigned long int n) {
  // do carry da bit na adrese n
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  trace  ("\tBIT %04lX.%lX -> CARRY\n", n>>3, n&7);
  PredefAdr (n);
  fprintf (mf, "  %%%ld = tail call zeroext i16 @WgetBit(i32 %ld) #2\n", PerNum, n);
  fprintf (mf, "  store i16 %%%ld, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
  
  lle e1 = currentFunction->callGetBit(n);
  currentFunction->carry.store(e1);
}
void LLVMProcessor::c_stbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  trace  ("\tCARRY -> BIT %04lX.%lX\n", n>>3, n&7);
  PredefAdr (n);
  fprintf (mf, "  %%%ld = load i16, i16* %%carry, align 2\n", PerNum);
  fprintf (mf, "  tail call void @WsetBit(i32 %ld, i16 zeroext %%%ld) #2\n", n, PerNum);
  PerNum += 1;
}
void LLVMProcessor::logOpBD (const char * op, bool inv, long unsigned int n) {
  PredefAdr (n);
  if (inv) trace  ("\tBIT %04lX.%lX DIRECT not %s CARRY -> CARRY\n", n>>3, n&7, op);
  else     trace  ("\tBIT %04lX.%lX DIRECT     %s CARRY -> CARRY\n", n>>3, n&7, op);
  PredefAdr (n);
  fprintf (mf, "  %%%ld = tail call zeroext i16 @WgetBit(i32 %ld) #2\n", PerNum, n);
  PerNum += 1;
  if (inv) {
    fprintf (mf, "  %%%ld = xor i16 %%%ld, -1\n", PerNum, PerNum-1);        // negace operandu
    PerNum += 1;
  }
  fprintf (mf, "  %%%ld = load i16, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
  fprintf (mf, "  %%%ld = %s i16 %%%ld, %%%ld\n", PerNum, op, PerNum-2, PerNum-1);
  fprintf (mf, "  store i16 %%%ld, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
}

void LLVMProcessor::c_orbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("or", false, n);
}
void LLVMProcessor::c_andbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("and", false, n);
}
void LLVMProcessor::c_norbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("or", true, n);
}
void LLVMProcessor::c_nandbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("and", true, n);
}
void LLVMProcessor::logOpWpop (const char * op) {
  c_popl();
  trace   ("\tCARRY %s POP[CARRY] -> CARRY\n", op);
  fprintf (mf, "  %%%ld = load i16, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
  fprintf (mf, "  %%%ld = %s i16 %%%ld, %%%ld\n", PerNum, op, PerNum-1, PerNum-2);
  fprintf (mf, "  store i16 %%%ld, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;

}
void LLVMProcessor::c_orb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  logOpWpop ("or");
}
void LLVMProcessor::c_andb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  logOpWpop ("and");
}
void LLVMProcessor::c_xorb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  logOpWpop ("xor");
}
void LLVMProcessor::c_notb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace  ("\tBIT CARRY NOT\n");
  fprintf (mf, "  %%%ld = load i16, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
  fprintf (mf, "  %%%ld = xor i16 %%%ld, -1\n", PerNum, PerNum-1);
  fprintf (mf, "  store i16 %%%ld, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
}

union R2U {
  struct {
    u_int16_t h;
    u_int16_t l;
  };
  u_int32_t hl;
  float x;
};
union D2U {
  u_int64_t u;
  double    d;
};
/** Vypada to, ze interne je pouzita zasobnikova neco jako RPN.
 *  c_ldrim() nacte konstantu                    +1
 *  c_ldaim() nacte adresu na vrchol zasobniku
 *  c_ldri () nacte operand dany c_ldaim()       +1
 * 
 *  c_stri () ulozi na adresu %RegHL             -1
 * 
 *  c_operace() provede operaci nad zasobnikem   -1 (binarni - ze 2 poslednich da vysledek do posledniho)
 * 
 * Na konci by mela byt 0.
 * 
 * Zkusime optimalizaci pomoci inline funkci misto pouziti pomocneho zasobniku. Zasady:
 *      Prevedeno do samostatne tridy LLVMStack.
 * Nevyhody:
 *      - velke mnozstvi preddefinovanych inline funkci.
 * Vyhody:
 *      - umozni lepsi optimalizaci LLVM opt.
 *      - a asi i prehlednejsi
 * */

void LLVMProcessor::c_ldaim (unsigned long int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX ->>>>>>>>>> je v bytech\n", __FUNCTION__, a);
  trace        ("\tNacteni adresy %04lX do TOS; TOS++\n", a >> 4);
  fprintf (mf, ";\tNacteni adresy %04lX do TOS; TOS++\n", a >> 4);
  PredefAdr (a);
  mStack.PushAddrInBytes (a >> 3, PerNum);
}
void LLVMProcessor::RealFromStack (void) {
  mStack.PopWord(PerNum);                                               // index v bytech
  fprintf(mf,"  %%%ld = ashr i16 %%%ld, 1\n", PerNum, PerNum-1);        // index ve WORD
  PerNum += 1;
  fprintf(mf,
    "  %%%ld = getelementptr inbounds %s, %s* %s, %s 0, i32 0, i16 %%%ld\n",
    PerNum, ramdef, ramdef, vardef, iFmt, PerNum-1);
  PerNum += 1;
  fprintf (mf, "  %%%ld = bitcast i16* %%%ld to float*\n", PerNum, PerNum-1);
  PerNum += 1;
}

void LLVMProcessor::c_stri() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  if (fromField) {
    trace        ("\tSTORE REAL TO FIELD -> @TOS; --TOS\n");
    fprintf (mf, ";\tSTORE REAL TO FIELD -> @TOS; --TOS\n");
    mStack.PopReal(PerNum);
    fprintf (mf,"  %%%ld = bitcast i16* %%%ld to float*\n", PerNum, PerNum-4);
    PerNum += 1;
    fprintf (mf,"  store float %%%ld, float* %%%ld, align 4\n\n", PerNum-2, PerNum-1);
  } else {
    trace        ("\tSTORE REAL @TOS-1 -> @TOS; TOS-=2\n");
    fprintf (mf, ";\tSTORE REAL @TOS-1 -> @TOS; TOS-=2\n");
    RealFromStack();
    mStack.PopReal(PerNum);
    fprintf (mf, "  store float %%%ld, float* %%%ld, align 4\n\n", PerNum-1, PerNum-4);
  }
  mStack.ZeroTest();
  fromField = false;
}
void LLVMProcessor::c_ldri() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  /*  load indirect operand  */
  if (fromField) {
    trace       ("\tLOAD REAL FROM FIELD -> @TOS++;\n");
    fprintf (mf,";\tLOAD REAL FROM FIELD -> @TOS++;\n");
    fprintf (mf,"  %%%ld = bitcast i16* %%%ld to float*\n", PerNum, PerNum-1);
    PerNum += 1;
    fprintf (mf,"  %%%ld = load float, float* %%%ld, align 2\n",   PerNum, PerNum-1);
    PerNum += 1;
    mStack.PushReal(PerNum);
  } else {
    trace       ("\tLOAD REAL @TOS -> @TOS;\n");
    fprintf (mf,";\tLOAD REAL @TOS -> @TOS;\n");
    RealFromStack();
    fprintf (mf, "  %%%ld = load float, float* %%%ld, align 4\n", PerNum, PerNum-1);
    PerNum += 1;
    mStack.PushReal(PerNum);
  }
  fromField = false;
}
void LLVMProcessor::c_ldrim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  R2U t; D2U d;         // Konverze je nepochopitelna, ale funkcni
  t.h = SemPop();       // !!! neprenositelna na be
  t.l = SemPop();
  d.d = t.x;    // musi byt double a z toho teprve hex string
  trace  ("\tLOAD IMM %g, 0x%016lX -> @TOS; TOS++\n", t.x, d.u);
  mStack.PushImmReal (d.u, PerNum);
}
void LLVMProcessor::mathOpRE (const char * op) {
  trace   ("\t! REAL @TOS %s @TOS-1 -> @TOS; --TOS\n", op);
  mStack.PopRealPtr(PerNum);
  fprintf (mf,"  tail call void @%sReal(float* %%%ld, float* %%%ld)\n", op, PerNum-1, PerNum-3);
}
void LLVMProcessor::c_addr() {
  //-fprintf  (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fadd");
}
void LLVMProcessor::c_subr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fsub");
}
void LLVMProcessor::c_mulr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fmul");
}
void LLVMProcessor::c_divr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fdiv");
}

/** *********************************************************************************************************/
void LLVMProcessor::c_chsr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tREAL @TOS -> -@TOS\n");
  fprintf (mf,";\tREAL @TOS -> -@TOS\n");
  mStack.CurrentRealPtr(PerNum);
  unsigned long last = PerNum - 1;
  fprintf (mf, "  %%%ld = load float, float* %%%ld, align 2\n", PerNum, PerNum-1);
  PerNum += 1;
  fprintf (mf, "  %%%ld = fsub float 0.0, %%%ld\n", PerNum, PerNum-1);
  PerNum += 1;
  fprintf (mf, "  store float %%%ld, float* %%%ld, align 4\n", PerNum-1, last);
}
void LLVMProcessor::c_absr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tREAL @TOS -> abs @TOS\n");
  fprintf (mf,";\tREAL @TOS -> abs @TOS\n");
  mStack.CurrentRealPtr(PerNum);
  unsigned long last = PerNum - 1;
  fprintf (mf, "  %%%ld = load float, float* %%%ld, align 2\n", PerNum, PerNum-1);
  PerNum += 1;
  fprintf (mf, "  %%%ld = tail call float @fabsf(float %%%ld)\n", PerNum, PerNum-1);
  PerNum += 1;
  fprintf (mf, "  store float %%%ld, float* %%%ld, align 4\n", PerNum-1, last);
}
void LLVMProcessor::c_truncr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tREAL @TOS -> abs @TOS\n");
  fprintf (mf,";\tREAL @TOS -> abs @TOS\n");
  mStack.CurrentRealPtr(PerNum);
  unsigned long last = PerNum - 1;
  fprintf (mf, "  %%%ld = load float, float* %%%ld, align 2\n", PerNum, PerNum-1);
  PerNum += 1;
  fprintf (mf, "  %%%ld = tail call float @truncf(float %%%ld)\n", PerNum, PerNum-1);
  PerNum += 1;
  fprintf (mf, "  store float %%%ld, float* %%%ld, align 4\n", PerNum-1, last);
}

/** *********************************************************************************************************/
void LLVMProcessor::c_testpole() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned long a = SemPop();  // adresa konce pole v bytech
  trace       ("\tTEST POLE %04lX; --TOS\n", a);
  fprintf (mf,";\tTEST POLE %04lX; --TOS\n", a);
  mStack.PopWord(PerNum);
  fprintf(mf,"  %%%ld = ashr i16 %%%ld, 1\n", PerNum, PerNum-1);                 // index ve WORD
  PerNum += 1; /// vrati omezenou adresu WORD, par: pozadavek, konec [ve WORD adress]
  fprintf(mf,"  %%%ld = tail call i16 @TestPole (i16 %%%ld, i16 %d)\n\n",
          PerNum, PerNum-1, (short) a >> 1);
  PerNum += 1;
  fprintf(mf,
    "  %%%ld = getelementptr inbounds %s, %s* %s, %s 0, i32 0, i16 %%%ld\n",    // OK, mame word ptr
    PerNum, ramdef, ramdef, vardef, iFmt, PerNum-1);
  PerNum += 1;  /// zde se vraci ptr na prvek pole, se kterym se dal bude pracovat
  fromField = true;
}
/** *********************************************************************************************************/
void LLVMProcessor::c_ldwi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace        ("\tLOAD  INDIRECT WORD; ++TOS\n");
  fprintf  (mf,";\tLOAD  INDIRECT WORD; ++TOS\n");
  fprintf(mf, "  %%%ld = load i16, i16* %%%ld, align 2\n", PerNum, PerNum-1);         // data
  PerNum += 1;
  mStack.PushWordAtCurrent(PerNum);
  fromField = false;
}
void LLVMProcessor::c_stwi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tSTORE INDIRECT WORD; --TOS ;\n");
  fprintf (mf,";\tSTORE INDIRECT WORD; --TOS ;\n");
  mStack.PopWordAtCurrent(PerNum);
  mStack.ZeroTest();
  fromField = false;
}
void LLVMProcessor::c_ldwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned long a = SemPop();
  trace       ("\tLOAD IMM WORD 0x%04lX -> @TOS; TOS++\n", a);
  fprintf (mf,";\tLOAD IMM WORD 0x%04lX -> @TOS; TOS++\n", a);
  mStack.PushImmWord (a, PerNum);
}
void LLVMProcessor::c_ldwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  
  trace       ("\tLOAD WORD @W%04lX -> @TOS; TOS++\n", n >> 4);
  fprintf (mf,";\tLOAD WORD @W%04lX -> @TOS; TOS++\n", n >> 4);
  getElementPtr(n >> 4);
  fprintf(mf, "  %%%ld = load i16, i16* %%%ld, align 2\n", PerNum, PerNum-1);
  PerNum += 1;
  mStack.PushWordAtCurrent(PerNum);
}
void LLVMProcessor::c_stwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  trace       ("\tSTORE @TOS -> WORD @W%04lX; --TOS\n", n >> 4);
  fprintf (mf,";\tSTORE @TOS -> WORD @W%04lX; --TOS\n", n >> 4);
  PredefAdr (n);
  getElementPtr(n >> 4);
  mStack.PopWordAtCurrent(PerNum);
  mStack.ZeroTest();
}
void LLVMProcessor::mathOpWDS (const char * op) {
  trace         ("\t! WORD @TOS-1 %s @TOS -> @TOS; --TOS;\n", op);
  fprintf (mf,"\n;\t! WORD @TOS-1 %s @TOS -> @TOS; --TOS;\n", op);
  mStack.PopWordPtr (PerNum);
  fprintf (mf,"  tail call void @%sWord(i16* %%%ld, i16* %%%ld)\n", op, PerNum-1, PerNum-3);
}

void LLVMProcessor::mathOpIM (const char * op) {
  unsigned a = SemPop();
  trace         ("\t! @TOS %s IMM WORD %04X -> @TOS\n", op, a);
  fprintf (mf,"\n;\t! @TOS %s IMM WORD %04X -> @TOS\n", op, a);
  fprintf (mf, "  store i16 %d, i16* %%RegHL, align 2\n"  , a);
  fprintf (mf, "  tail call void @%sWord(i16* %%%ld, i16* %%RegHL)\n", op, PerNum-1);
}

void LLVMProcessor::mathOpWD (const char * op, long unsigned int n) {
  PredefAdr (n);
  trace         ("\t! WORD @W%04lX %s @TOS -> @TOS\n", n >> 4, op);
  fprintf (mf,"\n;\t! WORD @W%04lX %s @TOS -> @TOS\n", n >> 4, op);
  getElementPtr(n >> 4);
  mStack.CurrentWordPtr(PerNum);
  fprintf (mf, "  tail call void @%sWord(i16* %%%ld, i16* %%%ld)\n", op, PerNum-1, PerNum-3);
}
void LLVMProcessor::c_addwp() {
  //-fprintf (stdout, "Funkce:%s - unused\n", __FUNCTION__);
  c_addw();     // to je blbost, lze vratit
}

void LLVMProcessor::c_addwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("add", n);
}
void LLVMProcessor::c_subwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("sub", n);
}
void LLVMProcessor::c_mulwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("mul", n);
}
void LLVMProcessor::c_divwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("udiv", n);
}

void LLVMProcessor::c_addwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("add");
}
void LLVMProcessor::c_subwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("sub");
}
void LLVMProcessor::c_mulwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("mul");
}
void LLVMProcessor::c_divwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("udiv");
}

void LLVMProcessor::c_addw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("add");
}

void LLVMProcessor::c_subw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("sub");
}

void LLVMProcessor::c_mulw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("mul");
}

void LLVMProcessor::c_divw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("udiv");
}

void LLVMProcessor::c_stimmd (unsigned long n) {
  //fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  unsigned a = SemPop();        // immediate op
  trace  ("\tWORD %04X -> @W%04lX\n", a, n >> 4);
  // + sitovy WORD vola jeste sluzbu site
  getElementPtr(n >> 4);
  fprintf(mf, "  tail call void @WordChangeWrap(i16* %%%ld, i16 %d)\n", PerNum-1, (short) a);
  fprintf(mf, "  store i16 %d, i16* %%%ld, align 2\n", (short) a, PerNum-1);
}
/** *********************************************************************************************************/
void LLVMProcessor::relOpWD (const char * op) {
  trace   ("\t@TOS %s @TOS-1 -> CARRY; TOS-=2\n", op);
  mStack.PopWordPtr(PerNum);
  fprintf (mf, "  %%%ld = load i16, i16* %%%ld, align 2\n", PerNum, PerNum-3);
  PerNum += 1;
  fprintf (mf, "  %%%ld = load i16, i16* %%%ld, align 2\n", PerNum, PerNum-2);
  PerNum += 1;  /// zjistit, zda to neni obracene
  fprintf (mf, "  %%%ld = icmp %s i16 %%%ld, %%%ld\n", PerNum, op, PerNum-1, PerNum-2);
  PerNum += 1;
  fprintf (mf, "  %%%ld = zext i1 %%%ld to i16\n", PerNum, PerNum-1);
  fprintf (mf, "  store i16 %%%ld, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
  mStack.DecrementStack(1);
}
void LLVMProcessor::relOpWDIM (const char * op) {
  unsigned long a = SemPop();
  trace       ("\t@TOS %s IMM %04lX -> CARRY; --TOS\n", op, a);
  fprintf (mf,";\t@TOS %s IMM %04lX -> CARRY; --TOS\n", op, a);
  mStack.PopWord(PerNum);
  fprintf (mf, "  %%%ld = icmp %s i16 %%%ld, %ld\n", PerNum, op, PerNum-1, a);
  PerNum += 1;
  fprintf (mf, "  %%%ld = zext i1 %%%ld to i16\n", PerNum, PerNum-1);
  fprintf (mf, "  store i16 %%%ld, i16* %%carry, align 2\n", PerNum);
  PerNum += 1;
}
void LLVMProcessor::c_ltw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ult");
}
void LLVMProcessor::c_lew() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ule");
}
void LLVMProcessor::c_gtw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ugt");
}
void LLVMProcessor::c_gew() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("uge");
}
void LLVMProcessor::c_eqw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("eq");
}
void LLVMProcessor::c_new() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ne");
}


void LLVMProcessor::c_muliim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("mul");
}
void LLVMProcessor::c_mulid (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD ("mul", n);
}
void LLVMProcessor::c_muli() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("mul");
}

void LLVMProcessor::c_diviim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("sdiv");
}
void LLVMProcessor::c_divid (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("sdiv", n);
}
void LLVMProcessor::c_divi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("sdiv");
}
/** *********************************************************************************************************/
void LLVMProcessor::c_newim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWDIM ("ne");
}
void LLVMProcessor::c_eqwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWDIM ("eq");
}

/** *************************************************************************/
void LLVMProcessor::c_caseof() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_endcase() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_retb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

/** *************************************************************************/
void LLVMProcessor::c_proced (LPOLOZKA * p, int pos) {
  //-fprintf (stdout, "Funkce:%s -> \"%s\" fpl=%d\n", __FUNCTION__, p->iden, p->fpl);
  
  ProcNum += 1;
  p->atr   = ProcNum;
  tempFile = mf;
  mf = procFile;
  mStack.clear();
  
  fprintf(mf, "\n; Function Attrs: nounwind inline\n");
  fprintf(mf, "define internal void @%s(i16* %%param, i16 %%dps) #0 {\n", p->iden);
  mStack.Define (8, iFmt);
  fprintf(mf, "  %%RegHL = alloca i16, align 2\n");
  loadRamPtr();
}
void LLVMProcessor::c_preturn (LPOLOZKA * p) {
  //-fprintf (stdout, "Funkce:%s -> \"%s\"\n", __FUNCTION__, p->iden);
  fprintf (mf,"\nret void\n}\n");
  
  mf = tempFile;
  PerNum = 1;
  mStack.clear();
}
void LLVMProcessor::c_pcall (char * u) {
  //-fprintf (stdout, "Funkce:%s -> [%s]\n", __FUNCTION__, u);
  
  fprintf (mf, "  tail call void @%s(i16* %%stack, i16 %d)\n", u, mStack.depth());
  mStack.clear();
}
void LLVMProcessor::c_spar (long unsigned int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
// ono to a ukazuje vlastne za parametr - musime zde tedy znat jeho velikost
  unsigned long parno = (a>>4)-2;
  fromField = true;
  trace       ("\tOPERAND REAL %ld z PARAM -> PTR\n", parno);
  fprintf (mf,";\tOPERAND REAL %ld z PARAM -> PTR\n", parno);
  fprintf (mf, "  %%%ld = getelementptr inbounds i16, i16* %%param, i16 %ld\n", PerNum, parno);
  PerNum += 1;
}

void LLVMProcessor::c_spa (unsigned long int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
  unsigned long parno = (a>>4)-1;
  //fromField = true;
  trace       ("\tOPERAND WORD %ld z PARAM -> PTR\n", parno);
  fprintf (mf,";\tOPERAND WORD %ld z PARAM -> PTR\n", parno);
  fprintf (mf, "  %%%ld = getelementptr inbounds i16, i16* %%param, i16 %ld\n", PerNum, parno);
  PerNum += 1;
}
void LLVMProcessor::c_ldaa (unsigned long int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
  unsigned long parno = (a>>4)-1;
  fromField = true;
  trace       ("\tOPERAND @WORD %ld z PARAM -> PTR\n", parno);
  fprintf (mf,";\tOPERAND @WORD %ld z PARAM -> PTR\n", parno);
  fprintf (mf,"  %%%ld = getelementptr inbounds i16, i16* %%param, i16 %ld\n", PerNum, parno);
  PerNum += 1;
  fprintf (mf,"  %%%ld = load i16, i16* %%%ld, align 2\n", PerNum, PerNum-1);
  PerNum += 1;
  fprintf (mf,"  %%%ld = ashr i16 %%%ld, 1\n", PerNum, PerNum-1);        // index ve WORD
  PerNum += 1;
  fprintf (mf,
    "  %%%ld = getelementptr inbounds %s, %s* %s, %s 0, i32 0, i16 %%%ld\n",
    PerNum, ramdef, ramdef, vardef, iFmt, PerNum-1);
  PerNum += 1;
}

void LLVMProcessor::c_func (LPOLOZKA * p, int pos) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_freturn (LPOLOZKA * p) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_fcall (char * u) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_skpb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_allot() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
int  LLVMProcessor::c_retval() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  return 0;
}
void LLVMProcessor::c_procerror() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
/** *************************************************************************/
void LLVMProcessor::c_ldbaim (unsigned long int a) {
  fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
}

void LLVMProcessor::c_ldbaa (unsigned long int a) {
  fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
}
void LLVMProcessor::c_spba (unsigned long int a) {
  fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
}

void LLVMProcessor::c_setbi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_resbi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_invbi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_ldbi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_stbi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
/** *********************************************************************************************************/

void LLVMProcessor::c_chsi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_absi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_swap() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_lti() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_lei() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_gti() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_gei() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_disp (int) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_dispb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_prnt (int) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_prntb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_ltr() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_ler() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_gtr() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_ger() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_eqr() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_ner() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_andwim() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_andwd (unsigned long int n) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_andw() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_orwim() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_orwd (unsigned long int n) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_orw() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_xorwim() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_xorwd (unsigned long int n) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_xorw() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

/** *******************************************************************************/
void LLVMProcessor::c_disps (char * u) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_prnts (char * u) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_memwrite() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_memread() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_unknown (char * u) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
/** *******************************************************************************/
const LLVMProcessor::conv_s LLVMProcessor::RetypeConversions[] = {
  {"float","i16",4,2},
  {"i16","float",2,4}
};

void LLVMProcessor::Convert (const LLVMProcessor::conv_s * ct, const char * util) {
  if (ct->align_from == 4) mStack.CurrentRealPtr (PerNum);
  else                     mStack.CurrentWordPtr (PerNum);
  fprintf (mf, "  %%%ld = load %s, %s* %%%ld, align %d\n",  PerNum, ct->from, ct->from, PerNum-1, ct->align_from);
  PerNum += 1;
  fprintf (mf, "  %%%ld = %s %s %%%ld to %s\n", PerNum, util, ct->from, PerNum-1, ct->to);
  PerNum += 1;
  fprintf (mf, "  %%%ld = bitcast i16* %%%ld to %s*\n", PerNum, PerNum-4, ct->to);
  PerNum += 1;
  fprintf (mf, "  store %s %%%ld, %s* %%%ld, align %d\n\n", ct->to, PerNum-2, ct->to, PerNum-1, ct->align_to);
}

void LLVMProcessor::c_rtoi() {
  //fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace        ("\tREAL -> INT  @TOS\n");
  fprintf (mf, ";\tREAL -> INT  @TOS\n");
  Convert (RetypeConversions + 0,"fptosi");
}
void LLVMProcessor::c_rtow() {
  //fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace        ("\tREAL -> WORD @TOS\n");
  fprintf (mf, ";\tREAL -> WORD @TOS\n");
  Convert (RetypeConversions + 0,"fptoui");
}
void LLVMProcessor::c_itor() {
  //fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tINT  -> REAL @TOS\n");
  fprintf (mf,";\tINT  -> REAL @TOS\n");
  Convert (RetypeConversions + 1,"sitofp");
}
void LLVMProcessor::c_wtor() {
  //fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tWORD -> REAL @TOS\n");
  fprintf (mf,";\tWORD -> REAL @TOS\n");
  Convert (RetypeConversions + 1,"uitofp");
}


