/*###########################################################################*/
/*                                                                           */
/*   Lexikon ZNAK - soubor funkci pro zprostredkovani znaku                  */
/*                                                                           */
/* 21.08.97 - pridan 'toupper'                                               */
/* 25.08.97 - priprava na MACRO. Prepinani prekladu mezi retezcem a file     */
/*            s automatickym navratem na patricne misto prekladu po          */
/*            vycerpani retezce.                                             */
/*                                                                           */
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <assert.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "inc/List.h"
#include "inc/lexem.h"
#include "inc/romram.h"
#include "inc/zasob.h"
#include "inc/Znak.h"
#include "inc/chyby.h"

/*---------------------------------------------------------------------------*/

#define CR 0x0a  /* line feed */
#define PULBUF  1024

#define BUFVEL PULBUF*2 /* celkova sirka bufferu               */
#define BUFDEL BUFVEL+2 /* pro registraci konce souboru        */


char     znak_posl ;
long int znak_uk   ;
int      ZnakFn    ;          /* cislo aktualni file                    */

static FILE   *  fp;
static int       TeckaFlg ;

static char    ReOpen = ~0; // indikuje ze soubor byl znovu otevren

extern char aktualnisoubor[RET_DEL];

f_sez FSez[POLOZEK];

/*---------------------------------------------------------------------------*/

static char FileCl() {
  if (ZnakFn <= 0) return EOF;
  if (fp != NULL) {
    fclose (fp);
    if (FSez[ZnakFn].buff != NULL) {
      free (FSez[ZnakFn].buff);
      FSez[ZnakFn].buff = NULL;
    }
    ZnakFn--;
    AddInfo ("\n%s ",FSez[ZnakFn].fname);
    (*FSez[ZnakFn].re) ();
  } else {
    ZnakFn--;
  }

  ReOpen = ~0;

  return ' ';
}

static void FileRe() {
  fp= freopen (FSez[ZnakFn].fname,"rb",fp);
  if (fp==NULL) ErrFile (FSez[ZnakFn].fname);
  znak_uk = FSez[ZnakFn].zuk ;
  strcpy (aktualnisoubor, FSez[ZnakFn].fname);
}

static char FileRd (long luk) {
  long dif/*,offset*/;
  int  numread;

  dif=luk-FSez[ZnakFn].off;
  if (dif<0L || dif>=BUFVEL) {
    ReOpen = ~0;
    FSez[ZnakFn].off= (0L< (luk-PULBUF)) ? luk-PULBUF : 0L ;
    fseek (fp,FSez[ZnakFn].off,0);
    dif=luk-FSez[ZnakFn].off;
    numread=fread (FSez[ZnakFn].buff,sizeof (char),BUFDEL,fp);
    FSez[ZnakFn].len=FSez[ZnakFn].off+numread ;
    if (TeckaFlg) {
      AddInfo (".");
    }
  }
  return * (FSez[ZnakFn].buff+dif) ;
}

void FileInit (char * file) {
  int numread;

  FSez[ZnakFn].zuk   = znak_uk;     /* to je pointer ktery opoustim */
  ZnakFn++;
  if (ZnakFn>=POLOZEK)  ErrFatal ("Too many includes");

  strcpy (aktualnisoubor, file);
  strcpy (FSez[ZnakFn].fname,file);    /* jmeno noveho souboru         */
  FSez[ZnakFn].re    = FileRe ;        /* ukazatele na funkce          */
  FSez[ZnakFn].cl    = FileCl ;        /* ukazatele na funkce          */
  FSez[ZnakFn].rd    = FileRd ;        /* ukazatele na funkce          */
  FSez[ZnakFn].buff  = (char *) malloc (BUFDEL); /* na zacatku souboru           */
  FSez[ZnakFn].zuk   = 0L  ;           /* na zacatku souboru           */
  FSez[ZnakFn].off   = 0L  ;           /* na zacatku souboru           */
  FSez[ZnakFn].len   = 0L  ;           /* na zacatku souboru           */

  if (FSez[ZnakFn].buff == NULL) ErrFatal ("No room for character buffer");

  if (fp) {
    fp= freopen (file,"rb",fp);
  } else  {
    fp= fopen (file,"rb");
  }

  if (fp==NULL) ErrFile (file);

  numread=fread (FSez[ZnakFn].buff,sizeof (char),BUFDEL,fp);
  FSez[ZnakFn].len= (numread<BUFDEL) ? numread : BUFDEL ;

  znak_uk=0L;

  ReOpen = ~0;

  AddInfo ("\n%s ",file);
}

/*---------------------------------------------------------------------------*/

void StringEnd() {
  free (FSez[ZnakFn].buff);
  ZnakFn--;
  (*FSez[ZnakFn].re) ();
}

static char StringCl() {
  FSez[ZnakFn].zuk   = znak_uk;
  return EOF;
}

static void StringRe() {
  znak_uk = FSez[ZnakFn].zuk ;
}

static char StringRd (long luk) {
  return * (FSez[ZnakFn].buff+luk) ;
}

void StringInit (const char * a) {
//     int numread;

  FSez[ZnakFn].zuk   = znak_uk;     /* to je pointer ktery opoustim */
  ZnakFn++;
  if (ZnakFn>=POLOZEK)  ErrFatal ("Too many includes");

  strcpy (FSez[ZnakFn].fname," ");          /* jmeno noveho souboru         */
  FSez[ZnakFn].re    = StringRe ;           /* ukazatele na funkce          */
  FSez[ZnakFn].cl    = StringCl ;           /* ukazatele na funkce          */
  FSez[ZnakFn].rd    = StringRd ;           /* ukazatele na funkce          */
  FSez[ZnakFn].buff  = (char *) malloc (strlen (a) +1); /* prostor pro retezec          */
  FSez[ZnakFn].zuk   = 0L  ;                /* na zacatku souboru           */
  FSez[ZnakFn].off   = 0L  ;                /* na zacatku souboru           */
  FSez[ZnakFn].len   = strlen (a) ;         /* na zacatku souboru           */

  if (FSez[ZnakFn].buff == NULL) ErrFatal ("No room for character buffer");

  strcpy (FSez[ZnakFn].buff,a);

  znak_uk=0L;
}

/*---------------------------------------------------------------------------*/

static char MacroCl() {
// fprintf(stdout,"MacroCl()\n");
  if (ZnakFn==1) return EOF;
  free (FSez[ZnakFn].buff);
  FSez[ZnakFn].buff = NULL;
  ZnakFn--;
  (*FSez[ZnakFn].re) ();
  return ' ';
}

static void MacroRe() {
// fprintf(stdout,"MacroRe()\n");
  znak_uk = FSez[ZnakFn].zuk ;
}
static char MacroRd (long luk) {
  long len = strlen (FSez[ZnakFn].fname);
// fprintf(stdout,"MacroRd()\n");
  if (FSez[ZnakFn].buff == NULL) {
    if (len != FSez[ZnakFn].len)
      ErrFatal ("Internal compiler error - 'MacroRd'");
    // musim naalokovat novy prostor pro retezec
    FSez[ZnakFn].buff  = (char *) malloc (len+1);
    strcpy (FSez[ZnakFn].buff, FSez[ZnakFn].fname);
  }

  return * (FSez[ZnakFn].buff+luk) ;
}

void MacroInit (char * a) {
//   fprintf(stdout,"MacroInit(%s)\n",a);

//     int numread;

  FSez[ZnakFn].zuk   = znak_uk;     /* to je pointer ktery opoustim */
  ZnakFn++;
  if (ZnakFn>=POLOZEK)  ErrFatal ("Too many includes");

  strcpy (FSez[ZnakFn].fname, a);          /* jmeno noveho souboru         */
  FSez[ZnakFn].re    = MacroRe ;            /* ukazatele na funkce          */
  FSez[ZnakFn].cl    = MacroCl ;            /* ukazatele na funkce          */
  FSez[ZnakFn].rd    = MacroRd ;            /* ukazatele na funkce          */
  FSez[ZnakFn].buff  = (char *) malloc (strlen (a) +1); /* prostor pro retezec          */
  FSez[ZnakFn].zuk   = 0L  ;                /* na zacatku souboru           */
  FSez[ZnakFn].off   = 0L  ;                /* na zacatku souboru           */
  FSez[ZnakFn].len   = strlen (a) ;         /* na zacatku souboru           */

  if (FSez[ZnakFn].buff == NULL) ErrFatal ("No room for character buffer");

  strcpy (FSez[ZnakFn].buff,a);

  znak_uk=0L;
}

/*---------------------------------------------------------------------------*/
/*   znak_in(luk)    cteni znaku s cislem luk v souboru                      */
/*---------------------------------------------------------------------------*/
static char znak_in (long luk) {
//   long dif,offset;
//   int  numread;
  char c;

  if (luk>=FSez[ZnakFn].len) {
    if (luk==FSez[ZnakFn].len) {
      return ' ';
    }
    c= (*FSez[ZnakFn].cl) ();
    return c;
  }
  return (*FSez[ZnakFn].rd) (luk);
}

static char NulCl()      {
  return EOF;
}
static char NulRd (long l) {
  return EOF;
}
static void NulRe()      {  }

void ZnakInit() {
  TeckaFlg=1;
  ZnakFn=0;
  fp=NULL;
  FSez[0].cl=NulCl;
  FSez[0].re=NulRe;
  FSez[0].rd=NulRd;
}

/*-------------------------------------------------------------*/
/*   ZnakEnd()      uzavreni souboru                           */
/*-------------------------------------------------------------*/
void ZnakEnd() {  }

/*-------------------------------------------------------------*/
/*   ZnakCti()   cteni znaku podle ukazovatka na zasobniku     */
/*-------------------------------------------------------------*/
char ZnakCti() {
  znak_posl = toupper (znak_in (znak_uk++)) ;
  return znak_posl;
}

char ZnakCtiMV() {
  znak_posl = znak_in (znak_uk++) ;
  return znak_posl;
}

/*-------------------------------------------------------------*/
/*   znak_kde()  vypise radek, na kterem se vyskytuje znak,    */
/*          na ktery ukazuje aktualni pol_zn                   */
/*-------------------------------------------------------------*/
void ZnakKde() {
  int  i;
  long /*j,*/uk,li,lj;
  char zn;

  TeckaFlg=0;

  uk = znak_uk-1 ;

  zn = znak_in (uk);
  /*
       while(   (uk > 0) &&
             (
                (zn == ' '  ) || (zn == 0x0d) || (zn == 0x0a ) || (zn == 0x09)
             )
            ){ uk--; zn=znak_in(uk); }

       uk++;
  */

  lj = uk ;

  while ( (uk>=0) && (zn= znak_in (uk)) != '\n'  &&  zn != EOF) uk-- ;

  i=1;
  for (li = 0L ; li <= lj ; li++) {
    if (znak_in (li) == '\n') i++;
  }
  /*     fprintf(stderr, "\nline %d\n",i);

       i=0; li=uk;
       for ( uk++ ; (zn=znak_in(uk)) != '\n'  && zn != EOF ; uk++ )
            { putc(zn,stderr); if(uk<lj) i++; }

       fprintf(stderr,"\n");
       for(;i>0;i--){ putc('.',stderr); } putc('^',stderr);

       fprintf(stderr,"\n");
       TeckaFlg=1;
  */
  AddInfo ("\nline %d\n",i);

  i=0;
  li=uk;
  char p[1000];
  int  j = 0;
  for (uk++ ; (zn=znak_in (uk)) != '\n'  && zn != EOF ; uk++) {
    p[j++] = zn;
    if (uk<lj) i++;
  }
  p[i] = 0;
  AddInfo (p);

  AddInfo ("\n");
  for (; i>0; i--) {
    AddInfo (".");
  }
  AddInfo ("^");

  AddInfo ("\n");
  TeckaFlg=1;

//   fflush (stderr);
//   fflush (stdout);
}

/*-------------------------------------------------------------*/
/*   ZnakLine()  vrati cislo radku, na kterem se nachazime     */
/*-------------------------------------------------------------*/

int  ZnakLine() {
  long /*j,*/uk,li,lj;
  char zn;
  static int  i = 1;
  static long  start = 0L;

  TeckaFlg=0;

  uk = znak_uk-1 ;
  zn = znak_in (uk);
  lj = uk ;

  while ( (uk >= 0) && (zn = znak_in (uk)) != '\n'  &&  zn != EOF) uk-- ;


  // pokud byl buffer znovu vytvoren
  if (ReOpen) {
    // musim projit celi text
    start = 0L;
    i = 1;
  }
  // a spocitam pocet radku
  for (li = start; li <= lj; li++) {
    if (znak_in (li) == '\n') i++;
  }
  start = li;
  ReOpen = 0;

  TeckaFlg=1;

//   printf("%d\n", i);

  return i ;
}
