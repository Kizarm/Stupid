/*###########################################################################*/
/*                                                                           */
/*   KONSTANTNI VYRAZ                                                        */
/*                                                                           */
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <string.h>

#include "inc/Znak.h"
#include "inc/Var.h"
#include "inc/List.h"
#include "inc/romram.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Znak.h"
#include "inc/fce.h"
#include "inc/chyby.h"


/*---------------------------------------------------------------------------*/
/*   konstanta()                                                             */
/*---------------------------------------------------------------------------*/

void Konstanta (char * t) {
  char u[ID_DEL];

  je();
  {
    RealCislo();
    if (pkk()) {
      strcpy (t,"REAL");
    }
    if (neb()) {
      Cislo();
      if (pkk()) {
        strcpy (t,"INT");
      }
      if (neb()) {
        IdentVSeznamu (u);
        if (zrtz()) {
          SezTypu (I_cons);
          if (zrtz()) {
            je();
            {
              SezPodtypu (T_ord);
              if (neb()) {
                SezPodtypu (T_real);
              }
            }
            vse();
            if (pkk()) {
              strcpy (t, (sez_uk->dtyp) +1);
              if (*t=='R') {
                RSemPush (sez_uk->fl);
              } else       {
                SemPush (sez_uk->atr);
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
/*   konst_cin()                                                             */
/*---------------------------------------------------------------------------*/
extern void KonstantniVyraz (char * t);
static void konst_prv (char * t) {
  je();
  {
    Konstanta (t);
    if (neb()) {
      KlSlovo ("(");
      if (zrtz()) {
        KonstantniVyraz (t);
        if (zrtz()) {
          KlSlovo (")");
        }
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   konst_soucin()                                                          */
/*---------------------------------------------------------------------------*/

void KonstantniCinitel (char * t) {
  char t1[30],t2[30];
  int   a, b  ;
  float fa,fb ;

  je();
  {
    konst_prv (t1);
    if (zrtz()) {
      _sled();
      do {
        KlSlovo ("*");
        if (zrtz()) {
          konst_prv (t2);
          if (pkk()) {
            if ( (t1[0]=='I') && (t2[0]=='I')) {
              a =       SemPop();
              b =       SemPop();
              SemPush (a*b);
            } else if ( (t1[0]=='R') && (t2[0]=='I')) {
              fa= (float) SemPop();
              fb=      RSemPop();
              RSemPush (fa*fb);
            } else if ( (t1[0]=='I') && (t2[0]=='R')) {
              fa=      RSemPop();
              fb= (float) SemPop();
              RSemPush (fa*fb);
              strcpy (t1,t2);
            } else if ( (t1[0]=='R') && (t2[0]=='R')) {
              fa=      RSemPop();
              fb=      RSemPop();
              RSemPush (fa*fb);
            }
          }

        }
        if (neb()) {
          KlSlovo ("/");
          if (zrtz()) {
            konst_prv (t2);
            if (pkk()) {
              if ( (t1[0]=='I') && (t2[0]=='I')) {
                b =       SemPop();
                a =       SemPop();
                if (b == 0)  ErrSynt ("Deleni nulou");
                SemPush (a/b);
              } else if ( (t1[0]=='R') && (t2[0]=='I')) {
                fb= (float) SemPop();
                fa=      RSemPop();
                if (fb == 0) ErrSynt ("Deleni nulou");
                RSemPush (fa/fb);
              } else if ( (t1[0]=='I') && (t2[0]=='R')) {
                fb=      RSemPop();
                fa= (float) SemPop();
                if (fb == 0) ErrSynt ("Deleni nulou");
                RSemPush (fa/fb);
                strcpy (t1,t2);
              } else if ( (t1[0]=='R') && (t2[0]=='R')) {
                fb=      RSemPop();
                fa=      RSemPop();
                if (fb == 0) ErrSynt ("Deleni nulou");
                RSemPush (fa/fb);
              }
            }
          }
        }
      } while (sled_());
    }
  }
  vse();
  if (pkk()) {
    strcpy (t,t1);
  }
}

void KonstantniSoucin (char * t) {
  je();
  {
    KonstantniCinitel (t);
    if (zrtz()) {
      je();
      {
        KlSlovo ("*");
        if (neb()) {
          KlSlovo ("/");
        }
      }
      vse();
      Opacne();
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   KonstantniVyraz()                                                       */
/*---------------------------------------------------------------------------*/

void KonstantniVyraz (char * t) {
  char t1[30],t2[30];
  int   a, b  ;
  float fa,fb ;

  je();
  {
    je();
    {
      KonstantniSoucin (t1);
      if (neb()) {
        KlSlovo ("-");
        if (zrtz()) {
          KonstantniSoucin (t1);
          if (pkk()) {
            if (t1[0]=='I')               {
              a =       SemPop();
              SemPush (-a);
            }
            if (t1[0]=='R')               {
              fa=      RSemPop();
              RSemPush (-fa);
            }
          }
        }
      }
    }
    vse();

    if (zrtz()) {
      _sled();
      do {
        KlSlovo ("+");
        if (zrtz()) {
          KonstantniSoucin (t2);
          if (pkk()) {
            if ( (t1[0]=='I') && (t2[0]=='I')) {
              a =       SemPop();
              b =       SemPop();
              SemPush (a+b);
            } else if ( (t1[0]=='R') && (t2[0]=='I')) {
              fa= (float) SemPop();
              fb=      RSemPop();
              RSemPush (fa+fb);
            } else if ( (t1[0]=='I') && (t2[0]=='R')) {
              fa=      RSemPop();
              fb= (float) SemPop();
              RSemPush (fa+fb);
              strcpy (t1,t2);
            } else if ( (t1[0]=='R') && (t2[0]=='R')) {
              fa=      RSemPop();
              fb=      RSemPop();
              RSemPush (fa+fb);
            }
          }

        }
        if (neb()) {
          KlSlovo ("-");
          if (zrtz()) {
            KonstantniSoucin (t2);
            if (pkk()) {
              if ( (t1[0]=='I') && (t2[0]=='I')) {
                b =       SemPop();
                a =       SemPop();
                SemPush (a-b);
              } else if ( (t1[0]=='R') && (t2[0]=='I')) {
                fb= (float) SemPop();
                fa=      RSemPop();
                RSemPush (fa-fb);
              } else if ( (t1[0]=='I') && (t2[0]=='R')) {
                fb=      RSemPop();
                fa= (float) SemPop();
                RSemPush (fa-fb);
                strcpy (t1,t2);
              } else if ( (t1[0]=='R') && (t2[0]=='R')) {
                fb=      RSemPop();
                fa=      RSemPop();
                RSemPush (fa-fb);
              }
            }

          }
        }
      } while (sled_());
    }
  }
  vse();
  if (pkk()) {
    strcpy (t,t1);
  }
}

