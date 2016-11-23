/*###########################################################################*/
/*                                                                           */
/*    Preddefinovane objekty                                                 */
/*                                                                           */
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/romram.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/fce.h"
#include "inc/chyby.h"

/*---------------------------------------------------------------------------*/

#define  ILEN (32)
#define  OLEN (32)
#define  DLEN (64)
#define  WLEN (128)

#define  XLEN (32)
#define  YLEN (32)
#define  MLEN (128)
#define  BLEN (128)

#define NILEN ( 64-ILEN)
#define NOLEN ( 64-OLEN)
#define NWLEN (256-WLEN)
#define NXLEN (256-XLEN)
#define NYLEN (256-YLEN)

/*---------------------------------------------------------------------------*/

#define PREDEFORG ( 8*0 )

#define  IORG (  PREDEFORG     )
#define  IEND (  IORG + 16*ILEN )
#define  OORG (  IEND )
#define  OEND (  OORG + 16*OLEN )
#define  DORG (  OEND )
#define  DEND (  DORG + 16*DLEN )
#define  WORG (  DEND )
#define  WEND (  WORG + 16*WLEN )

#define  XORG (  WEND )
#define  XEND (  XORG +    XLEN )
#define  YORG (  XEND )
#define  YEND (  YORG +    YLEN )
#define  MORG (  YEND )
#define  MEND (  MORG +    MLEN )
#define  BORG (  MEND )
#define  BEND (  BORG +    BLEN )

#define NIORG (  BEND )
#define NIEND ( NIORG + 16*NILEN )
#define NOORG ( NIEND )
#define NOEND ( NOORG + 16*NOLEN )
#define NWORG ( NOEND )
#define NWEND ( NWORG + 16*NWLEN )

#define NXORG ( NWEND )
#define NXEND ( NXORG +    NXLEN )
#define NYORG ( NXEND )
#define NYEND ( NYORG +    NYLEN )

#define FREEORG ( NYEND )


/*---------------------------------------------------------------------------*/

#define WRDL  (ILEN+OLEN+DLEN+WLEN)
#define NWRDL (NILEN+NOLEN+NWLEN)
#define BITL  (XLEN+YLEN+MLEN+BLEN)
#define NBITL (NXLEN+NYLEN)

static  struct {
  char  name ;
  char  typ[10];
  int   len  ;
  int   min  ;
  int   max  ;
  int   org  ;
}
Predefs[13]= {
  { 'X' ,  "IN_BIT",  L_bool,    0,       XLEN,  XORG },
  { 'Y' ,     "BIT",  L_bool,    0,       YLEN,  YORG },
  { 'M' ,     "BIT",  L_bool,    0,       MLEN,  MORG },
  { 'B' ,     "BIT",  L_bool,    0,       BLEN,  BORG },
  { 'I' , "IN_WORD",  L_word,    0,       ILEN,  IORG },
  { 'O' ,    "WORD",  L_word,    0,       OLEN,  OORG },
  { 'D' ,    "WORD",  L_word,    0,       DLEN,  DORG },
  { 'W' ,    "WORD",  L_word,    0,       WLEN,  WORG },

  { 'X' ,  "IN_BIT",  L_bool, XLEN, NXLEN+XLEN, NXORG },
  { 'Y' ,     "BIT",  L_bool, YLEN, NYLEN+YLEN, NYORG },
  { 'I' , "IN_WORD",  L_word, ILEN, NILEN+ILEN, NIORG },
  { 'O' ,    "WORD",  L_word, OLEN, NOLEN+OLEN, NOORG },
  { 'W' ,    "WORD",  L_word, WLEN, NWLEN+WLEN, NWORG }
};

static short WFlg[WRDL];
static short BFlg[BITL];
static short NWFlg[NWRDL];
static short NBFlg[NBITL];

/*---------------------------------------------------------------------------*/
void PredefInit (void) {
  int i;

  for (i=0; i<BITL; i++) {
    BFlg[i] = false;
  }

  for (i=0; i<WRDL; i++) {
    WFlg[i] = false;
  }

  for (i=0; i< (NXLEN+NYLEN); i++) {
    NBFlg[i] = false;
  }


  for (i=0; i< (NILEN+NOLEN+NWLEN); i++) {
    NWFlg[i] = false;
  }
}

extern void dec_cifra();

static void PredefVyhledej (char z, int a, char * id) {
  int  i;
  for (i=0; i<13; i++) {
    if ( (Predefs[i].name == z)
         && (Predefs[i].min  <= a)
         && (Predefs[i].max  >  a)
       ) {
      sprintf (id,"%c%d",z,a);
      strcpy (SezDummy->iden , id);
      strcpy (SezDummy->dtyp , Predefs[i].typ);
      SezDummy->size = Predefs[i].len   ;
      SezDummy->trida= C_stat;
      SezDummy->atr  = Predefs[i].len* (a-Predefs[i].min)
                       +Predefs[i].org;
      sez_uk         = SezDummy ;
      return;
    }
  }
  ErrSynt ("Cislo preddefinovane promenne presahlo rozsah");
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Preddefinovane promenne                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/


void PredefVar (char * t) {
  char z;
  int a/*,i*/;
  if ( (ZnakCti()  == 'X')
       || (znak_posl  == 'Y')
       || (znak_posl  == 'M')
       || (znak_posl  == 'B')
       || (znak_posl  == 'I')
       || (znak_posl  == 'O')
       || (znak_posl  == 'D')
       || (znak_posl  == 'W')
     ) {
    z = znak_posl;
  } else {
    nepravda();
    return;
  }

  je();
  {
    SemPush (0);     /* priprava a inicialisace dedicneho atributu       */
    dec_cifra();
    if (zrtz()) {
      _sled();
      do {
        dec_cifra();
      } while (sled_());
      if (zrtz()) {
        mezery();
      }
    }
  }
  vse();

  if (pkk()) {
    a=SemPop();
    PredefVyhledej (z,a,t);
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Preddefinovane - rozpoznani podle adresy a poznamka o pouziti             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void PredefAdr (unsigned long a) {
  int r;

  if (a< WEND)               {
    r=a/16;
    WFlg[r]  = 1;
    return;
  }
  if ( (a<NWEND) && (a>=NIORG)) {
    r= (a-NIORG) /16;
    NWFlg[r] = 1;
    return;
  }
  if (a< BEND)               {
    r=a-XORG;
    BFlg[r]  = 1;
    return;
  }
  if ( (a<NYEND) && (a>=NXORG)) {
    r=a-NXORG;
    NBFlg[r] = 1;
    return;
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Vypis                                                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/


static void VypisId (FILE * ff,POLOZKA * p) {
  unsigned long a;
  int r;

  if (p->typ != I_var)          return;
  a=p->atr;
  if (a>= (FREEORG))          return;

  if ( (a>= IORG) && (a< WEND)) {
    r=a/16;
    if (WFlg[r] && (!strcmp (p->dtyp,"WORD") || !strcmp (p->dtyp,"IN_WORD"))) {
      fprintf (ff,"\n#word %d %s",r,p->iden);
      return;
    }
  }
  if ( (a>=NIORG) && (a<NWEND)) {
    r= (a-NIORG) /16;
    if (NWFlg[r] && (!strcmp (p->dtyp,"WORD") || !strcmp (p->dtyp,"IN_WORD"))) {
      fprintf (ff,"\n#word %d %s",r,p->iden);
      return;
    }
  }

  if ( (a>= XORG) && (a< BEND)) {
    r=a-XORG;
    if (BFlg[r] && (!strcmp (p->dtyp,"BIT") || !strcmp (p->dtyp,"IN_BIT"))) {
      fprintf (ff,"\n#bit %d %s",r,p->iden);
      return;
    }
  }

  if ( (a>=NXORG) && (a<NYEND)) {
    r=a-NXORG;
    if (NBFlg[r] && (!strcmp (p->dtyp,"BIT") || !strcmp (p->dtyp,"IN_BIT"))) {
      fprintf (ff,"\n#bit %d %s",r,p->iden);
      return;
    }
  }
}

/*---------------------------------------------------------------------------*/

void PredefVypis (FILE * ff) {
  int i, ob, ow;
  POLOZKA * u;

  for (i=0; i<XLEN; i++) {
    if (BFlg[i]) {
      fprintf (ff,"\n#bit %d x%d ",i,i);
    }
  }

  for (; i< (XLEN+YLEN); i++) {
    if (BFlg[i]) {
      fprintf (ff,"\n#bit %d y%d ",i,i-XLEN);
    }
  }

  for (; i< (XLEN+YLEN+MLEN); i++) {
    if (BFlg[i]) {
      fprintf (ff,"\n#bit %d m%d ",i,i-XLEN-YLEN);
    }
  }

  for (; i<BITL; i++) {
    if (BFlg[i]) {
      fprintf (ff,"\n#bit %d b%d ",i,i-XLEN-YLEN-MLEN);
    }
  }

  ob=i;


  for (i=0; i<ILEN; i++) {
    if (WFlg[i]) {
      fprintf (ff,"\n#word %d i%d ",i,i);
    }
  }

  for (; i< (ILEN+OLEN); i++) {
    if (WFlg[i]) {
      fprintf (ff,"\n#word %d o%d ",i,i-ILEN);
    }
  }

  for (; i< (ILEN+OLEN+DLEN); i++) {
    if (WFlg[i]) {
      fprintf (ff,"\n#word %d d%d ",i,i-ILEN-OLEN);
    }
  }

  for (; i<WRDL; i++) {
    if (WFlg[i]) {
      fprintf (ff,"\n#word %d w%d ",i,i-ILEN-OLEN-DLEN);
    }
  }

  ow=i;


  /*------------------------ Nove bity -------------------------------------*/

  for (i=0; i<NXLEN; i++) {
    if (NBFlg[i]) {
      fprintf (ff,"\n#bit %d x%d ",ob+i,i+XLEN);
    }
  }

  for (; i< (NXLEN+NYLEN); i++) {
    if (NBFlg[i]) {
      fprintf (ff,"\n#bit %d y%d ",ob+i,i-NXLEN+YLEN);
    }
  }

  /*------------------------ Nove wordy ------------------------------------*/

  for (i=0; i<NILEN; i++) {
    if (NWFlg[i]) {
      fprintf (ff,"\n#word %d i%d ",ow+i,i+ILEN);
    }
  }

  for (; i< (NILEN+NOLEN); i++) {
    if (NWFlg[i]) {
      fprintf (ff,"\n#word %d o%d ",ow+i,i+OLEN-NILEN);
    }
  }

  for (; i< (NILEN+NOLEN+NWLEN); i++) {
    if (NWFlg[i]) {
      fprintf (ff,"\n#word %d d%d ",ow+i,i+WLEN-NILEN-NOLEN);
    }
  }

  u=Last;
  while (u!=NULL) {
    VypisId (ff, u);
    u=u->prev;
  }

}
