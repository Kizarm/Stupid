/*###########################################################################*/
/*                                                                           */
/*    Zakladni lexikalni analyza                                             */
/*                                                                           */
/*###########################################################################*/

//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/romram.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/chyby.h"
#include "inc/fce.h"
#include "inc/procesor.h"

char deklarace = ~0;

char aktualnisoubor[RET_DEL];

#define  APOSTROF (39)
#define  UVOZ     (0x22)
static char str[ID_DEL], *ukstr;

/*---------------------------------------------------------------------------*/
/*   slovo( string ) zjistuje pritomnost retezce ve zdrojovem textu          */
/*        je-li, vraci TRUE a nastavi ukazatel v textu za retezec            */
/*        neni-li, vraci FALSE a ukazatel je na prvnim odlisnem znaku        */
/*---------------------------------------------------------------------------*/
static void slovo (const char * s) {
  while (*s!='\0') {
    if (*s++ != ZnakCti()) {
      Nepravda();
      return;
    }
  }
  Pravda();
}
void znak (char s) {
  if (ZnakCti() == s) {
    pravda();
  }  else  nepravda();
}

/*---------------------------------------------------------------------------*/
/*   mezera() rozpoznava sled ignorovanych znaku a posouva ukazatel          */
/*            ve zdrojovem textu na prvni odlisny znak                       */
/*            vraci vzdy TRUE                                                */
/*---------------------------------------------------------------------------*/
static void mezera() {
  //static int line = 0;
  //char  *  text = NULL;
  ZnakCti();
  if ( (znak_posl == ' ') || (znak_posl == 0x0d)
       || (znak_posl == 0x0a) || (znak_posl == 0x09)
       || (znak_posl == 0x1a)) {
    Pravda();
    return;
  }
  Nepravda();
}

/*---------------------------------------------------------------------------*/
/*   koment()                                                                */
/*---------------------------------------------------------------------------*/

static void StructKoment() {
  je();
  {
    slovo ("{");
    if (zrtz()) {
      _sled();
      do {
        StructKoment();
        if (neb()) {
          je();
          {
            slovo ("}");
            if (neb()) {
              znak (EOF);
              if (pkk()) {
                ErrSynt ("EOF uvnitr komentare");
              }
            }
          }
          vse();
          Opacne();  // MK
          if (pkk()) {
            ZnakCti();
          }
        }
      } while (sled_());
      if (zrtz()) {
        slovo ("}");
        JinakCh ("EOF uvnitr komentare");
      }
    }
  }
  vse();
}

static void LineKoment() {
  je();
  {
    slovo ("//");
    if (pkk()) {
      do {
        ZnakCti();
      } while ( (znak_posl != 0x0a) && (znak_posl != EOF));
    }
  }
  vse();
}

void Mezery() {
  _sled();
  do {
    mezera();
    if (neb()) {
      StructKoment();
      if (neb()) {
        LineKoment();
      }
    }
  } while (sled_());
}

/*---------------------------------------------------------------------------*/
/*   Pismeno() rozpoznava znaky a->z a A->Z a posouva ukazatel               */
/*        je-li, vraci TRUE a nastavi ukazatel v textu za znak               */
/*               Znak pripisuje do retezce id na konec                       */
/*        neni-li, vraci FALSE a ukazatel je tez za znakem                   */
/*---------------------------------------------------------------------------*/
static void pismeno (char ** s) {
  if ( ( (ZnakCti()  >= 'A') && (znak_posl <= 'Z')) ||
       ( (znak_posl  >= 'a') && (znak_posl <= 'z')) ||
       (znak_posl  == '_')
     ) {
    **s = znak_posl ;
    (*s) ++;
    pravda();
  } else  nepravda();
}

/*---------------------------------------------------------------------------*/
/*   Cislice() rozpoznava znaky 0->9 a posouva ukazatel                      */
/*        je-li, vraci TRUE a nastavi ukazatel v textu za znak.              */
/*               Znak pripisuje do retezce id na konec                       */
/*        neni-li, vraci FALSE a ukazatel je tez za znakem                   */
/*---------------------------------------------------------------------------*/
static void cislice (char ** s) {
  if ( (ZnakCti() >= '0') && (znak_posl <= '9')) {
    **s = znak_posl ;
    (*s) ++;
    pravda();
  } else  nepravda();
}

/*---------------------------------------------------------------------------*/
/*   Dec_cifra() rozpoznava cislici a syntetizuje atribut hodnoty cisla      */
/*        jehoz je soucasti                                                  */
/*---------------------------------------------------------------------------*/
void dec_cifra() {
  ukstr=str;
  cislice (&ukstr);
  if (pkk()) {
    SemPush ( (znak_posl-'0') + 10*SemPop());
  }
}

/*---------------------------------------------------------------------------*/
/*   Hex_cifra() rozpoznava cislici a syntetizuje atribut hodnoty cisla      */
/*        jehoz je soucasti                                                  */
/*---------------------------------------------------------------------------*/
static void Hex_cifra() {
  ukstr=str;
  je();
  {
    cislice (&ukstr);
    if (pkk()) {
      SemPush ( (znak_posl-'0') + 16*SemPop());
    }
    if (neb()) {
      if ( (ZnakCti() >= 'A') && (znak_posl <= 'F')) {
        SemPush ( (znak_posl-'A'+10) + 16*SemPop());
        Pravda();
      } else Nepravda();
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Bin_cifra() rozpoznava binarni cislici                                  */
/*---------------------------------------------------------------------------*/
void Bin_cifra() {
  if ( (ZnakCti() == '0') || (znak_posl == '1')) {
    SemPush ( (znak_posl-'0') + 2*SemPop());
    Pravda();
  } else {
    Nepravda();
  }
}

/*---------------------------------------------------------------------------*/
/*   cislo_kladne() rozpoznava sled cifer a syntetizuje atribut hodnoty      */
/*---------------------------------------------------------------------------*/
static void cislo_kladne() {
  char a;

  je();
  {
    KlSlovo ("0X");
    if (zrtz()) {
      SemPush (0);     /* priprava a inicialisace dedicneho atributu       */
      Hex_cifra();
      if (zrtz()) {
        _sled();
        do {
          Hex_cifra();
        } while (sled_());
        if (zrtz()) {
          mezery();
        }
      }
    }
    if (neb()) {
      KlSlovo ("0B");
      if (zrtz()) {
        SemPush (0);     /* priprava a inicialisace dedicneho atributu       */
        Bin_cifra();
        if (zrtz()) {
          _sled();
          do {
            Bin_cifra();
          } while (sled_());
          if (zrtz()) {
            mezery();
          }
        }
      }
      if (neb()) {
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
        if (neb()) {
          znak (APOSTROF);
          if (zrtz()) {
            a=ZnakCtiMV();
            SemPush (a);
            if (zrtz()) {
              znak (APOSTROF);
              if (zrtz()) {
                mezery();
              }
            }
          }
        }
      }
    }
  }
  vse();
}
/*---------------------------------------------------------------------------*/
/*   Rcislo()                                                                */
/*---------------------------------------------------------------------------*/

static float atofloat() {
  return (float) (znak_posl-'0');
}

void RealCislo() {
  int /*a,i,*/pm;
  float fa,fb,fe,fz;

  fb=0.1;
  pm=0;
  fe=0.;
  fz=1.;
  fa=0.;

  ukstr=str;

  je();
  {
    je();
    {
      KlSlovo ("-");
      if (pkk()) {
        fz=-fz;
      }
      if (neb()) {
        KlSlovo ("+");
        if (neb()) {
          znak (' ');              /* je to kvuli Rread() */
          if (neb()) {
            nic();
          }
        }
      }
    }
    vse();
    if (zrtz()) {
      _sled();
      do {
        cislice (&ukstr);
        if (pkk()) {
          fa=10.*fa+atofloat();
        }
      } while (sled_());
      if (zrtz()) {
        je();
        {
          znak ('.');
          if (zrtz()) {
            _sled();
            do {
              cislice (&ukstr);
              if (pkk()) {
                fa+=fb*atofloat();
                fb*=0.1;
              }
            } while (sled_());
          }
        }
        vse();
        if (zrtz()) {
          je();
          {
            znak ('E');
            if (zrtz()) {
              je();
              {
                znak ('-');
                if (pkk()) {
                  pm=~0;
                }
                if (neb()) {
                  znak ('+');
                  if (neb()) {
                    nic();
                  }
                }
              }
              vse();
              if (zrtz()) {
                cislice (&ukstr);
                if (pkk()) {
                  fe=atofloat();
                }
                if (zrtz()) {
                  _sled();
                  do {
                    cislice (&ukstr);
                    if (pkk()) {
                      fe=10.*fe+atofloat();
                    }
                  } while (sled_());
                }
              }
            }
            if (neb()) {
              nic();
            }
          }
          vse();
          if (zrtz()) {
            mezery();
          }
        }
      }
    }
  }
  vse();
  if (pkk()) {
    if (pm) {
      fb = fz * fa * exp (-fe * log (10.));
    } else {
      fb = fz * fa * exp (fe * log (10.));
    }
    RSemPush (fb);
  }
}

/*---------------------------------------------------------------------------*/
/*   znak retezce                                                            */
/*---------------------------------------------------------------------------*/

static void ZnakRetezce (char ** s) {
  if ( (ZnakCtiMV()  != UVOZ) && (znak_posl != EOF)) {
    **s = znak_posl ;
    (*s) ++;
    pravda();
  } else  nepravda();
}

/*---------------------------------------------------------------------------*/
/*   filtr klicovych slov jazyka                                             */
/*---------------------------------------------------------------------------*/
void NeniSlovoJazyka (char * s) {
  static const char * keyw[]= {
    "VAR",
    "CONST",
    "END",
    "PROCEDURE",
    "SUBROUTINE",
    "FUNCTION",
    "SYMBOL",
    "XOR",
    "AND",
    "OR",
    "CASE",
    "ENDCASE",
    "OF",
    "IF",
    "THEN",
    "ELSE",
    "ENDIF",
    "DISPLAY",
    ""
  };
  int i;
  i=0;
  while (* (keyw[i])) {
    if (!strcmp (s,keyw[i])) {
      Nepravda();
      return;
    }
    i++;
  }
  Pravda();
}

/*###########################################################################*/
/*                                                                           */
/*      H l a v n i   e x p o r t o v a n e   f u n k c e                    */
/*                                                                           */
/*###########################################################################*/

void JinakCh (const char * str) {
  if (!uspech) ErrSynt (str);
}
void JinakChybi (const char * str) {
  if (!uspech) ErrSyntMiss (str);
}

void KlSlovo (const char * str) {
  je();
  {
    slovo (str);
    if (zrtz()) {
      mezery();
    }
  }
  vse();

  /*  pak printf("\nKlSlovo:%s",str); konec  */

}

void NKlSlovo (const char * str) {
  KlSlovo (str);
  JinakChybi (str);
}

void Cislo() {
  je();
  {
    KlSlovo ("-");
    if (zrtz()) {
      cislo_kladne();
      if (pkk()) {
        SemPush (-SemPop());
      }
    }
    if (neb()) {
      cislo_kladne();
    }
  }
  vse();
}

void KladneCislo() {
  cislo_kladne();
}

void IdentVSeznamu (char * s) {
  char * t;
  t=s;
  je();
  {
    pismeno (&s);
    if (zrtz()) {
      _sled();
      do {
        pismeno (&s);
        if (neb()) {
          cislice (&s);
        }
      } while (sled_());
      *s='\0' ;
      if (zrtz()) {
        SezNalezen (t);
        if (zrtz()) {
          mezery();
        }
      }
    }
  }
  vse();

}

void NovyId (char * t) {
  char * s;
  s=t;
  je();
  {
    PredefVar (s);
    if (pkk()) {
      ErrSynt ("Preddefinovany identifikator");
    }
    if (neb()) {
      pismeno (&s);
      if (zrtz()) {
        _sled();
        do {
          pismeno (&s);
          if (neb()) {
            cislice (&s);
          }
        } while (sled_());
        *s='\0' ;
        if (zrtz()) {
          NeniSlovoJazyka (t);
          if (zrtz()) {
            SezNalezen (t);
            Opacne();
            JinakCh ("Identifikator jiz existuje");
            if (zrtz()) {
              mezery();
            }
          }
        }
      }
    }
  }
  vse();
}

void Retezec (char * s) {
  int i;
  je();
  {
    znak (UVOZ);
    if (zrtz()) {
      i=0;
      _sled();
      do {
        ZnakRetezce (&s);
        if (++i>=RET_DEL) {
          ErrSynt ("Prilis dlouhy retezec");
        }
      } while (sled_());
      if (zrtz()) {
        NKlSlovo ("\x22");
      }
    }
  }
  vse();
  *s='\0' ;
}

void KonecSouboru() {
  if (ZnakCti() == EOF) {
    pravda();
  }  else  nepravda();
}


extern FILE * mf;
void WriteMyLine2MF (int pos) {
  return;
  if (mf != NULL) {
    fprintf (mf,"\n;;@@ %s,#%d", aktualnisoubor, pos);
  }
}
void WriteLine2MF (char write) {
  return;
  if ( (mf != NULL && !deklarace) || write) {
    fprintf (mf,"\n;;@@ %s,#%d", aktualnisoubor, ZnakLine());
  }
}
