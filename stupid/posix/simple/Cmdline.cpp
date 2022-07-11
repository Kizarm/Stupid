/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   Rozbor prikazoveho radku                                                */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <string.h>
//-#include <process.h>

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/fce.h"
#include "inc/error.h"

#define FILENAME_MAX_LEN  1024

extern const char   *   argm;
extern char      InpF[FILENAME_MAX_LEN];
extern char      MacF[FILENAME_MAX_LEN];
extern char      MaoF[FILENAME_MAX_LEN];
extern char      SymF[FILENAME_MAX_LEN];
extern char      ObjF[FILENAME_MAX_LEN];
extern char      HexF[FILENAME_MAX_LEN];
extern char      DnlF[FILENAME_MAX_LEN];
extern char      SupF[FILENAME_MAX_LEN];
extern char      SysF[FILENAME_MAX_LEN];
extern char      AsmF[FILENAME_MAX_LEN];
extern char      ErrF[FILENAME_MAX_LEN];
extern char      CfgF[FILENAME_MAX_LEN];
extern long int  RamBegin;

int    ListF = 0,MCodeF = 0,SrcF = 0,VerbF = 0,OptF = 0;

/*---------------------------------------------------------------------------*/
/*    Rozbor prikazoveho radku                                               */
/*---------------------------------------------------------------------------*/

static void Drive (char * s) {
  if ( (ZnakCti()  >= 'A') && (znak_posl <= 'Z')) {
    *s++ = znak_posl ;
    if (ZnakCti() == ':') {
      *s++=znak_posl;
      *s='\0';
      Pravda();
      return;
    }
  }
  nepravda();
}

/*---------------------------------------------------------------------------*/

static void NameLetter() {
  if (isalpha (ZnakCti())  ||
      isdigit (znak_posl) ||
      (znak_posl == '~') ||
      (znak_posl  == ' ') ||
      (znak_posl  == '_') ||
      (znak_posl  == '-')
     ) {
    Pravda();
  } else nepravda();
}

/*---------------------------------------------------------------------------*/

static void TrimAll (char * s) {
  char  t[255], *v = s;

  while (*v++ == ' ')
    ;
  --v;
  strcpy (t, v);

  v = t;
  v += strlen (v);

  while (*--v == ' ')
    ;
  ++v;

  *v = 0;

  strcpy (s, t);
}


/*---------------------------------------------------------------------------*/

static void Name (char * s) {
  char t[255], *v;
  int  i=0;
  v=t;

  je();
  {
    NameLetter();
    if (pkk()) {
      *v++ = znak_posl;
      i++;
    }
    if (zrtz()) {
      _sled();
      do {
        NameLetter();
        if (pkk()) {
          *v++ = znak_posl;
          i++;
        }
      } while (sled_());
    }
  }
  vse();
  if (pkk()) {
    *v='\0';
    TrimAll (t);
    strcpy (s,t);
  }
}

/*---------------------------------------------------------------------------*/

static void SubDir (char * s) {
  char n[255],e[255];

  je();
  {
    Name (n);
    if (zrtz()) {
      je();
      {
        KlSlovo (".");
        if (zrtz()) {
          Name (e);
          if (zrtz()) {
            KlSlovo ("\\");
            if (pkk()) {
              strcpy (s,n);
              strcat (s,".");
              strcat (s,e);
              strcat (s,"\\");
            }
          }
        }
        if (neb()) {
          KlSlovo ("\\");
          if (pkk()) {
            strcpy (s,n);
            strcat (s,"\\");
          }
        }
      }
      vse();
    }
    if (neb()) {
      KlSlovo ("..\\");
      if (pkk()) {
        strcpy (s,"..\\");
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/

void FName (char * s,char * e) {
  char t[1024];
  char * p = t;
  strcpy (s,"");
  strcpy (e,"");
  int n;

  je();
  {
    while (/*(int)ZnakCtiMV() != -1*/ true) {
      ZnakCtiMV();
      if (znak_posl == (char) -1) break;
      *p++ = znak_posl;
    }
    *p = 0;
    strcpy (s,t);
    p = strrchr (s,'.');
    *p = 0;
    ++p;
    strcpy (e,p);
    n = strlen (p)-1;   /// divne
    e[n] = '\0';        /// ale bez toho nechodi
  }
  vse();
  //-printf ("Fname [%s][%s], %d\n",s, e, n);
}

/*---------------------------------------------------------------------------*/
void ExPath (char * s) {
  char t[20];
  strcpy (s,"");

  je();
  {
    je();
    {
      Drive (t);
      if (pkk()) {
        strcat (s,t);
      }
      if (neb()) {
        nic();
      }
    }
    vse();
    if (zrtz()) {
      je();
      {
        KlSlovo ("\\");
        if (pkk()) {
          strcat (s,"\\");
        }
        if (neb()) {
          nic();
        }
      }
      vse();
      if (zrtz()) {
        _sled();
        do {
          SubDir (t);
          if (pkk()) {
            strcat (s,t);
          }
        } while (sled_());
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/

static void GetPars() {
  char fn[FILENAME_MAX_LEN],ex[10];
/// parsovani je zbytecne, ale lze ho ponechat
  je();
  {
    Mezery();
    _sled();
    do {
      KlSlovo ("/L");
      if (pkk()) {
        ListF =~0;
      }
      if (neb()) {
        KlSlovo ("/M");
        if (pkk()) {
          MCodeF=~0;
        }
        if (neb()) {
          KlSlovo ("/S");
          if (pkk()) {
            SrcF  =~0;
          }
          if (neb()) {
            KlSlovo ("/V");
            if (pkk()) {
              VerbF =~0;
            }
            if (neb()) {
              KlSlovo ("/O");
              if (pkk()) {
                OptF =~0;
              }
              if (neb()) {
                KlSlovo ("/RAM=");
                if (zrtz()) {
                  KladneCislo();
                  if (pkk()) {
                    RamBegin=SemPop() <<3;
                  }
                }
              }
            }
          }
        }
      }
    } while (sled_());
    if (zrtz()) {
      FName (fn,ex);
    }
  }
  vse();
  //-printf ("fn=%s, ex=%s,\n", fn, ex);

  if (pkk()) {
    strcat (fn,".");
    strcpy (InpF,fn);
    strcat (InpF,ex);
    strcpy (MacF,fn);
    strcat (MacF,"mac");
    strcpy (MaoF,fn);
    strcat (MaoF,"mao");
    strcpy (SymF,fn);
    strcat (SymF,"sym");
    strcpy (ObjF,fn);
    strcat (ObjF,"o");
    strcpy (HexF,fn);
    strcat (HexF,"hex");
    strcpy (DnlF,fn);
    strcat (DnlF,"dnl");
    strcpy (ErrF,fn);
    strcat (ErrF,"err");
  }
}

/*---------------------------------------------------------------------------*/
/*  Analyza prikazoveho radku                                                */
/*---------------------------------------------------------------------------*/
#ifdef UNIX
#include <unistd.h>         // readlink
static int findLastSlash (char * name) {
  const size_t len = strlen (name);
  if (len == 0) return 0;
  int n = len - 1;
  while (n) {
    if (name [n] == '/') break;
    n -= 1;
  }
  n += 1;
  name [n] = '\0';
  return n;
}
static char * GetConfFileName (const char * filename) {
  const int PATH_MAX = 0x1000;
  char mpath[PATH_MAX];
  memset (mpath, 0, PATH_MAX);
  ssize_t count = readlink ("/proc/self/exe", mpath, PATH_MAX);
  if (count > 0) {
    findLastSlash (mpath);
    strcat (mpath, filename);
  } else {
    fprintf (stderr, "error readlink\n");
    strcat (mpath, filename);
  }
  return strdup (mpath);
}
#else
static char * GetConfFileName (const char * filename) {
  return strdup (filename);
}
#endif

void AnalCmdLine() {
  static char  CmdLine[256];
  const char * ExeName = "Simple.exe";
  int          i = 0;

  ListF = 0;
  MCodeF = 0;
  SrcF = 0;
  VerbF = 0;
  OptF = 0;

  strcpy (CmdLine,"");
  strcat (CmdLine,argm + i);
  //-printf ("CmdLine=%s, ExeName=%s, i=%d\n", CmdLine, ExeName, i);

  StringInit (ExeName);
  ExPath (SupF);
  strcpy (SysF,SupF);
  strcpy (AsmF,SupF);
  strcpy (CfgF,SupF);
  strcat (SupF,"START.FMW");
  strcat (SysF,"SYSTEM.INC");
  strcat (AsmF,"STPASM.EXE");
  char * confFileName = GetConfFileName ("simple.scf");
  strcat (CfgF,confFileName);
  free (confFileName);

  StringInit (CmdLine);
  GetPars();

  if (!uspech) {
    /*
         fprintf(stderr,"\n");
         fprintf(stderr,"\nChyba v prikazovem radku !");
         fprintf(stderr,"\n");
         fprintf(stderr,"\nVolani : simple.exe [options] source_file");
         fprintf(stderr,"\nOptions: /L ... generuje se vypis symbolu");
         fprintf(stderr,"\n         /S ... generuje se assembler    ");
         fprintf(stderr,"\n         /V ... vypisuje jmena procedur a funkci");
         fprintf(stderr,"\n");

    */
    /*    fprintf(stdout,"\n");
        fprintf(stdout,"\nChyba v prikazovem radku !");
        fprintf(stdout,"\n");
        fprintf(stdout,"\nVolani : simple.exe [options] source_file");
        fprintf(stdout,"\nOptions: /L ... generuje se vypis symbolu");
        fprintf(stdout,"\n         /S ... generuje se assembler    ");
        fprintf(stdout,"\n         /V ... vypisuje jmena procedur a funkci");
        fprintf(stdout,"\n");
    */    //   fflush (stderr);
//    fflush (stdout);
    throw CError ("Chyba v prikazovem radku !");
  }
}
