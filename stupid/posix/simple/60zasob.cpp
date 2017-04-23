/*###########################################################################*/
/*                                                                           */
/*   Lexikon ZASOBNIK                                                        */
/*                                                                           */
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <string.h>
#include "inc/List.h"
#include "inc/romram.h"
#include "inc/zasob.h"
#include "inc/Znak.h"
#include "inc/chyby.h"

#define ZASPOL 400  /* maximalni pocet polozek na zasobniku   */

char * str_uk;
char SemString[SSL];

RAMADR rom_p;

/*---------------------------------------------------------------------------*/

static struct dvojity_zas {
  int           st_p ;          /* ukazovatko zasobniku            */
  char     *    str[ ZASPOL ];  /* prostor pro ukazatele do SemStr */
  ROMADR        adr[ ZASPOL ];  /* prostor pro prekladove adresy   */
  unsigned long zuk[ ZASPOL ];  /* prostor pro ukazovatka do textu */
  int           fnm[ ZASPOL ];  /* prostor pro cislo file          */
} stav ;

#define StavPush  stav.adr[++stav.st_p] = rom_p;    \
  stav.str[stav.st_p]   = str_uk  ; \
  stav.zuk[stav.st_p]   = znak_uk ; \
  stav.fnm[stav.st_p]   = ZnakFn  ;
#define StavPop   rom_p     = stav.adr[ stav.st_p ] ; \
  str_uk    = stav.str[ stav.st_p ] ; \
  ZnakFn    = stav.fnm[ stav.st_p ] ; \
  znak_uk   = stav.zuk[ stav.st_p-- ];

#define StavLoad  rom_p     = stav.adr[ stav.st_p ] ; \
  str_uk    = stav.str[ stav.st_p ] ; \
  ZnakFn    = stav.fnm[ stav.st_p ] ; \
  znak_uk   = stav.zuk[ stav.st_p ] ;
#define StavSave  stav.adr[stav.st_p]   = rom_p;    \
  stav.str[stav.st_p]   = str_uk  ; \
  stav.fnm[stav.st_p]   = ZnakFn  ; \
  stav.zuk[stav.st_p]   = znak_uk ;

/*---------------------------------------------------------------------------*/

static struct zasobnik {
  int     st_p ;         /* ukazovatko zasobniku            */
  int data [ ZASPOL ] ;  /* prostor pro polozky             */
} SPzas;

static struct l_zasobnik {
  int     st_p ;           /* ukazovatko zasobniku            */
  ATRIBUT data [ ZASPOL ]; /* prostor pro polozky             */
} atributy ;

int uspech;

void ZasInit() {
  atributy.st_p  = 0;
  SPzas.st_p = 0;
  stav.st_p = 0;
  str_uk = SemString;
  strcpy (str_uk,"");
}


/*===========================================================================*/
/*   operace na semantickem zasobniku atributu                               */
/*           ulozeni hodnoty                                                 */
/*           vyjmuti  hodnoty                                                */
/*           prevzeti hodnoty na N-te posici v zasobniku                     */
/*===========================================================================*/

void SemTest() {
  char s[80];
  if (atributy.st_p < 0 || atributy.st_p > ZASPOL) {
    sprintf (s,"zasobnik atributu = %d",atributy.st_p);
    ErrFatal (s);
  }
}

void    SemPush (ATRIBUT at) {
  atributy.data[ ++atributy.st_p ] = at;
  SemTest();
}
ATRIBUT SemPop()              {
  return atributy.data[ atributy.st_p-- ];
}
ATRIBUT SemPick (int n)        {
  return atributy.data[ atributy.st_p-n ];
}
ATRIBUT * SemPtr (int n)        {
  return &atributy.data[ atributy.st_p-n ];
}
void    SemDrop (int n)        {
  atributy.st_p-=n;
  SemTest();
}
int     SemPocetAtr()         {
  return atributy.st_p-SPzas.data[SPzas.st_p];
}

void    StrAdd (const char * s) {
  if ( ( (str_uk-SemString) +strlen (s)) >= (SSL-2)) {
    ErrFatal ("StrAdd pretekl");
  }
  strcat (str_uk,s);
}

void    StrPush (const char * s) {
  SemPush ( (ATRIBUT) str_uk);
  str_uk += strlen (str_uk) +1;
  if ( ( (str_uk-SemString) +strlen (s)) >= (SSL-2)) {
    ErrFatal ("StrPush pretekl");
  }
  strcpy (str_uk,s);
}

void    StrPop (char * s) {
  strcpy (s,str_uk);
  str_uk = (char *) SemPop();
}

void    StrDrop() {
  str_uk = (char *) SemPop();
}

void    StrApp () {
  * (str_uk-1) = ' ';
  str_uk = (char *) SemPop();
}


/*typedef*/ struct int2 {
  unsigned short lb;
  unsigned short hb;
};

static  union flr {
  struct       int2 i2;
  float             f ;
} ra ;

void  RSemPush (float f) {
  ra.f=f;
  SemPush (ra.i2.hb);
  SemPush (ra.i2.lb);
}
float RSemPop() {
  ra.i2.lb=SemPop();
  ra.i2.hb=SemPop();
  return ra.f;
}


/*===========================================================================*/
/*      ulozeni priznaku pravdy              pravda()                        */
/*      ulozeni priznaku nepravdy            nepravda()                      */
/*      negace priznaku na vrcholu           opacne()                        */
/*===========================================================================*/

void Pravda()   {
  uspech = ~0;
}
void Nepravda() {
  uspech =  0;
}
void Opacne()   {
  uspech ^= ~0 ;
}

/*===========================================================================*/
/*   funkce pro definici syntaxe                                             */
/*   uvozeni definice                             je()                       */
/*   zretezeni                                    zrtz()                     */
/*   nebo                                         neb()                      */
/*   ukonceni                                     vse()                      */
/*   uvozeni sledu                                _sled()                    */
/*   ukonceni sledu                               sled_()                    */
/*   struktura pak konec                          pkk()                      */
/*   typ rozeznavajici prazdny retezec            nic()                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/*   je(); uvozeni definice <je vse> provede zapamatovani                    */
/*         vychozi pozice v textu a zalozeni noveho sem. zas.                */
/*---------------------------------------------------------------------------*/
void je() {
  StavPush
  SPzas.data[++SPzas.st_p]=atributy.st_p;
}

/*---------------------------------------------------------------------------*/
/*   zrtz()  funkce vraci logickou hodnotu pro if :                          */
/*           pravda, kdyz predchozi syntakticky element                      */
/*                   vratil  tf.                                             */
/*           nepravda, kdyz je  ff                                           */
/*---------------------------------------------------------------------------*/

int zrtz() {
  return uspech;
}

/*---------------------------------------------------------------------------*/
/*   neb()   funkce vraci logickou hodnotu pro if :                          */
/*           pravda, kdyz predchozi syntakticky element vratil FALSE. Zaroven*/
/*                   vymaze vytvoreny zasobnik a provede navrat na zacatek   */
/*                   textu, kde zacala definice .                            */
/*           nepravda, kdyz je na TOSu TRUE                                  */
/*---------------------------------------------------------------------------*/
int neb() {
  if (uspech) return 0 ;
  StavLoad
  atributy.st_p = SPzas.data[ SPzas.st_p ] ;            /* vymaz atributu */
  return ~0 ;
}

/*---------------------------------------------------------------------------*/
/*   vse()   funkce ukoncuje definici syntaktickeho elem.                    */
/*         * Kdyz syntakticky element vratil tf,                             */
/*           posune ukazovatko textu a polozky, vytvorene na                 */
/*           semantickem zasobniku prida ke starym polozkam.                 */
/*         * Kdyz obdrzi ff, vrati ukazovatko textu                          */
/*           na zacatek textu a zrusi svuj atributy zasob.                   */
/*---------------------------------------------------------------------------*/
void vse() {
  if (uspech) {
    stav.st_p-- ; /* drop zapamatovanych hodnot ukazatelu textu a rom_p */
    SPzas.st_p--; /* zahozeni delitka mezi zasobniky = pridani atributu */
  } else {        /* obnoveni zapamatovanych hodnot                     */
    StavPop
    atributy.st_p=SPzas.data[ SPzas.st_p-- ];
  }
}

/*---------------------------------------------------------------------------*/
/*   _sled() uvozeni definice syntaktickeho elementu                         */
/*           <sled   ...   sled>                                             */
/*           viz je()                                                        */
/*---------------------------------------------------------------------------*/
void _sled() {
  StavPush
  SPzas.data[++SPzas.st_p]=atributy.st_p;
}

/*---------------------------------------------------------------------------*/
/*   sled_() vraci logickou hodnotu pro while(  )                            */
/*           pravda, kdyz element je rozeznan. Provede tez                   */
/*                 posun ukazovatka v textu a predani polo-                  */
/*                 zek semantickeho zasobniku. Dale vytvori                  */
/*                 novy semanticky zasobnik pro opakovani                    */
/*                 pokusu o rozeznani.                                       */
/*           nepravda, kdyz element neni rozeznan. Vrati                     */
/*                 ukazovatko textu na zacatek posledniho                    */
/*                 pokusu, zrusi naposled zalozeny semantic.                 */
/*                 zasobnik a neguje uspech                                  */
/*---------------------------------------------------------------------------*/
int sled_() {
//   int uk;
  if (uspech) {
    StavSave
    SPzas.data[ SPzas.st_p ] = atributy.st_p;
    return ~0;
  } else {
    StavPop
    atributy.st_p=SPzas.data[ SPzas.st_p-- ];
    pravda(); /* sled je vzdy pravdivy */
    return 0 ;
  }
}

/*---------------------------------------------------------------------------*/
/*   pkk() vrati hodnotu na vrcholu priznakoveho zasobniku                   */
/*---------------------------------------------------------------------------*/

int  pkk() {
  return uspech ;
}

