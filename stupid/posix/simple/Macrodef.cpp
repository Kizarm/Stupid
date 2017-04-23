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
/* Definice promennych                                                       */
/*                                                                           */
/*===========================================================================*/

void DeklaraceMacro() {
  LPOLOZKA p= {"",I_mac,T_mac,"",0,0,0,0,0.,0,0,0,0};
  char u[ID_DEL];
  char * s;
  long l1,l2,l3;

  s=0;

  je();
  {
    KlSlovo ("SYMBOL");
    if (zrtz()) {
      _sled();
      do {
        l1=znak_uk;
        Vyraz (u);
        l2=znak_uk;
        if (zrtz()) {
          KlSlovo ("#");
          if (zrtz()) {
            NovyId (u);
            if (zrtz()) {
              NKlSlovo (";");
              if (pkk()) {
                strcpy (p.iden,u);
                l3=znak_uk;
                znak_uk=l1;
                s=p.dtyp;
                while (znak_uk!=l2) {
                  ZnakCti();
                  *s++=znak_posl;
                }
                s--;
                while ( (*s == ' ') || (*s == 0x0d)
                        || (*s == 0x0a) || (*s == 0x09)
                        || (*s == 0x1a)) {
                  s--;
                }
                s++;
                /*     *s++='\1'; */
                *s  ='\0';
                znak_uk=l3;
                SezUloz (&p);
                StrDrop();

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
