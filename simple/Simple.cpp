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

static int Exec (const char * cmd) {
  int res = system(cmd);
  printf ("Command : \"%s\"  ", cmd);
  if (res) printf ("failed\n");
  else     printf ("OK\n");
  return res;
}
static void Linking (LLVMTypeMachine m) {
  const int max = 1024;
  char soname[max], oname[max], hname[max];
//snprintf (soname, max, "libpes.so");
  snprintf (soname, max, "%s.so", MacF);
  snprintf ( oname, max, "%s.o",  MacF);
  snprintf ( hname, max, "%s.hex",MacF);
  const char * prefix = "arm-none-eabi-";
  const char * procty [2] = {"cortex-m3","cortex-m4"}; 
  char cmd [max];
  if (m == MachineTypeLinux64) {
    snprintf (cmd, max, "as %s -o %s", MacF, oname);
    if (Exec(cmd)) return;
    snprintf (cmd, max, "ld -shared -fPIC %s -o %s", oname, soname);
    if (Exec(cmd)) return;
  } else {
    snprintf (cmd, max, "%sas %s -o %s", prefix, MacF, oname);
    if (Exec(cmd)) return;
    snprintf (cmd, max, "%sgcc -mthumb -mcpu=%s -Wl,--gc-sections,-Map=pes.map -T script.ld %s -o %s",
              prefix, procty[m-1], oname, soname);
    if (Exec(cmd)) return;
    snprintf (cmd, max, "%ssize %s", prefix, soname);
    if (Exec(cmd)) return;
    snprintf (cmd, max, "%sobjcopy --strip-unneeded -O ihex %s %s", prefix, soname, hname);
    if (Exec(cmd)) return;
  }
  remove (oname);
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
    char str[0xFF];
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
    char  str[0xFF];
    snprintf (str, 0xFF,"Soubor \'%s\' nelze nahradit.", MacF);
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

/*###########################################################################*/
/*                                                                           */
/*   Nezbytne inicializace a hlavni program                                  */
/*                                                                           */
/*###########################################################################*/
int Simple (const char * name, const unsigned flags) {
  char * arg = strdup (name);
  argm = arg;

  CompilerFlags cf;
  cf.Common = flags;
  switch (cf.F.TGT) {
  case MachineTypeLinux64:
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
      char copy[1024];
      strncpy (copy, MacF, 1024);
      strcat  (copy, ".ll");
      CompileLLtoASFile (copy, MacF, cf.F.TGT);
      Linking (cf.F.TGT);
    }
    fclose (ef);
  } catch (CError ex) {
    AddInfo (ex.GetError());
    CloseAllFiles (arg);
    FreeConfig();
    return ex.GetCode();
  }

  CloseAllFiles (arg);
  FreeConfig();

  AddInfo ("Preklad uspesne dokoncen\n");

  delete BaseProcessorPtr;
  printf ("\n");
  return 0;
}
