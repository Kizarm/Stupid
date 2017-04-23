/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   Prekladac                                                               */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <string.h>

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/fce.h"


/*===========================================================================*/
/*                                                                           */
/* Definice konstant                                                         */
/*                                                                           */
/*===========================================================================*/

static void DeklaraceEnum() {
  LPOLOZKA p= {"",I_cons,T_ord,"!INT",16,0,0,0,0.,0,0,0,0};
//   char u[ID_DEL];
  unsigned int i;

  i=0;
  je();
  {
    KlSlovo ("ENUM");
    if (zrtz()) {
      NovyId (p.iden);
      if (pkk()) {
        p.atr=i++;
        SezUloz (&p);
      }
      if (zrtz()) {
        _sled();
        do {
          KlSlovo (",");
          if (zrtz()) {
            NovyId (p.iden);
            if (pkk()) {
              p.atr=i++;
              SezUloz (&p);
            }
          }
        } while (sled_());
        if (zrtz()) {
          NKlSlovo (";");
        }
      }
    }
  }
  vse();
}

void DeklaraceKonst() {
  LPOLOZKA p= {"",I_cons,0,"!",0,0,0,0,0.,0,0,0,0};
  char u[ID_DEL];

  je();
  {
    KlSlovo ("CONST");
    if (zrtz()) {
      _sled();
      do {
        DeklaraceEnum();
        if (neb()) {
          KonstantniVyraz (u);
          if (zrtz()) {
            KlSlovo ("#");
            if (zrtz()) {
              NovyId (p.iden);
              if (zrtz()) {
                NKlSlovo (";");
                if (pkk()) {
                  strcpy (p.dtyp   ,"!");
                  strcat (p.dtyp   , u);
                  if (u[0]=='I') {
                    p.podtyp = T_ord   ;
                    p.size   = L_int   ;
                    p.atr    = SemPop();
                  } else if (u[0]=='R') {
                    p.podtyp = T_real   ;
                    p.size   = L_real   ;
                    p.fl     = RSemPop();
                  }
                  SezUloz (&p);
                }
              }
            }
          }
        }
      } while (sled_());
      if (zrtz()) {
        NKlSlovo ("END");
      }
    }
  }
  vse();
}
