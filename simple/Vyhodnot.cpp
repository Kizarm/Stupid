/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   Rozbor semantickeho stringu provedeni prikazu                           */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/fce.h"
#include "inc/procesor.h"
#include "inc/chyby.h"

/*###########################################################################*/
/*###########################################################################*/
/*---------------------------------------------------------------------------*/
/*   Procedury                                                               */
/*---------------------------------------------------------------------------*/
/*###########################################################################*/
/*###########################################################################*/

extern char * str_uk;
extern char SemString[2000];


static void Konverze() {
  je();
  {
    KlSlovo ("(INT:REAL)");
    if (pkk()) {
      BaseProcessorPtr->c_itor();
    }
    if (neb()) {
      KlSlovo ("(WORD:REAL)");
      if (pkk()) {
        BaseProcessorPtr->c_wtor();
      }
      if (neb()) {
        KlSlovo ("(IN_WORD:REAL)");
        if (pkk()) {
          BaseProcessorPtr->c_wtor();
        }
        if (neb()) {
          KlSlovo ("(REAL:INT)");
          if (pkk()) {
            BaseProcessorPtr->c_rtoi();
          }
          if (neb()) {
            KlSlovo ("(REAL:WORD)");
            if (pkk()) {
              BaseProcessorPtr->c_rtow();
            }
            if (neb()) {
              KlSlovo ("(REAL:IN_WORD)");
              if (pkk()) {
                BaseProcessorPtr->c_rtow();
              }
            }
          }
        }
      }
    }
  }
  vse();
}

static void Alokace() {
  je();
  {
    KlSlovo ("~");
    if (zrtz()) {
      Cislo();
    }
  }
  vse();
  if (pkk()) {
    BaseProcessorPtr->c_allot();
  }
}

static void Interval (unsigned long int * min, unsigned long int * max) {
  je();
  {
    KlSlovo ("<");
    if (zrtz()) {
      Cislo();
      if (pkk()) {
        *min=SemPop();
      }
      if (zrtz()) {
        KlSlovo ("..");
        if (zrtz()) {
          Cislo();
          if (pkk()) {
            *max=SemPop();
          }
          if (zrtz()) {
            KlSlovo (">");
          }
        }
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Immediate operand                                                       */
/*---------------------------------------------------------------------------*/

static char ImmediateOperand() {
  char t[ID_DEL];
  char c = -1;

  je();
  {
    KlSlovo ("#I");
    if (zrtz()) {
      Cislo();
      if (pkk()) {
        c='I';
      }
    }
    if (neb()) {
      KlSlovo ("#R");
      if (zrtz()) {
        Konstanta (t);
        if (pkk()) {
          if (t[0]=='I') {
            RSemPush ( (float) SemPop());
          }
          c='R';
        }
      }
    }
  }
  vse();
  return c;
}

static void ImmPricteni() {
  je();
  {
    KlSlovo ("+");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_addwim();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_addwim();
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_addwim();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void ImmOdecteni() {
  je();
  {
    KlSlovo ("-");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_subwim();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_subwim();
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_subwim();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void ImmNasobeni() {
  je();
  {
    KlSlovo ("*");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_muliim();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_mulwim();
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_mulwim();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void ImmAND() {
  je();
  {
    KlSlovo ("&");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_andwim();
        }
        /*     if(neb())
             {
               KlSlovo("WORD");
               if(pkk()){ c_andwim();  }
               if(neb()){
                 KlSlovo("IN_WORD");
                 if(pkk()){ c_mulwim();  }
               }
             }
         */
      }
      vse();
    }
  }
  vse();
}

static void ImmOR() {
  je();
  {
    KlSlovo ("|");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_orwim();
        }
        /*     if(neb())
             {
               KlSlovo("WORD");
               if(pkk()){ c_andwim();  }
               if(neb()){
                 KlSlovo("IN_WORD");
                 if(pkk()){ c_mulwim();  }
               }
             }
         */
      }
      vse();
    }
  }
  vse();
}

static void ImmXOR() {
  je();
  {
    KlSlovo ("WXOR");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_xorwim();
        }
        /*     if(neb())
             {
               KlSlovo("WORD");
               if(pkk()){ c_andwim();  }
               if(neb()){
                 KlSlovo("IN_WORD");
                 if(pkk()){ c_mulwim();  }
               }
             }
         */
      }
      vse();
    }
  }
  vse();
}

static void ImmDeleni() {
  je();
  {
    KlSlovo ("/");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_diviim();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_divwim();
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_divwim();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void ImmRovno() {
  je();
  {
    KlSlovo ("=");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_eqwim();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_eqwim();
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_eqwim();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void ImmNerovno() {
  je();
  {
    KlSlovo ("<>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_newim();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_newim();
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_newim();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static unsigned long int PevnaPromenna();

static void ImmUlozDirect() {
  unsigned long int a;

  je();
  {
    KlSlovo ("<R>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (neb()) {
          KlSlovo ("WORD");
        }
      }
      vse();
      if (zrtz()) {
        a=PevnaPromenna();
        if (zrtz()) {
          KlSlovo (":=");
          if (zrtz()) {
            je();
            {
              KlSlovo ("INT");
              if (pkk()) {
                BaseProcessorPtr->c_stimmd (a);
              }
              if (neb()) {
                KlSlovo ("WORD");
                if (pkk()) {
                  BaseProcessorPtr->c_stimmd (a);
                }
              }
            }
            vse();
          }
        }
      }
    }
  }
  vse();
}

static void Immediate() {
  char t;
  je();
  {
    t=ImmediateOperand();
    if (zrtz()) {
      je();
      {
        ImmPricteni();
        if (neb()) {
          ImmOdecteni();
          if (neb()) {
            ImmOR();
            if (neb()) {
              ImmXOR();
              if (neb()) {
                ImmNasobeni();
                if (neb()) {
                  ImmAND();
                  if (neb()) {
                    ImmDeleni();
                    if (neb()) {
                      ImmRovno();
                      if (neb()) {
                        ImmNerovno();
                        if (neb()) {
                          ImmUlozDirect();
                          if (neb()) {
                            nic();
                            if (pkk()) {
                              if (t=='I') {
                                BaseProcessorPtr->c_ldwim();
                              } else      {
                                BaseProcessorPtr->c_ldrim();
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Prima adresa                                                            */
/*---------------------------------------------------------------------------*/

static unsigned long int PevnaPromenna() {
//   char t[ID_DEL];
  unsigned long int a = 0,n = 0,d = 0,b = 0,min,max;

  je();
  {
    KlSlovo (",");
    if (zrtz()) {
      Cislo();
      if (zrtz()) {
        je();
        {
          KlSlovo (".");
          if (zrtz()) {
            je();
            {
              KlSlovo ("BIT");
              if (pkk()) {
                b=1;
              }
              if (neb()) {
                KlSlovo ("IN_BIT");
                if (pkk()) {
                  b=1;
                }
                if (neb()) {
                  KlSlovo ("INT");
                  if (pkk()) {
                    b=8;
                  }
                  if (neb()) {
                    KlSlovo ("WORD");
                    if (pkk()) {
                      b=8;
                    }
                    if (neb()) {
                      KlSlovo ("IN_WORD");
                      if (pkk()) {
                        b=8;
                      }
                    }
                  }
                }
              }
            }
            vse();
          }
        }
        vse();
        if (zrtz()) {
          a=SemPop();
          je();
          {
            _sled();
            do {
              KlSlovo ("#I");
              if (zrtz()) {
                Cislo();
                if (pkk()) {
                  n=SemPop();
                }
                if (zrtz()) {
                  Interval (&min,&max);
                  if (zrtz()) {
                    if (n<min || n>max) {
                      StringEnd();
                      ErrSynt ("Prekrocena mez pole");
                    }
                    if (zrtz()) {
                      KlSlovo ("[");
                      if (zrtz()) {
                        Cislo();
                        if (zrtz()) {
                          KlSlovo ("]");
                          if (pkk()) {
                            d=SemPop();
                            a=a+n*d*b;
                          }
                        }
                      }
                    }
                  }
                }
              }
            } while (sled_());
            if (zrtz()) {
              KlSlovo ("<T.");
              if (zrtz()) {
                Cislo();
                if (pkk()) {
                  d=SemPop();
                }
                if (zrtz()) {
                  KlSlovo (".");
                  if (zrtz()) {
                    Cislo();
                    if (pkk()) {
                      d+=SemPop();
                    }
                    if (zrtz()) {
                      KlSlovo (">");
                      if (zrtz()) {
                        je();
                        {
                          KlSlovo ("BIT");
                          if (neb()) {
                            KlSlovo ("IN_BIT");
                            if (neb()) {
                              KlSlovo ("INT");
                              if (neb()) {
                                KlSlovo ("WORD");
                                if (neb()) {
                                  KlSlovo ("IN_WORD");
                                }
                              }
                            }
                          }
                        }
                        vse();
                        if (pkk()) {
                          /* if(a>d)
                                                     {  StringEnd();
                                                        ErrSynt("Prekrocena mez pole");
                                                     }*/
                        }
                      }
                    }
                  }
                }
              }
            } if (neb()) {
              nic();
            }
          }
          vse();
        }
      }
    }
  }
  vse();
  return a;
}

/*---------------------------------------------------------------------------*/
/*   Operace s primou adresou na 16-bit promennych                           */
/*---------------------------------------------------------------------------*/

static void NaznakVycteni() {
  je();
  {
    KlSlovo ("@");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (neb()) {
          KlSlovo ("IN_BIT");
          if (neb()) {
            KlSlovo ("INT");
            if (neb()) {
              KlSlovo ("WORD");
              if (neb()) {
                KlSlovo ("IN_WORD");
              }
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrimeVycteni (unsigned long int a) {
  je();
  {
    KlSlovo ("@");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (pkk()) {
          BaseProcessorPtr->c_ldbd (a);
        }
        if (neb()) {
          KlSlovo ("IN_BIT");
          if (pkk()) {
            BaseProcessorPtr->c_ldbd (a);
          }
          if (neb()) {
            KlSlovo ("INT");
            if (pkk()) {
              BaseProcessorPtr->c_ldwd (a);
            }
            if (neb()) {
              KlSlovo ("WORD");
              if (pkk()) {
                BaseProcessorPtr->c_ldwd (a);
              }
              if (neb()) {
                KlSlovo ("IN_WORD");
                if (pkk()) {
                  BaseProcessorPtr->c_ldwd (a);
                }
              }
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrimePricteni (unsigned long int a) {
  je();
  {
    KlSlovo ("+");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_addwd (a);
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_addwd (a);
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_addwd (a);
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrimeOdecteni (unsigned long int a) {
  je();
  {
    KlSlovo ("-");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_subwd (a);
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_subwd (a);
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_subwd (a);
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrimeNasobeni (unsigned long int a) {
  je();
  {
    KlSlovo ("*");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_mulid (a);
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_mulwd (a);
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_mulwd (a);
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrimeAND (unsigned long int a) {
  je();
  {
    KlSlovo ("&");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_andwd (a);
        }
        //if(neb()){ KlSlovo("WORD");        if(pkk()){ c_mulwd(a);  }
        //if(neb()){ KlSlovo("IN_WORD");     if(pkk()){ c_mulwd(a);  }
        //}}
      }
      vse();
    }
  }
  vse();
}

static void PrimeOR (unsigned long int a) {
  je();
  {
    KlSlovo ("|");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_orwd (a);
        }
        //if(neb()){ KlSlovo("WORD");        if(pkk()){ c_mulwd(a);  }
        //if(neb()){ KlSlovo("IN_WORD");     if(pkk()){ c_mulwd(a);  }
        //}}
      }
      vse();
    }
  }
  vse();
}

static void PrimeXOR (unsigned long int a) {
  je();
  {
    KlSlovo ("WXOR");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_xorwd (a);
        }
        //if(neb()){ KlSlovo("WORD");        if(pkk()){ c_mulwd(a);  }
        //if(neb()){ KlSlovo("IN_WORD");     if(pkk()){ c_mulwd(a);  }
        //}}
      }
      vse();
    }
  }
  vse();
}

static void PrimeDeleni (unsigned long int a) {
  je();
  {
    KlSlovo ("/");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_divid (a);
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_divwd (a);
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_divwd (a);
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrimaOperace() {
  unsigned long int a;

  je();
  {
    a=PevnaPromenna();
    if (zrtz()) {
      je();
      {
        KlSlovo ("<SET>BIT");
        if (pkk()) {
          BaseProcessorPtr->c_setbd (a);
        }
        if (neb()) {
          KlSlovo ("<RES>BIT");
          if (pkk()) {
            BaseProcessorPtr->c_resbd (a);
          }
          if (neb()) {
            KlSlovo ("<INV>BIT");
            if (pkk()) {
              BaseProcessorPtr->c_invbd (a);
            }

            if (neb()) {
              NaznakVycteni();
              if (zrtz()) {
                je();
                {
                  PrimePricteni (a);
                  if (neb()) {
                    PrimeOdecteni (a);
                    if (neb()) {
                      PrimeOR (a);
                      if (neb()) {
                        PrimeXOR (a);
                        if (neb()) {
                          PrimeNasobeni (a);
                          if (neb()) {
                            PrimeAND (a);
                            if (neb()) {
                              PrimeDeleni (a);
                            }
                          }
                        }
                      }
                    }
                  }
                }
                vse();
              }
              if (neb()) {
                PrimeVycteni (a);
              }
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrimePrirazeni() {
  unsigned long int a;

  je();
  {
    KlSlovo ("<R>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (neb()) {
          KlSlovo ("INT");
          if (neb()) {
            KlSlovo ("WORD");
          }
        }
      }
      vse();
      if (zrtz()) {
        a=PevnaPromenna();
        if (zrtz()) {
          KlSlovo (":=");
          if (zrtz()) {
            je();
            {
              KlSlovo ("BIT");
              if (pkk()) {
                BaseProcessorPtr->c_stbd (a);
              }
              if (neb()) {
                KlSlovo ("INT");
                if (pkk()) {
                  BaseProcessorPtr->c_stwd (a);
                }
                if (neb()) {
                  KlSlovo ("WORD");
                  if (pkk()) {
                    BaseProcessorPtr->c_stwd (a);
                  }
                }
              }
            }
            vse();
          }
        }
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Neprima adresa                                                          */
/*---------------------------------------------------------------------------*/


static void Indexy() {
  unsigned long int a = 0,n = 0,d,min,max;
  a=0;
  je();
  {
    je();
    {
      KlSlovo ("#I");
      if (zrtz()) {
        Cislo();
        if (pkk()) {
          n=SemPop();
        }
        if (zrtz()) {
          Interval (&min,&max);
          if (zrtz()) {
            if (n<min || n>max) {
              StringEnd();
              ErrSynt ("Prekrocena mez pole");
            }
            if (zrtz()) {
              KlSlovo ("[");
              if (zrtz()) {
                Cislo();
                if (zrtz()) {
                  KlSlovo ("]");
                  if (pkk()) {
                    d=SemPop();
                    a=a+n*d;
                  }
                  if (zrtz()) {
                    _sled();
                    do {
                      KlSlovo ("#I");
                      if (zrtz()) {
                        Cislo();
                        if (pkk()) {
                          n=SemPop();
                        }
                        if (zrtz()) {
                          Interval (&min,&max);
                          if (zrtz()) {
                            if (n<min || n>max) {
                              StringEnd();
                              ErrSynt ("Prekrocena mez pole");
                            }
                            if (zrtz()) {
                              KlSlovo ("[");
                              if (zrtz()) {
                                Cislo();
                                if (zrtz()) {
                                  KlSlovo ("]");
                                  if (pkk()) {
                                    d=SemPop();
                                    a=a+n*d;
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    } while (sled_());
                  }
                }
              }
            }
          }
        }
      }
    }
    vse();
    if (a) {
      SemPush (a);
      BaseProcessorPtr->c_addwim();
    }
    if (neb()) {
      Interval (&min,&max);
      if (zrtz()) {
        KlSlovo ("[");
        if (zrtz()) {
          Cislo();
          if (zrtz()) {
            KlSlovo ("]");
            if (pkk()) {
              BaseProcessorPtr->c_mulwim();
              BaseProcessorPtr->c_addwp();
            }
          }
        }
      }
    }
  }
  vse();
}

static void NeprimyOperand() {
//   char t[ID_DEL];
  unsigned long int a = 0l;

  je();
  {
    je();
    {
      KlSlovo (",");
      if (zrtz()) {
        Cislo();
      }
    }
    vse();
    if (pkk()) {
      a=SemPop();
    }
    if (zrtz()) {
      je();
      {
        je();
        {
          KlSlovo (".");
          if (zrtz()) {
            je();
            {
              KlSlovo ("BIT");
              if (pkk()) {
                BaseProcessorPtr->c_ldbaim (a);
              }
              if (neb()) {
                KlSlovo ("IN_BIT");
                if (pkk()) {
                  BaseProcessorPtr->c_ldbaim (a);
                }
                if (neb()) {
                  KlSlovo ("INT");
                  if (pkk()) {
                    BaseProcessorPtr->c_ldaim (a);
                  }
                  if (neb()) {
                    KlSlovo ("WORD");
                    if (pkk()) {
                      BaseProcessorPtr->c_ldaim (a);
                    }
                    if (neb()) {
                      KlSlovo ("IN_WORD");
                      if (pkk()) {
                        BaseProcessorPtr->c_ldaim (a);
                      }
                      if (neb()) {
                        KlSlovo ("REAL");
                        if (pkk()) {
                          BaseProcessorPtr->c_ldaim (a);
                        }
                      }
                    }
                  }
                }
              }
            }
            vse();
          }
        }
        vse();

        if (neb()) { /*--------- skutecny parametr VAR (na stacku je adresa) --------*/

          je();
          {
            KlSlovo ("?");
            if (zrtz()) {
              je();
              {
                KlSlovo ("BIT");
                if (pkk()) {
                  BaseProcessorPtr->c_ldbaa (a);
                }
                if (neb()) {
                  KlSlovo ("IN_BIT");
                  if (pkk()) {
                    BaseProcessorPtr->c_ldbaa (a);
                  }
                  if (neb()) {
                    KlSlovo ("INT");
                    if (pkk()) {
                      BaseProcessorPtr->c_ldaa (a);
                    }
                    if (neb()) {
                      KlSlovo ("WORD");
                      if (pkk()) {
                        BaseProcessorPtr->c_ldaa (a);
                      }
                      if (neb()) {
                        KlSlovo ("IN_WORD");
                        if (pkk()) {
                          BaseProcessorPtr->c_ldaa (a);
                        }
                        if (neb()) {
                          KlSlovo ("REAL");
                          if (pkk()) {
                            BaseProcessorPtr->c_ldaa (a);
                          }
                        }
                      }
                    }
                  }
                }
              }
              vse();
            }
          }
          vse();

          if (neb()) { /*----------skutecny parametr (na stacku je hodnota) -----------*/

            je();
            {
              KlSlovo ("^");
              if (zrtz()) {
                je();
                {
                  KlSlovo ("BIT");
                  if (pkk()) {
                    BaseProcessorPtr->c_spba (a);
                  }
                  if (neb()) {
                    KlSlovo ("IN_BIT");
                    if (pkk()) {
                      BaseProcessorPtr->c_spba (a);
                    }
                    if (neb()) {
                      KlSlovo ("INT");
                      if (pkk()) {
                        BaseProcessorPtr->c_spa (a);
                      }
                      if (neb()) {
                        KlSlovo ("WORD");
                        if (pkk()) {
                          BaseProcessorPtr->c_spa (a);
                        }
                        if (neb()) {
                          KlSlovo ("IN_WORD");
                          if (pkk()) {
                            BaseProcessorPtr->c_spa (a);
                          }
                          if (neb()) {
                            KlSlovo ("REAL");
                            if (pkk()) {
                              BaseProcessorPtr->c_spar (a);
                            }
                          }
                        }
                      }
                    }
                  }
                }
                vse();
              }
            }
            vse();
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Result() {
//   unsigned long int a;

  je();
  {
    KlSlovo ("<R>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (pkk()) {
          BaseProcessorPtr->c_pushl();
        }
        if (neb()) {
          KlSlovo ("INT");
          if (neb()) {
            KlSlovo ("WORD");
            if (neb()) {
              KlSlovo ("REAL");
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Aritmeticka Operace                                                     */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

static void Pricteni() {
  je();
  {
    KlSlovo ("+");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_addw();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_addw();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_addr();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Odecteni() {
  je();
  {
    KlSlovo ("-");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_subw();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_subw();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_subr();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Nasobeni() {
  je();
  {
    KlSlovo ("*");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_muli();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_mulw();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_mulr();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void AND() {
  je();
  {
    KlSlovo ("&");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_andw();
        }
        //if(neb()){ KlSlovo("WORD");        if(pkk()){ c_mulw();  }
        //if(neb()){ KlSlovo("REAL");        if(pkk()){ c_mulr();  }
        //}}
      }
      vse();
    }
  }
  vse();
}
static void OR() {
  je();
  {
    KlSlovo ("|");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_orw();
        }
        //if(neb()){ KlSlovo("WORD");        if(pkk()){ c_mulw();  }
        //if(neb()){ KlSlovo("REAL");        if(pkk()){ c_mulr();  }
        //}}
      }
      vse();
    }
  }
  vse();
}
static void XOR() {
  je();
  {
    KlSlovo ("WXOR");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_xorw();
        }
        //if(neb()){ KlSlovo("WORD");        if(pkk()){ c_mulw();  }
        //if(neb()){ KlSlovo("REAL");        if(pkk()){ c_mulr();  }
        //}}
      }
      vse();
    }
  }
  vse();
}

static void Deleni() {
  je();
  {
    KlSlovo ("/");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_divi();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_divw();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_divr();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void AbsHod() {
  je();
  {
    KlSlovo ("<ABS>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_absi();
        }
        if (neb()) {
          KlSlovo ("REAL");
          if (pkk()) {
            BaseProcessorPtr->c_absr();
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void MemRead() {
  je();
  {
    KlSlovo ("<MEMREAD>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_memread();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_memread();
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Swap() {
  je();
  {
    KlSlovo ("<SWAP>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_swap();
        }
      }
      vse();
    }
  }
  vse();
}

static void Trunc() {
  KlSlovo ("<TRUNC>REAL");
  if (pkk()) {
    BaseProcessorPtr->c_truncr();
  }
}

static void Prirazeni() {
  je();
  {
    KlSlovo (":=");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (pkk()) {
          BaseProcessorPtr->c_stbi();
        }
        if (neb()) {
          KlSlovo ("INT");
          if (pkk()) {
            BaseProcessorPtr->c_stwi();
          }
          if (neb()) {
            KlSlovo ("WORD");
            if (pkk()) {
              BaseProcessorPtr->c_stwi();
            }
            if (neb()) {
              KlSlovo ("REAL");
              if (pkk()) {
                BaseProcessorPtr->c_stri();
              }
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PredaniSkP() {
  je();
  {
    KlSlovo ("<SKP>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (pkk()) {
          BaseProcessorPtr->c_skpb();
        }
        if (neb()) {
          KlSlovo ("INT");
          if (neb()) {
            KlSlovo ("WORD");
            if (neb()) {
              KlSlovo ("REAL");
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void PrevzetiRetVal() {
  je();
  {
    KlSlovo ("<RV>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (pkk()) {
          BaseProcessorPtr->c_retb();
        }
        if (neb()) {
          KlSlovo ("INT");
          if (neb()) {
            KlSlovo ("WORD");
            if (neb()) {
              KlSlovo ("REAL");
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void ChangeSig() {
  je();
  {
    KlSlovo ("<CHS>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_chsi();
        }
        if (neb()) {
          KlSlovo ("REAL");
          if (pkk()) {
            BaseProcessorPtr->c_chsr();
          }
        }
      }
      vse();
    }
  }
  vse();
}


static void AritmetickaOperace() {
  je();
  {
    Pricteni();
    if (neb()) {
      Odecteni();
      if (neb()) {
        OR();
        if (neb()) {
          XOR();
          if (neb()) {
            Nasobeni();
            if (neb()) {
              AND();
              if (neb()) {
                Deleni();
                if (neb()) {
                  AbsHod();
                  if (neb()) {
                    MemRead();
                    if (neb()) {
                      Swap();
                      if (neb()) {
                        Trunc();
                        if (neb()) {
                          Prirazeni();
                          if (neb()) {
                            PredaniSkP();
                            if (neb()) {
                              PrevzetiRetVal();
                              if (neb()) {
                                ChangeSig();
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  vse();
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Relace                                                                  */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

static void Rovno() {
  je();
  {
    KlSlovo ("=");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_eqw();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_eqw();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_eqr();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Mensi() {
  je();
  {
    KlSlovo ("<");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_lti();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_ltw();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_ltr();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Vetsi() {
  je();
  {
    KlSlovo (">");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_gti();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_gtw();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_gtr();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void MensiNeboRovno() {
  je();
  {
    KlSlovo ("<=");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_lei();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_lew();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_ler();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}


static void VetsiNeboRovno() {
  je();
  {
    KlSlovo (">=");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_gei();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_gew();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_ger();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Nerovno() {
  je();
  {
    KlSlovo ("<>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("INT");
        if (pkk()) {
          BaseProcessorPtr->c_new();
        }
        if (neb()) {
          KlSlovo ("WORD");
          if (pkk()) {
            BaseProcessorPtr->c_new();
          }
          if (neb()) {
            KlSlovo ("REAL");
            if (pkk()) {
              BaseProcessorPtr->c_ner();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Relace() {
  je();
  {
    VetsiNeboRovno();
    if (neb()) {
      MensiNeboRovno();
      if (neb()) {
        Vetsi();
        if (neb()) {
          Mensi();
          if (neb()) {
            Rovno();
            if (neb()) {
              Nerovno();
            }
          }
        }
      }
    }
  }
  vse();
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Prima logicka operace                                                   */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

static void PrimaLOp() {
  unsigned long int a;

  je();
  {
    KlSlovo ("<PUSHL>");
    if (zrtz()) {
      a=PevnaPromenna();
      if (zrtz()) {
        NaznakVycteni();
        if (zrtz()) {
          je();
          {
            KlSlovo ("<NOT>BIT");
            if (zrtz()) {
              je();
              {
                KlSlovo ("<OR>BIT");
                if (pkk()) {
                  BaseProcessorPtr->c_norbd (a);
                }
                if (neb()) {
                  KlSlovo ("<AND>BIT");
                  if (pkk()) {
                    BaseProcessorPtr->c_nandbd (a);
                  }
                }
              }
              vse();
            }
            if (neb()) {
              KlSlovo ("<OR>BIT");
              if (pkk()) {
                BaseProcessorPtr->c_orbd (a);
              }
              if (neb()) {
                KlSlovo ("<AND>BIT");
                if (pkk()) {
                  BaseProcessorPtr->c_andbd (a);
                }
              }
            }
          }
          vse();
        }
      }
    }
  }
  vse();
}


/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   Logicka operace                                                         */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

static void LogickaOperace() {
  je();
  {
    KlSlovo ("<OR>BIT");
    if (pkk()) {
      BaseProcessorPtr->c_orb();
    }
    if (neb()) {
      KlSlovo ("<AND>BIT");
      if (pkk()) {
        BaseProcessorPtr->c_andb();
      }
      if (neb()) {
        KlSlovo ("<XOR>BIT");
        if (pkk()) {
          BaseProcessorPtr->c_xorb();
        }
        if (neb()) {
          KlSlovo ("<NOT>BIT");
          if (pkk()) {
            BaseProcessorPtr->c_notb();
          }
          if (neb()) {
            KlSlovo ("<SET>BIT");
            if (pkk()) {
              BaseProcessorPtr->c_setbi();
            }
            if (neb()) {
              KlSlovo ("<RES>BIT");
              if (pkk()) {
                BaseProcessorPtr->c_resbi();
              }
              if (neb()) {
                KlSlovo ("<INV>BIT");
                if (pkk()) {
                  BaseProcessorPtr->c_invbi();
                }
                if (neb()) {
                  KlSlovo ("<PUSHL>");
                  if (pkk()) {
                    BaseProcessorPtr->c_pushl();
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  vse();
}

/*'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/
/*   zbyle akce                                                              */
/*,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,*/

static void TestPole() {
  unsigned long a = 0,d = 0;

  je();
  {
    KlSlovo ("<T.");
    if (zrtz()) {
      Cislo();
      if (pkk()) {
        a=SemPop() >>3;
      }
      if (zrtz()) {
        KlSlovo (".");
        if (zrtz()) {
          Cislo();
          if (pkk()) {
            d=SemPop();
          }
          if (zrtz()) {
            KlSlovo (">");
            if (zrtz()) {
              je();
              {
                KlSlovo ("BIT");
                if (pkk()) {
                  SemPush (d);
                }
                if (neb()) {
                  KlSlovo ("IN_BIT");
                  if (pkk()) {
                    SemPush (d);
                  }
                  if (neb()) {
                    KlSlovo ("INT");
                    if (pkk()) {
                      d /= 8;
                      SemPush (a+d);
                    }
                    if (neb()) {
                      KlSlovo ("WORD");
                      if (pkk()) {
                        d /= 8;
                        SemPush (a+d);
                      }
                      if (neb()) {
                        KlSlovo ("IN_WORD");
                        if (pkk()) {
                          d /= 8;
                          SemPush (a+d);
                        }
                        if (neb()) {
                          KlSlovo ("REAL");
                          if (pkk()) {
                            d /= 8;
                            SemPush (a+d);
                          }
                        }
                      }
                    }
                  }
                }
              }
              vse();
            }
          }
        }
      }
    }
  }
  vse();
  if (pkk()) {
    BaseProcessorPtr->c_testpole();
  }
}

static void Zobrazeni() {
  char u[ID_DEL];
//   int  t;

  je();
  {
    KlSlovo ("<D.STR>");
    if (zrtz()) {
      Retezec (u);
      if (pkk()) {
        BaseProcessorPtr->c_disps (u);
      }
    }
    if (neb()) {
      KlSlovo ("<DIS>");
      if (zrtz()) {
        je();
        {
          KlSlovo ("BIT");
          if (pkk()) {
            BaseProcessorPtr->c_dispb();
          }
          if (neb()) {
            KlSlovo ("IN_BIT");
            if (pkk()) {
              BaseProcessorPtr->c_dispb();
            }
            if (neb()) {
              KlSlovo ("WORD");
              if (pkk()) {
                BaseProcessorPtr->c_disp (1);
              }
              if (neb()) {
                KlSlovo ("IN_WORD");
                if (pkk()) {
                  BaseProcessorPtr->c_disp (1);
                }
                if (neb()) {
                  KlSlovo ("INT");
                  if (pkk()) {
                    BaseProcessorPtr->c_disp (2);
                  }
                  if (neb()) {
                    KlSlovo ("REAL");
                    if (pkk()) {
                      BaseProcessorPtr->c_disp (3);
                    }
                  }
                }
              }
            }
          }
        }
        vse();
      }
    }
  }
  vse();
}

static void VolaniStProcedury() {
  //- char u[ID_DEL];

  je();
  {
    KlSlovo ("<MEMWRITE>");
    if (zrtz()) {
      je();
      {
        KlSlovo ("WORD");
        if (pkk()) {
          BaseProcessorPtr->c_memwrite();
        }
        if (neb()) {
          KlSlovo ("INT");
          if (pkk()) {
            BaseProcessorPtr->c_memwrite();
          }
          if (neb()) {
            KlSlovo ("IN_WORD");
            if (pkk()) {
              BaseProcessorPtr->c_memwrite();
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Tisk() {
  char u[ID_DEL];

  je();
  {
    KlSlovo ("<P.STR>");
    if (zrtz()) {
      Retezec (u);
      if (pkk()) {
        BaseProcessorPtr->c_prnts (u);
      }
    }
    if (neb()) {
      KlSlovo ("<PRN>");
      if (zrtz()) {
        je();
        {
          KlSlovo ("BIT");
          if (pkk()) {
            BaseProcessorPtr->c_prntb();
          }
          if (neb()) {
            KlSlovo ("IN_BIT");
            if (pkk()) {
              BaseProcessorPtr->c_prntb();
            }
            if (neb()) {
              KlSlovo ("WORD");
              if (pkk()) {
                BaseProcessorPtr->c_prnt (1);
              }
              if (neb()) {
                KlSlovo ("IN_WORD");
                if (pkk()) {
                  BaseProcessorPtr->c_prnt (1);
                }
                if (neb()) {
                  KlSlovo ("INT");
                  if (pkk()) {
                    BaseProcessorPtr->c_prnt (2);
                  }
                  if (neb()) {
                    KlSlovo ("REAL");
                    if (pkk()) {
                      BaseProcessorPtr->c_prnt (3);
                    }
                  }
                }
              }
            }
          }
        }
        vse();
      }
    }
  }
  vse();
}

static void Volani() {
  char u[ID_DEL];
//   int  n;

  je();
  {
    KlSlovo ("$");
    if (zrtz()) {
      IdentVSeznamu (u);
//   if(zrtz()){   Cislo(); if(pkk()){  n = SemPop(); }
      if (zrtz()) {
        je();
        {
          SezPodtypu (T_proc);
          if (pkk()) {
            BaseProcessorPtr->c_pcall (u);
          }
          if (neb()) {
            SezPodtypu (T_fce);
            if (pkk()) {
              BaseProcessorPtr->c_fcall (u);
            }
          }
        }
        vse();
      }
    }
  }
  vse();
}

static void Vycteni() {
  je();
  {
    KlSlovo ("@");
    if (zrtz()) {
      je();
      {
        KlSlovo ("BIT");
        if (pkk()) {
          BaseProcessorPtr->c_ldbi();
        }
        if (neb()) {
          KlSlovo ("IN_BIT");
          if (pkk()) {
            BaseProcessorPtr->c_ldbi();
          }
          if (neb()) {
            KlSlovo ("INT");
            if (pkk()) {
              BaseProcessorPtr->c_ldwi();
            }
            if (neb()) {
              KlSlovo ("WORD");
              if (pkk()) {
                BaseProcessorPtr->c_ldwi();
              }
              if (neb()) {
                KlSlovo ("IN_WORD");
                if (pkk()) {
                  BaseProcessorPtr->c_ldwi();
                }
                if (neb()) {
                  KlSlovo ("REAL");
                  if (pkk()) {
                    BaseProcessorPtr->c_ldri();
                  }
                }
              }
            }
          }
        }
      }
      vse();
    }
  }
  vse();
}

static void Operace() {
  je();
  {
    PrimaLOp();
    if (neb()) {
      LogickaOperace();
      if (neb()) {
        AritmetickaOperace();
        if (neb()) {
          Relace();
          if (neb()) {
            Vycteni();
            if (neb()) {
              Zobrazeni();
              if (neb()) {
                VolaniStProcedury();
                if (neb()) {
                  Tisk();
                  if (neb()) {
                    Volani();
                  }
                }
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
/*   Celkem                                                                  */
/*---------------------------------------------------------------------------*/

extern int MCodeF;

void vyhodnot() {
  StrAdd ("|");
  StringInit (str_uk);
  if (MCodeF) {
    BaseProcessorPtr->c_unknown (str_uk+znak_uk);
  }

  Mezery();

  je();
  {
    _sled();
    do {
      PrimaOperace();
      if (neb()) {
        PrimePrirazeni();
        if (neb()) {
          NeprimyOperand();
          if (neb()) {
            Indexy();
            if (neb()) {
              Konverze();
              if (neb()) {
                Operace();
                if (neb()) {
                  Immediate();
                  if (neb()) {
                    Alokace();
                    if (neb()) {
                      Result();
                      if (neb()) {
                        TestPole();
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    } while (sled_());
    if (zrtz()) {
      KlSlovo ("|");
    }
  }
  vse();

  if (!uspech) {
    BaseProcessorPtr->c_unknown (str_uk+znak_uk);
    StringEnd();
    ErrFatal ("Internal compiler error - 'vyhodnot'");
  }

  StringEnd();
  StrDrop();
}
