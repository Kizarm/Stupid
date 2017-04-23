/*###########################################################################*/
/*    FUNKCE CHYBOVYCH HLASENI                                               */
/*###########################################################################*/

//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "inc/romram.h"  /* kvuli uvolneni pameti         */
#include "inc/chyby.h"   /* kvuli konsistenci s prototypy */
#include "inc/Znak.h"
#include "inc/procesor.h"
#include "inc/error.h"

extern FILE * of;
extern bool config_error;// = false;
extern char LastOpenedFile[80];

/*---------------------------------------------------------------------------*/
/*    Fatalni chyba                                                          */
/*---------------------------------------------------------------------------*/
void ErrFatal (const char * s) { /* tisk 1 radky hlaseni, ukonci program */

  strcpy (LastOpenedFile, FSez[ZnakFn].fname);
  fprintf (ef,    "Vazna chyba\n");
  fprintf (ef,    "%s\n",s);
  BaseProcessorPtr->c_procerror();

  SezZapomenVse();

  fclose (ef);
  fclose (of);

  throw CError (s);
}

/*---------------------------------------------------------------------------*/
/*   Chyba otevreni souboru                                                  */
/*---------------------------------------------------------------------------*/
void ErrFile (const char * s) {     /* tisk 1 radky hlaseni, ukonci program */
  fprintf (ef    , "Nelze otevrit soubor '%s'\n",s);

  SezZapomenVse();

  fclose (ef);
  fclose (of);

  throw CFileError (s);
}

/*---------------------------------------------------------------------------*/
/*   Syntakticka chyba                                                       */
/*---------------------------------------------------------------------------*/
void ErrSynt (const char * s) {
  strcpy (LastOpenedFile, FSez[ZnakFn].fname);
  fprintf (ef    ,"Chyba na radku %d: %s",ZnakLine(),s);
  BaseProcessorPtr->c_procerror();

  SezZapomenVse();

  fclose (ef);
  fclose (of);

  throw CSyntaxError (ZnakLine(), s);
}

/*---------------------------------------------------------------------------*/
/*   Syntakticka chyba - chybi klicove slovo                                 */
/*---------------------------------------------------------------------------*/
void ErrSyntMiss (const char * s) {
  strcpy (LastOpenedFile, FSez[ZnakFn].fname);
  fprintf (ef    ,"Chyba na radku %d: Ocekavam '%s'",ZnakLine(),s);
  BaseProcessorPtr->c_procerror();

  SezZapomenVse();

  fclose (ef);
  fclose (of);

  throw CSyntMissErr (ZnakLine(), s);
}
/*---------------------------------------------------------------------------*/
/*   Warning                                                                 */
/*---------------------------------------------------------------------------*/
void WarSynt (char * s) {
  strcpy (LastOpenedFile, FSez[ZnakFn].fname);
  AddInfo ("\nVarovani: %s\n",s);
  fprintf (ef    ,"Varovani: %s\n",s);

  //fclose (ef);

  ZnakKde();
}

void Warning (char * s) {
  strcpy (LastOpenedFile, FSez[ZnakFn].fname);
  AddInfo ("\nVarovani: %s\n",s);
  fprintf (ef, "Varovani: %s\n",s);
}

void AddInfo (const char * pszFormat, ...) {
  char    buf[1024], buff[1024];
  int     i, j;

  // Sestaveni textu
  va_list arglist;
  va_start (arglist, pszFormat);
  vsprintf (buf, pszFormat, arglist);
  va_end (arglist);

  for (i = 0, j = 0; buf[i] != 0; i++) {
    if (buf[i] != '\r') {
      if (buf[i] == '\n') {
        buff[j++] = '\r';
        buff[j] = '\n';
      } else {
        buff[j] = buf[i];
      }
      ++j;
    }
  }

  buff[j] = 0;
  // volani fce
  if (LogFce != NULL)
    (*LogFce) (buff);
}
