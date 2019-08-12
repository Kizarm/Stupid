/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   Prekladac                                                               */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
#include "Compiler.h"
#include "Optimizer.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
//#include <boost/concept_check.hpp>

#include "inc/chyby.h"
#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/8051Processor.h"
#include "inc/LLVMProcessor.h"
#include "inc/fce.h"
#include "inc/error.h"

#include "inc/CfgTypes.h"

#include "../lib/AsmApi.h"
#include "./llcapi/llcapi.h"

#define   ERRFILE "Errlist.txt"

BaseProcessor * BaseProcessorPtr;
extern "C" const char * tscript;

FILE * ef;
FILE * of,*sf;
const char * argm;
void (*LogFce) (const char * buff); // pointer na funkci, ktera je volana pri hlaseni

#define FILENAME_MAX_LEN  1024
char   InpF[FILENAME_MAX_LEN];
char   MacF[FILENAME_MAX_LEN];
char   MaoF[FILENAME_MAX_LEN];
char   SymF[FILENAME_MAX_LEN];
char   ObjF[FILENAME_MAX_LEN];
char   HexF[FILENAME_MAX_LEN];
char   DnlF[FILENAME_MAX_LEN];
char   SupF[FILENAME_MAX_LEN];
char   SysF[FILENAME_MAX_LEN];
char   AsmF[FILENAME_MAX_LEN];
char   ErrF[FILENAME_MAX_LEN];
char   CfgF[FILENAME_MAX_LEN];

extern int    ListF,MCodeF,SrcF,VerbF,OptF;
extern long int  CSize,TSize ;
extern char t[80];

static POLOZKA * LastSystem = NULL;

char  LastOpenedFile[80];

static void MakeDnl();
#ifdef UNIX
#include <unistd.h>
/// Linux - replace std system() call - redirect stdout to user fnc.
static int RunCmd (const char * cmd) {
  int pipefd[2], result;
  result = pipe (pipefd);
  if (result) return result;

  pid_t cpid = fork();
  if (cpid < 0) return -1;
  if (cpid == 0) {   // child
    close (pipefd[0]);    // close reading end in the child
    dup2  (pipefd[1], 1); // send stdout to the pipe
//  dup2  (pipefd[1], 2); // send stderr to the pipe
    result = execl ("/bin/sh", "sh", "-c", cmd, (char *) 0);
    close (pipefd[1]);    // this descriptor is no longer needed
  } else {           // parent
    const unsigned max = 1024;
    char buffer [max + 4];
    close (pipefd[1]); // close the write end of the pipe in the parent
    for (;;) {
      int n = read (pipefd[0], buffer, max);
      if (n <= 0) break;
      buffer[n] = '\0';
      AddInfo (buffer);
    }
    close (pipefd[0]);
  }
  return result;
}
#else
static inline int RunCmd (const char * cmd) {
  return system(cmd);
}
#endif // UNIX

static int Exec (const char * cmd) {
  int len = 48 - strlen(cmd);
  if (len < 0) len = 0;
  int res = RunCmd (cmd);
  AddInfo ("Command : \"%s\"  ", cmd);
  if (res) AddInfo ("%*sfailed\n", len, " ");
  else     AddInfo ("%*sOK\n",     len, " ");
  return res;
}
static int Linking (LLVMTypeMachine m) {
  const int maxn = strlen(MacF) + 10;
  char soname[maxn], oname[maxn];//, hname[maxn];
//snprintf (soname, maxn, "libpes.so");
  snprintf (soname, maxn, "%s.so", MacF);
  snprintf ( oname, maxn, "%s.o",  MacF);
//snprintf ( hname, maxn, "%s.hex",MacF);
  // na RPi je lepe nainstalobat take gcc-arm-none-eabi, jde to
  const char * prefix = "arm-none-eabi-";
  const char * procty [3] = {"cortex-m0","cortex-m3","cortex-m4"}; 
  const int max = 0x1000;
  char cmd [max];
  if (m == MachineTypeLinux64) {
    snprintf (cmd, max, "as %s -o %s", MacF, oname);
    if (Exec(cmd)) return 1;
    snprintf (cmd, max, "ld -shared -fPIC %s -o %s", oname, soname);
    if (Exec(cmd)) return 1;
  } else {
    const char * sname = "script.ld";
    FILE * sc = fopen(sname,"w");
    fprintf(sc,"%s",tscript);
    fclose (sc);
    snprintf (cmd, max, "%sas %s -o %s", prefix, MacF, oname);
    if (Exec(cmd)) return 1;
    snprintf (cmd, max, "%sgcc -nostartfiles -mthumb -mcpu=%s -Wl,--gc-sections,-Map=pes.map -T %s %s -o %s",
              prefix, procty[m-1], sname, oname, soname);
    if (Exec(cmd)) return 1;
    snprintf (cmd, max, "%ssize %s", prefix, soname);
    if (Exec(cmd)) return 1;
    snprintf (cmd, max, "%sobjcopy --strip-unneeded -O ihex %s %s", prefix, soname, HexF);
    if (Exec(cmd)) return 1;
    remove (sname);
    MakeDnl();
  }
  remove (oname);
  return 0;
}

/** Workaround pro překrývající se stringy v strcpy()
 * */
extern "C" {
  char * ByteCopy (char * dst, const char * src) {
    int n = strlen (src);
    for (int i=0; i<n; i++) dst[i] = src[i];
    dst[n] = '\0';
    return dst;
  }
}

/*---------------------------------------------------------------------------*/
/* kroky po prekladu                                                         */
/*---------------------------------------------------------------------------*/

static void GenListFile() {
  if (ListF) {
    AddInfo ("\nGeneruje se %s",SymF);
//   fflush (stdout);
    sf=fopen (SymF,"w");
    if (sf==NULL) ErrFile (SymF);
    SeznamList (sf,LastSystem);    /* pouze uzivatelem definovane           */
    /*   SeznamList(sf,NULL);   */      /* vsechny identifikatory                */
    /*   CSourceGen();          */
    fclose (sf);
  } else {
    remove (SymF);
  }
}

/*---------------------------------------------------------------------------*/
/* spusteni stpasm.exe                                                       */
/*---------------------------------------------------------------------------*/

static bool Assembler() {

  AddInfo ("\n");

  if (!access (MacF, R_OK)) {
    if (AsmHexApi (MacF)) return false;
    printf (" Asm OK.\n");
  }
  //-printf ("SrcF=%d, MacF=%s, ObjF=%s, HexF=%s.\n", SrcF, MacF, ObjF, HexF);
  if (!access (ObjF, W_OK)) {
    remove (ObjF);
  }
  return true;
}

/*---------------------------------------------------------------------------*/
/* vytvoreni souboru *.dnl                                                   */
/*---------------------------------------------------------------------------*/

extern unsigned long RamPtr ;
extern long int  RomBegin,RamBegin;


static void MakeDnl() {
  long int a,b;

  a=RamPtr>>3;
  b=RamBegin>>3;

  of=fopen (HexF,"a");
  if (of==NULL) ErrFile (HexF);
  fprintf (of,"\n");
  fprintf (of,"\n#Version 3.00");
  if (NetAdr != 31)
    fprintf (of,"\n#Station %d",NetAdr);
  fprintf (of,"\n#BaudRate 0");
  fprintf (of,"\n#DataBegin %ld",b);
  fprintf (of,"\n#DataSize  %ld", (a-b));
  fprintf (of,"\n");
  PredefVypis (of);
  VarsVypis (of,LastSystem);
  fclose (of);

  remove (DnlF);
  if (rename (HexF,DnlF) != 0) {
    char str[0x1000];
    sprintf (str,"Soubor \'%s\' nelze nahradit.", DnlF);
    ErrFatal (str);
  }
}

/*---------------------------------------------------------------------------*/
/* optimalizace souboru *.mac                                                */
/*---------------------------------------------------------------------------*/
static bool OptimizeMac() {
  //- long  line = 0;
  try {
    COptimizer  Opt (MacF);

    if (!Opt.RunOptimize())
      return false;
  } catch (CError & err) {
    printf ("%s", err.GetError());
  }
  AddInfo ("\nOptimalizace ukoncena");
  remove (MacF);
  //-printf  ("Optimalizace ukoncena - rename (%s,%s)\n", MaoF,MacF);
  if (rename (MaoF,MacF) < 0) {
    char  str[0x1000];
    snprintf (str, 0x01000,"Soubor \'%s\' nelze nahradit.", MacF);
    ErrFatal (str);
  }
  return true;
}

static void CloseAllFiles (char * name) {
  char  fail = 0;
  while (ZnakFn) {
    fail++;
    (*FSez[ZnakFn].cl) ();
    if (fail == 0)
      break;
  }
  free (name);
}
static void InitFlags (const unsigned flags) {
  CompilerFlags cf;
  cf.Common = flags;
  if (cf.F.L) ListF  = ~0;
  if (cf.F.M) MCodeF = ~0;
  if (cf.F.S) SrcF   = ~0;
  if (cf.F.V) VerbF  = ~0;
  if (cf.F.O) OptF   = ~0;
  if (cf.F.RAM) RamBegin = cf.F.RAM << 3;
}
int CompileLLtoASFile (const char * infile, const char * outfile, LLVMTypeMachine f) {
  const unsigned max = 1024;
  char   cmdbuf [max];
#if PC64
  const char * llc = "llc-3.8";
#else
  const char * llc = "llc";
#endif
  switch (f) {
    case MachineTypeLinux64:
      snprintf (cmdbuf, max, "%s -O3 -relocation-model=pic %s -o %s",
                llc, infile, outfile);
      break;
    case MachineTypeCortexM0:
      snprintf (cmdbuf, max, "%s -O3 -function-sections -mcpu=cortex-m0 %s -o %s",
                llc, infile, outfile);
      break;
    case MachineTypeCortexM3:
      snprintf (cmdbuf, max, "%s -O3 -function-sections -mcpu=cortex-m3 %s -o %s",
                llc, infile, outfile);
      break;
    case MachineTypeCortexM4F:
      snprintf (cmdbuf, max, "%s -O3 -function-sections -mcpu=cortex-m4 -mattr=+fp-only-sp,+d16 %s -o %s",
                llc, infile, outfile);
      break;
    default:
      return 1;
  }
  return Exec(cmdbuf);
}
/*###########################################################################*/
/*                                                                           */
/*   Nezbytne inicializace a hlavni program                                  */
/*                                                                           */
/*###########################################################################*/
int Simple (const char * name, const unsigned flags) {
  setlocale (LC_NUMERIC, "POSIX");

  char * arg = strdup (name);
  int result = 0;
  argm = arg;

  CompilerFlags cf;
  cf.Common = flags;
  switch (cf.F.TGT) {
  case MachineTypeLinux64:
  case MachineTypeCortexM0:
  case MachineTypeCortexM3:
  case MachineTypeCortexM4F:
    BaseProcessorPtr = new  LLVMProcessor (cf.F.TGT);
    break;
  default:
    BaseProcessorPtr = new I8051Processor();
    break;
  }

  NetAdr = 31;
  RamBegin = 0;
  RomBegin = 0;
  RomEnd = 0xFFFF;
  RamEnd = 0x8000;

  config_error = false;

  LastOpenedFile[0] = 0;

  AddInfo ("Kompilator    SIMPLE 4.23 ---- (C) HYPEL(r) 1998-2009\n");
  AddInfo ("Posix gcc version derived ---- (C) KIZARM   2015-2016\n");

  try {
    AnalCmdLine();
    InitFlags (flags);

    //-printf ("InpF=%s, ErrF=%s, MacF=%s, CfgF=%s,\n", InpF, ErrF, MacF, CfgF);

    if (access (InpF, R_OK)) {
      throw CFileError (InpF);
    }

    ef = fopen (ErrF, "w");
    if (ef == NULL) {
      throw CFileError (ErrF);
    }

    of = fopen (MacF, "w");
    if (of == NULL) {
      ErrFile (MacF);
    }

    PredefInit();
    ZnakInit();
    SezInit();
    ZasInit();
    TypesInit();

    LastSystem=Last;

    ReadConfigFile (CfgF);
    ZnakInit();
    FileInit (InpF);
    Program();
    if (cf.F.TGT == LLVMTypesMax) {

      GenListFile();

      ///OptF = 1;
      if (OptF) OptimizeMac();

      if (!Assembler())
        return -1;
      MakeDnl();

    } else {
      const int maxi = strlen(MacF) + 10;
      char copy[maxi];
      strncpy (copy, MacF,  maxi);
      strncat (copy, ".ll", maxi);
      result = CompileLLtoASFile (copy, MacF, cf.F.TGT);
      if (!result) {
        //remove (copy);
        result = Linking (cf.F.TGT);
      }
    }
    fclose (ef);
  } catch (CError & ex) {
    AddInfo (ex.GetError());
    CloseAllFiles (arg);
    FreeConfig();
    return ex.GetCode();
  }

  CloseAllFiles (arg);
  FreeConfig();

  remove (ErrF);
  AddInfo ("Preklad uspesne dokoncen\n");

  delete BaseProcessorPtr;
  printf ("\n");
  return result;
}
