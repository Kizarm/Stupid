/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   implementace                                                            */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* 06.10.97 - vyuziti registru tak, aby bylo v souladu s C51                 */
/*          - eliminovano nasobeni a deleni jednickou                        */
/*          - opravena chyba v ldbaim()                                      */
/*          - vysledek logickeho vyrazu je pri indirect ukladani vzdy        */
/*            ocekavan ne v C, ale v _LAC                                    */
/* 07.10.97 - prechod na WORD relace ve firmware                             */
/* 08.10.97 - pridelano WADD, WSUB, WMULT, WDIV atd, ktere maji jeden oper.  */
/*            na zasobniku.                                                  */
/* 09.10.97 - procedury a funkce dostavaji zastupna jmena, abychom se vyhnuli*/
/*            kolizim s vyhrazenymi jmeny assembleru a take nemeli potize    */
/*            s dlouhymi jmeny                                               */
/*          - hlasi se pocet procedur a funkci                               */
/*          - c_lit                                                          */
/* 10.10.97 - test pole                                                      */
/* 22.10.97 - rozliseni operandu REAL pri push a pop                         */
/* 25.10.97 - Odstranen start.fmw - volani systemu opreno o tabulku ve       */
/*            firmware.h                                                     */
/*                                                                           */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>

#include "inc/Znak.h"
//#include "inc/list.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/8051Processor.h"
#include "inc/firmware.h"
#include "inc/fce.h"
#include "inc/chyby.h"

extern char   InpF[80];
extern char   MacF[80];
extern char   SymF[80];
extern char   ObjF [80];
extern char   HexF[80];
extern char   DnlF[80];
extern char   SupF[80];
extern int    ListF,MCodeF,SrcF,VerbF;
extern FILE  * of;

FILE  * mf;
/*
static FILE  *tf,*pf;
static short  Stack[256];
static int    HLSt,HASt,Soff,Sp,LReg;
static int    MaxSp;
static int    LblNum,TxtNum,ProcNum,FuncNum ;
static int    FlagLJ;

static long int  CSize,LSize,TSize ;
static char t[80];
*/
/*---------------------------------------------------------------------------*/
/*   Pomocne promenne pro hlidani obsahu registru DPTR a ACC */
/*---------------------------------------------------------------------------*/
unsigned char ACC = 0;    // obsah ACC
char      DPTRUsed = 0; // byl uz reg. DPTR pouzit ?
char      ACCUsed = 0;  // byl uz reg. ACC pouzit ?

/*typedef*/ struct byte2 {
  unsigned short lb;
  unsigned short hb;
};
/*
static  union dp {
  unsigned short  DPTR;
  struct  byte2 b2;
} DPTR ; // obsah DPTR
*/
/*---------------------------------------------------------------------------*/
/*   Alokace promennych                                                      */
/*---------------------------------------------------------------------------*/

int     NetAdr = 31;
int     MaxSpInModule;
long int  AutoPtr;
unsigned long RamPtr ;
long int  RomBegin,RamBegin;
unsigned long RomEnd, RamEnd;

/*---------------------------------------------------------------------------*/
/*  Adresy vnejsich funkci a vazba na C51 a na firmware                      */
/*---------------------------------------------------------------------------*/

/* vzhledem ke kompatibilite s C51 je treba vyuzivat registry nasledovne:    */
/*                                                                           */
/* R7   LSB prvniho parametru                                                */
/* R6   MSB prvniho parametru                                                */
/* R5   LSB druheho parametru                                                */
/* R4   MSB druheho parametru                                                */


#define R1L       "R7"
#define R1H       "R6"
#define R2L       "R5"
#define R2H       "R4"

#define MAX_STACK (250)


/*---------------------------------------------------------------------------*/
/*   Pragma                                                                  */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_PragmaLj() {
  FlagLJ=~0;
}
void I8051Processor::c_PragmaSj() {
  FlagLJ= 0;
}

/*---------------------------------------------------------------------------*/

void I8051Processor::rcall (int n) {
  fprintf (mf,"\n\tLCALL 0%XH",RomTab+3*n);
  if (MCodeF) {
    fprintf (mf,"\t;; %s",RomFName[n]);
  }
  CSize+=3;
}

void I8051Processor::rjump (int n) {
  fprintf (mf,"\n\tLJMP  0%XH",RomTab+3*n);
  if (MCodeF) {
    fprintf (mf,"\t;; %s",RomFName[n]);
  }
  CSize+=3;
}

/*---------------------------------------------------------------------------*/

void I8051Processor::FileAppend (FILE * to,FILE * from) {
  int c;
  c=getc (from);
  while (c!=EOF) {
    putc (c,to);
    c=getc (from);
  }
  fclose (from);
}

void I8051Processor::c_procinit() {
  Sp = 0;
  LReg = 0;
  HLSt = 0;
  HASt = 0;
  Soff = 0;
  MaxSp = 0;
  LblNum = 0;
  TxtNum = 0;
  ProcNum = 0;
  FuncNum = 0;
  FlagLJ = ~0;

  CSize = 0;
  LSize = 0;
  TSize = 0;

  mf=of;

  if (!RomBegin) {
    RomBegin = ROMORG;
  }
  if (!RamBegin) {
    RamBegin = RAMORG;
  }

  RamPtr=RamBegin;

  tf=fopen ("texty.mac","w");
  if (tf==NULL) ErrFile ("texty.mac");
  pf=fopen ("_proc.mac","w");
  if (pf==NULL) ErrFile ("_proc.mac");

  fprintf (tf,"\n");
  fprintf (tf,"\n;;=================== Textove konstanty ===================");
  fprintf (tf,"\n");

  fprintf (pf,"\n");
  fprintf (pf,"\n;;=================== Procedury a funkce ==================");
  fprintf (pf,"\n");

  AddInfo ("\nImplementace pro I8052, firmware HYPEL  v. 4.01\n");

  fprintf (mf,"\n;;=================== Hlavni smycka =======================");
  fprintf (mf,"\n");
  fprintf (mf,"\n_LAC\tdata\t0%XH",LAC);
  fprintf (mf,"\n_HSP\tequ\t0%XH",HSP);
  fprintf (mf,"\n\tORG\t0%lXH", (unsigned long) RomBegin);
  fprintf (mf,"\n_L%05d:",0);
}

void I8051Processor::c_procend() {
  long int a,b;
//   char c;

  rjump (SYSTEM);

  fclose (tf);
  tf=fopen ("texty.mac","r");
  if (tf==NULL) ErrFile ("texty.mac");
  fclose (pf);
  pf=fopen ("_proc.mac","r");
  if (pf==NULL) ErrFile ("_proc.mac");

  FileAppend (mf,pf);
  FileAppend (mf,tf);

  fprintf (mf,"\n\tEND\n");
  fclose (mf);

  remove ("texty.mac");
  remove ("_proc.mac");

  a=RamPtr>>3;
  b=RamBegin>>3;
  if (RamPtr&7) {
    a++;
  }

  if ( (Sp) || (LReg)) {
    sprintf (t,"\nInterni chyba kompilatoru (Sp = %d)(LReg = %d)",Sp,LReg);
    ErrFatal (t);
  }

  AddInfo ("\n");
  AddInfo ("\nAdresa stanice    ... %d",NetAdr);
  AddInfo ("\nPocet procedur    ... %d",ProcNum);
  AddInfo ("\nPocet funkci      ... %d",FuncNum);
  AddInfo ("\nDelka textu v prg ... %04lXH        ",TSize);
  printf ("  = %ld",TSize);
  AddInfo ("\nProgram           ... %04lXH",RomBegin);
  AddInfo (" - %04lXH",RomBegin+CSize);
  printf ("  = %ld",CSize);
  AddInfo ("\nPromenne          ... %04lXH",b);
  AddInfo (" - %04lXH",a);
  printf ("  = %ld", (a-b));
  AddInfo ("\nNutny zasobnik    ... %d",MaxSp);
  AddInfo ("\n");

  /*   if (RomBegin+CSize > 0xFFFF)
     {
        sprintf(t,"\nPrekrocena maximalni velikost programu a textu o %dB",RomBegin+CSize+TSize - RomEnd);
        ErrFatal(t);
     }
  */
  if (RomBegin+CSize > (long int) RomEnd) {
    sprintf (t,"\nNedostatek programove pameti. Chybi %ldB", RomBegin+CSize+TSize - RomEnd);
    Warning (t);
  }
  if (a > (long int) RamEnd) {
    sprintf (t,"\nNedostatek datove pameti. Chybi %ldB", a - RamEnd);
    Warning (t);
  }
  //flushall ();
}

extern char   MacF[80];
void I8051Processor::c_procerror() {
  if (tf != NULL) fclose (tf);
  if (tf != NULL) fclose (pf);
  remove ("texty.mac");
  remove ("_proc.mac");
  remove (MacF);
}

/*---------------------------------------------------------------------------*/

void  I8051Processor::ASp (int a) {
  int m;

  HASt+=a;
  m=HASt+Soff;
  if (m > MaxSp) {
    MaxSp=m;
    if (m > MAX_STACK) {
      StringEnd();
      ErrSynt ("Zasobnik za chodu programu pretece !");
    }
  }
  if (m > MaxSpInModule) {
    MaxSpInModule=m;
  }
}

/*---------------------------------------------------------------------------*/

void I8051Processor::LSp (int a) {
  HLSt+=a;
}

int I8051Processor::LTos() {
  if (HLSt > 8) {
    StringEnd();
    ErrSynt ("Prilis slozity logicky vyraz. Zjednoduste prosim.");
  }
  return HLSt-1;
}


/*---------------------------------------------------------------------------*/

void   I8051Processor::c_popl() {
  if (--HLSt) {
    fprintf (mf,"\n\tMOV   C,_LAC.%d",LTos());
    CSize+=2;
  }
}
void   I8051Processor::c_pushl() {
  if (HLSt) {
    fprintf (mf,"\n\tMOV   _LAC.%d,C",LTos());
    CSize+=2;
  }
}

int  I8051Processor::uber()       {
  LReg=Stack[Sp--];
  ASp (-LReg);
  return LReg;
}
void I8051Processor::pridej()     {
  Stack[++Sp]=LReg;
  ASp (LReg);
}

void I8051Processor::pop_a (void) {
  int a;
  switch (a=uber()) {
    case 0:
      break;
    case 2:
      rcall (XAPOP);
      break;
    case 4:
      rcall (XRPOP);
      break;
    default:
      sprintf (t,"\nInterni chyba 'pop_a' (LReg=%d)",LReg);
      ErrFatal (t);
  }
}

void I8051Processor::pop_b (void) {
  switch (uber()) {
    case 0:
      break;
    case 2:
      rcall (XBPOP);
      break;
    default:
      sprintf (t,"\nInterni chyba 'pop_b' (LReg=%d)",LReg);
      ErrFatal (t);
  }
}

void I8051Processor::push_a (void) {
  switch (LReg) {
    case 0:
      break;
    case 2:
      rcall (XAPUSH);
      break;
    case 4:
      rcall (XRPUSH);
      break;
    default:
      ErrFatal ("Internal error 'push_a'");
  }
  pridej();
}

/*---------------------------------------------------------------------------*/

void I8051Processor::c_lit (unsigned long a) {
  push_a();
  if (a) {
    fprintf (mf,"\n\tMOV   " R1L ",#0%lXH",  a     &0xff);
    fprintf (mf,"\n\tMOV   " R1H ",#0%lXH", (a>>8) &0xff);
    CSize+=4;
  } else {
    fprintf (mf,"\n\tCLR   A");
    fprintf (mf,"\n\tMOV   " R1L ",A");
    fprintf (mf,"\n\tMOV   " R1H ",A");
    CSize+=3;
  }
  LReg=2;
}

/*---------------------------------------------------------------------------*/
/*   Struktura IF THEN ELSE                                                  */
/*---------------------------------------------------------------------------*/

void I8051Processor::PushShortLbl() {
  SemPush (-CSize);
  SemPush (LblNum);
}
void I8051Processor::PushLongLbl() {
  SemPush (CSize);
  SemPush (LblNum);
}

void I8051Processor::PopLbl() {
  long int s;
  fprintf (mf,"\n_L%05d:", (int) SemPop());
  s=SemPop();
  if (s<0) {
    if ( (CSize+s) > 127)
      ErrSynt ("Relativni skok pres vice nez 127 bytu. Pouzijte pragma $LJ");
  }
}

void I8051Processor::c_if() {
  if (FlagLJ) {
    fprintf (mf,"\n\tJC    $+5");
    fprintf (mf,"\n\tLJMP  _L%05d",++LblNum);
    CSize+=5;
    PushLongLbl();
  } else {
    fprintf (mf,"\n\tJNC   _L%05d",++LblNum);
    CSize+=2;
    PushShortLbl();
  }
  c_popl();
}

void I8051Processor::c_else() {
//   int n;
//   long int s;
//   WriteLine2MF(0);
  fprintf (mf,"\n\tLJMP  _L%05d\t;;",++LblNum);
  CSize+=3;
  PopLbl();
  PushLongLbl();
}

void I8051Processor::c_endif() {
  //WriteLine2MF(0);
  PopLbl();
}

/*---------------------------------------------------------------------------*/
/*   Struktura CASE                                                          */
/*---------------------------------------------------------------------------*/

void I8051Processor::porovimm() {
  unsigned int a,al,ah;
  a=SemPop();
  al = a&0xff;
  ah = a>>8&0xff;

  if (ah &&  al) {
    fprintf (mf,"\n\tMOV   A," R1L);
    fprintf (mf,"\n\tXRL   A,#0%XH",al);
    fprintf (mf,"\n\tMOV   " R2L ",A");
    fprintf (mf,"\n\tMOV   A," R1H);
    fprintf (mf,"\n\tXRL   A,#0%XH",ah);
    fprintf (mf,"\n\tORL   A," R2L);
    CSize+=8;
    return;
  }

  if (!ah &&  al) {
    fprintf (mf,"\n\tMOV   A," R1L);
    fprintf (mf,"\n\tXRL   A,#0%XH",al);
    fprintf (mf,"\n\tORL   A," R1H);
    CSize+=4;
    return;
  }

  if (ah && !al) {
    fprintf (mf,"\n\tMOV   A," R1H);
    fprintf (mf,"\n\tXRL   A,#0%XH",ah);
    fprintf (mf,"\n\tORL   A," R1L);
    CSize+=4;
    return;
  }

  if (!ah && !al) {
    fprintf (mf,"\n\tMOV   A," R1L);
    fprintf (mf,"\n\tORL   A," R1H);
    CSize+=2;
    return;
  }
}

void I8051Processor::c_caseof() {
  //WriteLine2MF(0);
  porovimm();
  if (FlagLJ) {
    fprintf (mf,"\n\tJZ    $+5");
    fprintf (mf,"\n\tLJMP  _L%05d",++LblNum);
    CSize+=5;
    PushLongLbl();
  } else {
    fprintf (mf,"\n\tJNZ   _L%05d",++LblNum);
    CSize+=2;
    PushShortLbl();
  }
}

void I8051Processor::c_endcase() {
  pop_a();
}

/*---------------------------------------------------------------------------*/
/*   Procedury                                                               */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_proced (LPOLOZKA * p, int pos) {
  if (VerbF) {
    AddInfo ("\n               PROCEDURE %s",p->iden);
  }

  ProcNum+=1;
  p->atr=ProcNum;
  mf=pf;
  LSize=CSize;

  fprintf (mf,"\n");
  fprintf (mf,"\n;;-- PROCEDURE %s",p->iden);
  fprintf (mf,"\n");
  WriteMyLine2MF (pos);
  fprintf (mf,"\n_P%05d:", (int) p->atr);
  MaxSpInModule=0;
  Soff=0;
}

void I8051Processor::c_func (LPOLOZKA * p, int pos) {
  if (VerbF) {
    AddInfo ("\n               FUNCTION  %s",p->iden);
  }

  FuncNum+=1;
  p->atr=FuncNum;
  mf=pf;
  LSize=CSize;

  fprintf (mf,"\n");
  fprintf (mf,"\n;;-- FUNCTION  %s",p->iden);
  fprintf (mf,"\n");
  WriteMyLine2MF (pos);
  fprintf (mf,"\n_F%05d:", (int) p->atr);
  rcall (FSTART);
  MaxSpInModule=0;
  Soff=1; /* ukladaji se 1 byty na zasobnik po zavolani funkce */
}

void I8051Processor::c_pcall (char * u) {
  int a;
  push_a();
  fprintf (mf,"\n\tLCALL _P%05d\t;; %s", (int) sez_uk->atr,u);
  CSize+=3;
  ASp (sez_uk->hst);
  ASp (-sez_uk->hst);
  a=sez_uk->fpl;
  a=a>>3;
  while (a) {
    uber();
    a-=LReg;
  }
  pop_a();
}

void I8051Processor::c_fcall (char * u) {
  int a;
  push_a();
  fprintf (mf,"\n\tLCALL _F%05d\t;; %s", (int) sez_uk->atr,u);
  CSize+=3;
  ASp (sez_uk->hst);
  ASp (-sez_uk->hst);
  a=sez_uk->fpl;
  a=a>>3;
  while (a) {
    uber();
    a-=LReg;
  }
  uber();
}

void I8051Processor::c_preturn (LPOLOZKA * p) {
  if (AutoPtr) {
    fprintf (mf,"\n\tMOV   A,#%ld", (AutoPtr>>3));
    CSize+=2;
    rjump (PRET); /* na konci provede RET */
  } else {
    fprintf (mf,"\n\tRET");
    CSize+=1;
  }

  if (HASt) {
    sprintf (t,"\nInterni chyba kompilatoru (HASt = %d)",HASt);
    ErrFatal (t);
  }
  if ( (Sp) || (LReg)) {
    sprintf (t,"\nInterni chyba kompilatoru (Sp = %d)(LReg = %d)",Sp,LReg);
    ErrFatal (t);
  }
  mf=of;

  if (VerbF) {
    AddInfo ("\r%04LXH",CSize-LSize);
    AddInfo (" = %5ld",CSize-LSize);
    AddInfo ("  PROCEDURE %s",p->iden);
  }
}

void I8051Processor::c_freturn (LPOLOZKA * p) {
  WriteLine2MF (~0);
  if (AutoPtr) {
    fprintf (mf,"\n\tMOV   A,#%ld", (AutoPtr>>3) &0xff);
    CSize+=2;
    switch (p->rvs) {
      case  1:
        rjump (FPRETW);
        break; /* na konci provede RET */
        //case  2:
      case 16:
        rjump (FPRETW);
        break; /* na konci provede RET */
        //case  4:
      case 32:
        rjump (FPRETR);
        break; /* na konci provede RET */
      default:
        sprintf (t,"\nInterni chyba c_fret (rvs = %d)",p->rvs);
        ErrFatal (t);
    }
  } else {
    switch (p->rvs) {
      case  1:
        rjump (FRETW);
        break; /* na konci provede RET */
        //case  2:
      case 16:
        rjump (FRETW);
        break; /* na konci provede RET */
        //case  4:
      case 32:
        rjump (FRETR);
        break; /* na konci provede RET */
      default:
        sprintf (t,"\nInterni chyba c_fret (rvs = %d)",p->rvs);
        ErrFatal (t);
    }
  }

  Soff=0;

  if (HASt) {
    sprintf (t,"\nInterni chyba kompilatoru (HASt = %d)",HASt);
    ErrFatal (t);
  }
  if ( (Sp) || (LReg)) {
    sprintf (t,"\nInterni chyba kompilatoru (Sp = %d)(LReg = %d)",Sp,LReg);
    ErrFatal (t);
  }
  mf=of;

  if (VerbF) {
    AddInfo ("\r%04LXH",CSize-LSize);
    AddInfo (" = %5ld",CSize-LSize);
    AddInfo ("  FUNCTION  %s",p->iden);
  }
}

void I8051Processor::c_allot() {  /* alokace pameti na zasobniku pro vracenou hodnotu funkce */
  int a;
  a=SemPop();
  if (a!=1) {
    a=a>>3;
    push_a();
    LReg=a;
  } else    {
    push_a();
    LReg=2;
  }
}

int  I8051Processor::c_retval() {
  return 0;
}

void I8051Processor::c_skpb() {  /*--- predani skutecneho parametru typu BIT                 */
  push_a();
  fprintf (mf,"\n\tCLR   A");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  fprintf (mf,"\n\tMOV   ACC.0,C");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  CSize+=5;
  LReg=2;
  c_popl();
}

/*---------------------------------------------------------------------------*/
/*   adresove operace                                                        */
/*---------------------------------------------------------------------------*/

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Nacteni adresy                                                          */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_ldbaim (unsigned long int a) {     /*  load bit address               */
  PredefAdr (a);
  c_lit (a>>3);
  c_lit (a&7);
}

void I8051Processor::c_ldaim (unsigned long int a) {      /*  load address                   */
  PredefAdr (a);
  c_lit (a>>3);
}

/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

int I8051Processor::ConvAToOff (long int a) {
//   fprintf(mf,"\n;;^ a=%ld A=%ld HASt=%d LReg=%d",a,AutoPtr,HASt,LReg);
  return (Soff+HASt+ ( (AutoPtr-a) /8));
}

void I8051Processor::c_ldbaa (unsigned long int a) {     /*  load bit address ze stacku      */
  push_a();
  fprintf (mf,"\n\tMOV   A,#%d",ConvAToOff (a) &0xff);
  CSize+=2;
  rcall (LDBAA);
  LReg=2;
  pridej();
}

void I8051Processor::c_ldaa (unsigned long int a) {      /*  load address ze stacku          */
  push_a();
  fprintf (mf,"\n\tMOV   A,#%d",ConvAToOff (a) &0xff);
  LReg=2;
  CSize+=2;
  rcall (LDAA);
}

void I8051Processor::c_spba (unsigned long int a) {      /*  vytvor bit adr operandu na zas. */
  int o;
  push_a();
  o=ConvAToOff (a);
  if (o) {
    fprintf (mf,"\n\tMOV   A,#%d",o&0xff);
    fprintf (mf,"\n\tADD   A,R0");
    fprintf (mf,"\n\tMOV   " R1L ",A");
    CSize+=4;
  } else {
    fprintf (mf,"\n\tMOV   " R1L ",AR0");
    CSize+=2;
  }
  fprintf (mf,"\n\tMOV   " R1H ",#_HSP");
  LReg=2;
  push_a();
  fprintf (mf,"\n\tCLR   A");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  CSize+=5;
  LReg=2;
}
void I8051Processor::c_spar (long unsigned int a) {
  c_spa(a);
}

void I8051Processor::c_spa (unsigned long int a) {       /*  vytvor adr operandu na zas.     */
  int o;
  push_a();
  o=ConvAToOff (a);
  if (o) {
    fprintf (mf,"\n\tMOV   A,#%d",o&0xff);
    fprintf (mf,"\n\tADD   A,R0");
    fprintf (mf,"\n\tMOV   " R1L ",A");
    CSize+=4;
  } else {
    fprintf (mf,"\n\tMOV   " R1L ",AR0");
    CSize+=2;
  }
  fprintf (mf,"\n\tMOV   " R1H ",#_HSP");
  CSize+=2;
  LReg=2;
}


/*---------------------------------------------------------------------------*/
/*   16-bit operace                                                          */
/*---------------------------------------------------------------------------*/

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Nacteni operandu                                                        */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_ldwim() {                   /*  load immediate operand         */
  c_lit (SemPop());
}

void I8051Processor::c_ldwd (unsigned long int n) { /*  load direct operand            */
  PredefAdr (n);
  push_a();
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  CSize+=8;
  LReg=2;
}

void I8051Processor::c_ldwi() {                    /*  load indirect operand          */
  fprintf (mf,"\n\tMOV   DPL," R1L);
  fprintf (mf,"\n\tMOV   DPH," R1H);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  CSize+=9;
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Ulozeni operandu                                                        */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::SpecVar (unsigned long int n) {
  if ( (n>=192) && (n< (192+32*2))) { /* D32-D63                            */
    fprintf (mf,"\n\tMOV   R7,#%ld", (n/2) &0xff);
    CSize+=2;
    rcall (LOADNET);
  }
}

#define MORG (8*520)

void I8051Processor::SpecBit (unsigned long int n) { /* n=bitova adresa                  */
  if ( (n>=MORG+64) && (n< (MORG+128))) { /* M63-M127                         */
    fprintf (mf,"\n\tMOV   R7,#%ld", (128+n- (MORG+64)) &0xff);
    CSize+=2;
    rcall (LOADNET);
  }
}

void I8051Processor::c_stimmd (unsigned long int n) { /*  store immediate to direct var       */
  unsigned int a,al,ah;

  PredefAdr (n);

  a=SemPop();
  al = a&0xff;
  ah = a>>8;
  n=n>>3;

  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n);
  CSize+=3;
  if (ah) {
    fprintf (mf,"\n\tMOV   A,#0%XH",ah&0xff);
    CSize+=2;
  } else {
    fprintf (mf,"\n\tCLR   A");
    CSize+=1;
  }
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  fprintf (mf,"\n\tINC   DPTR");
  CSize+=2;

  if (al != ah) {
    if (al) {
      fprintf (mf,"\n\tMOV   A,#0%XH",al&0xff);
      CSize+=2;
    } else {
      fprintf (mf,"\n\tCLR   A");
      CSize+=1;
    }
  }
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  CSize+=1;

  SpecVar (n);
}

void I8051Processor::c_stwd (unsigned long int n) { /*  store direct operand           */
  PredefAdr (n);
  n=n>>3;
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n);
  fprintf (mf,"\n\tMOV   A," R1H);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOV   A," R1L);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  SpecVar (n);
  CSize+=8;
  pop_a();
}

void I8051Processor::c_stwi (/*unsigned long int n*/) { /*  store indirect operand         */
  pop_b();
  fprintf (mf,"\n\tMOV   DPL," R1L);
  fprintf (mf,"\n\tMOV   DPH," R1H);
  fprintf (mf,"\n\tMOV   A," R2H);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOV   A," R2L);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  CSize+=9;
  pop_a();
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Pricteni operandu                                                       */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_addwim() {
  unsigned int a;
  a=SemPop();
  if (a) {
    fprintf (mf,"\n\tMOV   A," R1L);
    fprintf (mf,"\n\tADD   A,#0%XH",a&0xff);
    fprintf (mf,"\n\tMOV   " R1L ",A");
    fprintf (mf,"\n\tMOV   A," R1H);
    fprintf (mf,"\n\tADDC  A,#0%XH", (a>>8) &0xff);
    fprintf (mf,"\n\tMOV   " R1H ",A");
    CSize+=8;
  }
}

void I8051Processor::c_addwd (unsigned long int n) {
  PredefAdr (n);
  n=n>>3;
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n+1);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tADD   A," R1L);
  fprintf (mf,"\n\tMOV   " R1L ",A");

  fprintf (mf,"\n\tDEC   DPL");
  if ( ( (n+1) &0xff00) != (n&0xff00)) {
    fprintf (mf,"\n\tDEC   DPH");
    CSize+=1;
  }
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tADDC  A," R1H);
  fprintf (mf,"\n\tMOV   " R1H ",A");
  CSize+=10;
}

void I8051Processor::c_addw() {
  uber();
  rcall (WADD);
}
void I8051Processor::c_addwp() {
  c_addw();
}


/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Odecteni operandu                                                       */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_subwim() {
  unsigned int a,al,ah;
  a  = -SemPop();
  al = a&0xff;
  ah = a>>8;
  fprintf (mf,"\n\tMOV   A," R1L);
  fprintf (mf,"\n\tADD   A,#0%XH",al&0xff);
  fprintf (mf,"\n\tMOV   " R1L ",A");
  fprintf (mf,"\n\tMOV   A," R1H);
  fprintf (mf,"\n\tADDC  A,#0%XH",ah&0xff);
  fprintf (mf,"\n\tMOV   " R1H ",A");
  CSize+=8;
}

void I8051Processor::c_subwd (unsigned long int n) {
  PredefAdr (n);
  n=n>>3;
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n+1);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tCLR   C");
  fprintf (mf,"\n\tXCH   A," R1L);
  fprintf (mf,"\n\tSUBB  A," R1L);
  fprintf (mf,"\n\tMOV   " R1L ",A");

  fprintf (mf,"\n\tDEC   DPL");
  if ( ( (n+1) &0xff00) != (n&0xff00)) {
    fprintf (mf,"\n\tDEC   DPH");
    CSize+=1;
  }
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tXCH   A," R1H);
  fprintf (mf,"\n\tSUBB  A," R1H);
  fprintf (mf,"\n\tMOV   " R1H ",A");
  CSize+=13;
}

void I8051Processor::c_subw() {
  uber();
  rcall (WSUB);
}


/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Nasobeni operandu                                                       */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_mulwim() {
  unsigned int a;
  a=SemPop();
  if (a==1) return;

  fprintf (mf,"\n\tMOV   " R2L ",#0%XH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%XH", (a>>8) &0xff);
  CSize+=4;
  rcall (WMULTR);
}

void I8051Processor::c_mulwd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2L ",A");
  CSize+=8;
  rcall (WMULTR);
}

void I8051Processor::c_mulw() {
  uber();
  rcall (WMULT);
}

void I8051Processor::c_muliim() {
  unsigned int a;
  a=SemPop();
  if (a==1) return;
  fprintf (mf,"\n\tMOV   " R2L ",#0%XH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%XH", (a>>8) &0xff);
  CSize+=4;
  rcall (WMULTR);
}

void I8051Processor::c_mulid (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2L ",A");
  CSize+=8;
  rcall (WMULTR);
}

void I8051Processor::c_muli() {
  uber();
  rcall (WMULT);
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Deleni   operandu                                                       */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_divwim() {
  unsigned int a;
  a=SemPop();
  if (a==1) return;

  fprintf (mf,"\n\tMOV   " R2L ",#0%XH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%XH", (a>>8) &0xff);
  CSize+=4;
  rcall (WDIVR);
}

void I8051Processor::c_divwd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2L ",A");
  CSize+=8;
  rcall (WDIVR);
}

void I8051Processor::c_divw() {
  uber();
  rcall (WDIV);
}

void I8051Processor::c_diviim() {
  unsigned int a;
  a=SemPop();
  if (a==1) return;

  fprintf (mf,"\n\tMOV   " R2L ",#0%XH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%XH", (a>>8) &0xff);
  CSize+=4;
  rcall (IDIVR);
}

void I8051Processor::c_divid (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2L ",A");
  CSize+=8;
  rcall (IDIVR);
}

void I8051Processor::c_divi() {
  uber();
  rcall (IDIV);
}

void I8051Processor::c_absi() {
  fprintf (mf,"\n\tMOV   A," R1H);
  fprintf (mf,"\n\tJNB   ACC.7,_L%05d",++LblNum);
  fprintf (mf,"\n\tMOV   A," R1L);
  fprintf (mf,"\n\tCPL   A");
  fprintf (mf,"\n\tADD   A,#1");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  fprintf (mf,"\n\tMOV   A," R1H);
  fprintf (mf,"\n\tCPL   A");
  fprintf (mf,"\n\tADDC  A,#0");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  fprintf (mf,"\n_L%05d:",LblNum);
  CSize+=12;
}

void I8051Processor::c_swap() {
  /*   fprintf(mf,"\n\tMOV   A,"R1H);
     fprintf(mf,"\n\tJNB   ACC.7,_L%05d",++LblNum);
     fprintf(mf,"\n\tMOV   A,"R1L);
     fprintf(mf,"\n\tCPL   A");
     fprintf(mf,"\n\tADD   A,#1");
     fprintf(mf,"\n\tMOV   "R1L",A");
     fprintf(mf,"\n\tMOV   A,"R1H);
     fprintf(mf,"\n\tCPL   A");
     fprintf(mf,"\n\tADDC  A,#0");
     fprintf(mf,"\n\tMOV   "R1H",A");
     fprintf(mf,"\n_L%05d:",LblNum);
  */
  fprintf (mf,"\n\tINC  A");
  CSize+=12;
}


void I8051Processor::c_chsi() {
  fprintf (mf,"\n\tMOV   A," R1L);
  fprintf (mf,"\n\tCPL   A");
  fprintf (mf,"\n\tADD   A,#1");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  fprintf (mf,"\n\tMOV   A," R1H);
  fprintf (mf,"\n\tCPL   A");
  fprintf (mf,"\n\tADDC  A,#0");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  CSize+=10;
}


/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Porovnani immediate                                                     */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/


void I8051Processor::c_eqwim() {
  porovimm();
  fprintf (mf,"\n\tADD   A,#0FFH");
  CSize+=2;
  c_notb();
  LSp (1);
  pop_a();
}
void I8051Processor::c_newim() {
  porovimm();
  fprintf (mf,"\n\tADD   A,#0FFH");
  CSize+=2;
  LSp (1);
  pop_a();
}

/*---------------------------------------------------------------------------*/
/*   Real   operace                                                          */
/*---------------------------------------------------------------------------*/

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Nacteni operandu                                                        */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_ldrim() {                   /*  load immediate operand         */
  unsigned long int a,b;
  b=SemPop();
  a=SemPop();
  push_a();
  fprintf (mf,"\n\tMOV   R7,#0%lXH", (a>>8) &0xff);
  fprintf (mf,"\n\tMOV   R6,#0%lXH",a     &0xff);
  fprintf (mf,"\n\tMOV   R5,#0%lXH", (b>>8) &0xff);
  fprintf (mf,"\n\tMOV   R4,#0%lXH",b     &0xff);
  CSize+=8;
  LReg=4;
}

void I8051Processor::c_ldri() {                    /*  load indirect operand          */
  rcall (LDRI);
  LReg=4;
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Ulozeni operandu                                                        */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/


void I8051Processor::c_stri() { /*  store indirect operand         */
  rcall (STRI);
  uber();
  pop_a();
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Operace                                                                 */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_addr()  {
  rcall (RADD);
  uber();
}
void I8051Processor::c_subr()  {
  rcall (RSUB);
  uber();
}
void I8051Processor::c_mulr()  {
  rcall (RMULT);
  uber();
}
void I8051Processor::c_divr()  {
  rcall (RDIV);
  uber();
}

void I8051Processor::c_chsr()  {
  rcall (RCHS);
}
void I8051Processor::c_absr()  {
  rcall (RABS);
}
void I8051Processor::c_truncr() {
  rcall (RTRUN);
}

void I8051Processor::c_ner()   {
  rcall (RNE);
  uber();
  LSp (1);
  pop_a();
}
void I8051Processor::c_ltr()   {
  rcall (RLT);
  uber();
  LSp (1);
  pop_a();
}
void I8051Processor::c_gtr()   {
  rcall (RGT);
  uber();
  LSp (1);
  pop_a();
}

void I8051Processor::c_ger()   {
  c_ltr();
  c_notb();
}
void I8051Processor::c_ler()   {
  c_gtr();
  c_notb();
}
void I8051Processor::c_eqr()   {
  c_ner();
  c_notb();
}

/*---------------------------------------------------------------------------*/
/*   zobrazeni                                                               */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_dispb() {
  fprintf (mf,"\n\tMOV   R3,#0H");
  CSize+=2;
  rcall (XDISP);
  c_popl();
}

void I8051Processor::c_disp (int t) {

  fprintf (mf,"\n\tMOV   R3,#0%XH",t&0xff);
  CSize+=2;
  rcall (XDISP);
  pop_a();
}

void I8051Processor::c_disps (char * u) {

  fprintf (mf,"\n\tMOV   DPTR,#_T%05d",++TxtNum);
  CSize+=3;
  rcall (SDISP);
  fprintf (tf,"\n_T%05d: DB  ",TxtNum);
  while (*u) {
    fprintf (tf,"%d,", (*u++) &0xff);
    CSize+=1;
    TSize+=1;
  }
  fprintf (tf,"0");
  CSize+=1;
  TSize+=1;
}

/*---------------------------------------------------------------------------*/
/*   tisk                                                                    */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_prntb() {
  fprintf (mf,"\n\tMOV   R3,#0H");
  rcall (XPRNT);
  c_popl();
}

void I8051Processor::c_prnt (int t) {
  fprintf (mf,"\n\tMOV   R3,#0%XH",t&0xff);
  rcall (XPRNT);
  pop_a();
}

void I8051Processor::c_prnts (char * u) {

  fprintf (mf,"\n\tMOV   DPTR,#_T%05d",++TxtNum);
  CSize+=3;
  rcall (SPRNT);
  fprintf (tf,"\n_T%05d: DB  ",TxtNum);
  while (*u) {
    fprintf (tf,"%d,", (*u++) &0xff);
    CSize+=1;
    TSize+=1;
  }
  fprintf (tf,"0");
  CSize+=1;
  TSize+=1;
}

/*---------------------------------------------------------------------------*/
/*   relace                                                                  */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_lti() {
  rcall (ILT);
  uber();
  LSp (1);
  pop_a();
}
void I8051Processor::c_ltw() {
  rcall (WLT);
  uber();
  LSp (1);
  pop_a();
}
void I8051Processor::c_gti() {
  rcall (IGT);
  uber();
  LSp (1);
  pop_a();
}
void I8051Processor::c_gtw() {
  rcall (WGT);
  uber();
  LSp (1);
  pop_a();
}
void I8051Processor::c_new() {
  rcall (WNE);
  uber();
  LSp (1);
  pop_a();
}

void I8051Processor::c_gei() {
  c_lti();
  c_notb();
}
void I8051Processor::c_gew() {
  c_ltw();
  c_notb();
}
void I8051Processor::c_lei() {
  c_gti();
  c_notb();
}
void I8051Processor::c_lew() {
  c_gtw();
  c_notb();
}
void I8051Processor::c_eqw() {
  c_new();
  c_notb();
}


/*---------------------------------------------------------------------------*/
/*   Bitove operace                                                          */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_setbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tSETB  ACC.%ld",n&7);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  CSize+=7;
  SpecBit (n);
}

void I8051Processor::c_resbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tCLR   ACC.%ld",n&7);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  CSize+=7;
  SpecBit (n);
}

void I8051Processor::c_invbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tCPL   ACC.%ld",n&7);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  CSize+=7;
  SpecBit (n);
}

void I8051Processor::c_ldbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   C,ACC.%ld",n&7);
  CSize+=6;
  LSp (1);
}

void I8051Processor::c_orbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tORL   C,ACC.%ld",n&7);
  CSize+=6;
}

void I8051Processor::c_andbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tANL   C,ACC.%ld",n&7);
  CSize+=6;
}

void I8051Processor::c_norbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tORL   C,/ACC.%ld",n&7);
  CSize+=6;
}

void I8051Processor::c_nandbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tANL   C,/ACC.%ld",n&7);
  CSize+=6;
}

void I8051Processor::c_stbd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   ACC.%ld,C",n&7);
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  CSize+=7;
  SpecBit (n);
  c_popl();
}

void I8051Processor::c_setbi() {
  rcall (SETBI);
  uber();
  pop_a();
}
void I8051Processor::c_resbi() {
  rcall (RESBI);
  uber();
  pop_a();
}
void I8051Processor::c_invbi() {
  rcall (INVBI);
  uber();
  pop_a();
}
void I8051Processor::c_ldbi() {
  rcall (LDBI);
  uber();
  pop_a();
  LSp (1);
}
void I8051Processor::c_stbi() {
  c_popl();
  rcall (STBI);
  uber();
  pop_a();
}

void I8051Processor::c_retb() {
  fprintf (mf,"\n\tMOV   A," R1H);
  fprintf (mf,"\n\tMOV   C,ACC.0");
  CSize+=3;
  pop_a();
  LSp (1);
}

void I8051Processor::c_orb() {
  LSp (-1);
  fprintf (mf,"\n\tORL   C,_LAC.%d",LTos());
  CSize+=2;
}
void I8051Processor::c_andb() {
  LSp (-1);
  fprintf (mf,"\n\tANL   C,_LAC.%d",LTos());
  CSize+=2;
}
void I8051Processor::c_notb() {
  fprintf (mf,"\n\tCPL   C");
  CSize+=1;
}
void I8051Processor::c_xorb() {
  LSp (-1);
  fprintf (mf,"\n\tJNB   _LAC.%d,_L%05d",LTos(),++LblNum);
  CSize+=3;
  fprintf (mf,"\n\tCPL   C");
  CSize+=1;
  fprintf (mf,"\n_L%05d:",LblNum);
}

/*---------------------------------------------------------------------------*/
/*   Konverze                                                                */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_itor() {
  rcall (ITOR);
  LReg=4;
}
void I8051Processor::c_wtor() {
  rcall (WTOR);
  LReg=4;
}
void I8051Processor::c_rtoi() {
  rcall (RTOI);
  LReg=2;
}
void I8051Processor::c_rtow() {
  rcall (RTOW);
  LReg=2;
}

/*---------------------------------------------------------------------------*/
/*   Na zaver                                                                */
/*---------------------------------------------------------------------------*/

void I8051Processor::c_testpole() {
  unsigned long a;
  a=SemPop();

  // printf("\n\n%ld\n\n",a);
  fprintf (mf,"\n\tMOV   " R2L ",#0%lXH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%lXH", (a>>8) &0xff);

  CSize+=4;
  rcall (TPOLE);
}

void I8051Processor::c_unknown (char * u) {
  fprintf (mf,"\n\t\t\t;;- %s",u);
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Logicke operace s operandem typu WORD                                   */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

void I8051Processor::c_andwim() {
  unsigned int a;
  a=SemPop();
  if (a==1) return;

  fprintf (mf,"\n\tMOV   " R2L ",#0%XH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%XH", (a>>8) &0xff);
  CSize+=4;
  rcall (WANDR);
}

void I8051Processor::c_andwd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2L ",A");
  CSize+=8;
  rcall (WANDR);
}

void I8051Processor::c_andw() {
  uber();
  rcall (WAND);
}

void I8051Processor::c_orwim() {
  unsigned int a;
  a=SemPop();
  if (a==1) return;

  fprintf (mf,"\n\tMOV   " R2L ",#0%XH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%XH", (a>>8) &0xff);
  CSize+=4;
  rcall (WORR);
}

void I8051Processor::c_orwd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2L ",A");
  CSize+=8;
  rcall (WORR);
}

void I8051Processor::c_orw() {
  uber();
  rcall (WOR);
}

void I8051Processor::c_xorwim() {
  unsigned int a;
  a=SemPop();
  if (a==1) return;

  fprintf (mf,"\n\tMOV   " R2L ",#0%XH",a&0xff);
  fprintf (mf,"\n\tMOV   " R2H ",#0%XH", (a>>8) &0xff);
  CSize+=4;
  rcall (WXORR);
}

void I8051Processor::c_xorwd (unsigned long int n) {
  PredefAdr (n);
  fprintf (mf,"\n\tMOV   DPTR,#0%lXH",n>>3);
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R2L ",A");
  CSize+=8;
  rcall (WXORR);
}

void I8051Processor::c_xorw() {
  uber();
  rcall (WXOR);
}

void I8051Processor::c_memwrite() {
  // nacteni docasne ulozeni hodnoty
  fprintf (mf,"\n\tMOV   A," R1H);
  fprintf (mf,"\n\tMOV   R2,A");
  fprintf (mf,"\n\tMOV   A," R1L);
  fprintf (mf,"\n\tMOV   R3,A");
  CSize+=4;

  // nacteni adresy ze zasobniku
  pop_a();

  // nacteni stranky z promene PAGEPOINTER
  fprintf (mf,"\n\tMOV   DPTR,#0121H");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   084h,A");  // mov  DPP,PAGE
  // test na hodnotu stranky < 8
  fprintf (mf,"\n\tANL   A,#0F8H");
  fprintf (mf,"\n\tJNZ   _L%05d",++LblNum);

  // nastaveni offsetu ve strance
  fprintf (mf,"\n\tMOV   DPH," R1H);
  fprintf (mf,"\n\tMOV   DPL," R1L);
  CSize+=14;

  // zapis hodnoty
  fprintf (mf,"\n\tMOV   A,R2");
  fprintf (mf,"\n\tMOVX  @DPTR,A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOV   A,R3");
  fprintf (mf,"\n\tMOVX  @DPTR,A");

  // konecne vycisteni registru
  fprintf (mf,"\n_L%05d:",LblNum);
  fprintf (mf,"\n\tMOV   084h,#00h"); // mov  DPP,#0
  CSize+=8;

  uber();
}

void I8051Processor::c_memread() {
  int lb;

  fprintf (mf,"\n\tMOV   DPTR,#0121H");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   084h,A");  // mov  DPP,PAGE
  // test na hodnotu stranky < 8
  fprintf (mf,"\n\tANL   A,#0F8H");
  fprintf (mf,"\n\tJZ   _L%05d",++LblNum);
  lb = LblNum;
  fprintf (mf,"\n\tMOV   A,#00h");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  fprintf (mf,"\n\tSJMP  _L%05d",++LblNum);
  CSize+=16;

  fprintf (mf,"\n_L%05d:",lb);
  fprintf (mf,"\n\tMOV   DPH," R1H);
  fprintf (mf,"\n\tMOV   DPL," R1L);
  CSize+=4;

  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R1H ",A");
  fprintf (mf,"\n\tINC   DPTR");
  fprintf (mf,"\n\tMOVX  A,@DPTR");
  fprintf (mf,"\n\tMOV   " R1L ",A");
  fprintf (mf,"\n_L%05d:",LblNum);
  fprintf (mf,"\n\tMOV   084h,#00h"); // mov  DPP,#0
  CSize+=8;
}

