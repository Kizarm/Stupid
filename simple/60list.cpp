/*###########################################################################*/
/*    FUNKCE PRACE SE SEZNAMEM PRO NOVOU KOSTRU PRIMASU (snad)               */
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/chyby.h"
#include "inc/Var.h"


/*---------------------------------------------------------------------------*/
/*    Exportovane moduly                                                     */
/*---------------------------------------------------------------------------*/

POLOZKA * sez_uk = NULL; // ukazatel na naposled ulozenou, nebo nalezenou polozku
POLOZKA * SezDummy = NULL; // ukazatel na naposled ulozenou, nebo nalezenou polozku
int     Hloubka ; // promenna obsahujici aktualni hloubku zanoreni modulu


/*---------------------------------------------------------------------------*/
/*    Vnitrni staticke promenne                                              */
/*---------------------------------------------------------------------------*/

POLOZKA * Last = NULL  ; /* Ukazatel na posledni polozku v seznamu         */

/*---------------------------------------------------------------------------*/

void  SezNalezen (char * Identifikator) {
  sez_uk=Last;
  while (sez_uk != NULL) {
    if (!strcmp (Identifikator, (sez_uk->iden))) {
      Pravda();
      return;
    }
    sez_uk=sez_uk->prev;
  }
  Nepravda();
}

void SezTypu (int t) {
  if (sez_uk->typ==t) Pravda();
  else               Nepravda();
}

void SezPodtypu (int t) {
  if (sez_uk->podtyp==t) Pravda();
  else               Nepravda();
}


void SezUloz (LPOLOZKA * p) {
  char  *  s,/**t,*/*d;

  sez_uk = (POLOZKA *) malloc (sizeof (POLOZKA));
  if (sez_uk == NULL) ErrFatal ("List - nedostatek pameti pro Polozku");

  s = (char *) malloc (strlen (p->iden) +1);
  if (s == NULL) ErrFatal ("List - nedostatek pameti pro identifikator");

  d = (char *) malloc (strlen (p->dtyp) +1);
  if (d == NULL) ErrFatal ("List - nedostatek pameti pro dtyp");

  sez_uk->prev=Last;
  Last=sez_uk;

  sez_uk->iden    =s         ;
  strcpy (s ,       p->iden) ;
  sez_uk->dtyp    =d         ;
  strcpy (d ,       p->dtyp) ;

  sez_uk->typ     =p->typ    ;
  sez_uk->podtyp  =p->podtyp ;
  sez_uk->size    =p->size   ;
  sez_uk->hloubka =   Hloubka;
  sez_uk->trida   =p->trida  ;
  sez_uk->atr     =p->atr    ;
  sez_uk->fl      =p->fl     ;
  sez_uk->rvs     =p->rvs    ;
  sez_uk->fpl     =p->fpl    ;
  sez_uk->hz      =p->hz     ;
  sez_uk->hst     =p->hst    ;
}
void SezSUloz (SPOLOZKA * p) {
  char  *  s/*,*t*/,*d;

  sez_uk= (POLOZKA *) malloc (sizeof (POLOZKA));
  if (sez_uk==NULL) ErrFatal ("List - nedostatek pameti pro Polozku");

  s= (char *) malloc (strlen (p->iden) +1);
  if (s==NULL) ErrFatal ("List - nedostatek pameti pro identifikator");

  d= (char *) malloc (strlen (p->dtyp) +1);
  if (d==NULL) ErrFatal ("List - nedostatek pameti pro dtyp");

  sez_uk->prev=Last;
  Last=sez_uk;

  sez_uk->iden    =s         ;
  strcpy (s ,       p->iden) ;
  sez_uk->dtyp    =d         ;
  strcpy (d ,       p->dtyp) ;

  sez_uk->typ     =p->typ    ;
  sez_uk->podtyp  =p->podtyp ;
  sez_uk->size    =p->size   ;
  sez_uk->hloubka =   Hloubka;
  sez_uk->trida   =p->trida  ;
  sez_uk->atr     =p->atr    ;
  sez_uk->fl      =p->fl     ;
  sez_uk->rvs     =p->rvs    ;
  sez_uk->fpl     =p->fpl    ;
  sez_uk->hz      =p->hz     ;
  sez_uk->hst     =p->hst    ;
}

POLOZKA * SezUlozId (LPOLOZKA * p) {
  char  *  s/*,*t,*d*/;

  sez_uk= (POLOZKA *) malloc (sizeof (POLOZKA));
  if (sez_uk==NULL) ErrFatal ("List - nedostatek pameti pro Polozku");

  s= (char *) malloc (strlen (p->iden) +1);
  if (s==NULL) ErrFatal ("List - nedostatek pameti pro identifikator");

  sez_uk->prev=Last;
  Last=sez_uk;

  sez_uk->iden    =s         ;
  strcpy (s ,       p->iden) ;

  sez_uk->dtyp    =NULL      ;

  sez_uk->typ     =p->typ    ;
  sez_uk->podtyp  =p->podtyp ;
  sez_uk->size    =p->size   ;
  sez_uk->hloubka =   Hloubka;
  sez_uk->trida   =p->trida  ;
  sez_uk->atr     =p->atr    ;
  sez_uk->fl      =p->fl     ;
  sez_uk->rvs     =p->rvs    ;
  sez_uk->fpl     =p->fpl    ;
  sez_uk->hz      =p->hz     ;
  sez_uk->hst     =p->hst    ;

  return sez_uk;
}

void SezDopln (POLOZKA * n,LPOLOZKA * p) {
  char    /**s,*t,*/*d;

  d= (char *) malloc (strlen (p->dtyp) +1);
  if (d==NULL) ErrFatal ("List - nedostatek pameti pro dtyp");

  n->dtyp    =d         ;
  strcpy (d ,  p->dtyp) ;

  n->typ     =p->typ    ;
  n->podtyp  =p->podtyp ;
  n->size    =p->size   ;
  n->trida   =p->trida  ;
  n->atr     =p->atr    ;
  n->fl      =p->fl     ;
  n->rvs     =p->rvs    ;
  n->fpl     =p->fpl    ;
  n->hz      =p->hz     ;
  n->hst     =p->hst    ;

}

static void odeber() {
  POLOZKA * u;
  u=Last;
  if (u==NULL) return;
  Last=Last->prev;
  free (u->iden);
  free (u->dtyp);
  free (u);
}

void SezZapomenHladinu (int HloubkaHladiny) {
  if (Last!=NULL) {
    while ( (Last != NULL) && (Last->hloubka >= HloubkaHladiny))
      odeber();
  }
}

void SezZapomenVse() {
  SezZapomenHladinu (0);
}

void SezInit() {
  LPOLOZKA p= {"aaaaaaaaa",I_var,T_ord,"bbbbbbbbbb",L_int,0,C_undef,N_int ,0.};
  Last=NULL;
  Hloubka=0;
  SezUloz (&p);
  SezDummy = sez_uk;
  Last=NULL;
  Hloubka=0;
}
