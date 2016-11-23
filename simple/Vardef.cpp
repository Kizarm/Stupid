/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   Prekladac                                                               */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <string.h>

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/procesor.h"
#include "inc/fce.h"
#include "inc/chyby.h"

int VarSaved; // indikuje pouziti funkce SaveVars

/*===========================================================================*/
/*                                                                           */
/* Definice promennych                                                       */
/*                                                                           */
/*===========================================================================*/

static void SaveVars (char * t,unsigned long int * rp) {
  LPOLOZKA p= {"",I_var,T_ord,"",0,0,0,-1,0.,0,0,0,0};
  POLOZKA * r;
  long int  i,n,m,s;

  s = *rp;

  strcpy (p.dtyp,t);
  p.podtyp=UrciPtyp (p.dtyp);
  i=n=SemPocetAtr();
  m=UrciSize (p.dtyp);
  p.size  =m;
  p.trida =C_stat;

  if ( (m>7) && (s&7)) {
    s = (s&0xfffffff8) +8;
  }

  while (n--) {
    p.atr = s + m*n;
    r = (POLOZKA *) SemPop();
    SezDopln (r ,&p);
  }
  n=m*i;
  s=s+n;
  *rp=s;

  VarSaved = ~0;
}

/*---------------------------------------------------------------------------*/

static void SynonymumVar() {
  LPOLOZKA p= {"",I_var,0,"",0,0,0,-1,0.,0,0,0,0};
  char u[ID_DEL];
  POLOZKA * r = NULL;

  je();
  {
    je();
    {
      IdentVSeznamu (p.iden);
      if (zrtz()) {
        SezTypu (I_var);
      }
      if (neb()) {
        PredefVar (p.iden);
      }
    }
    vse();
    if (pkk()) {
      r=sez_uk;
    }
    if (zrtz()) {
      KlSlovo ("#");
      if (zrtz()) {
        NovyId (p.iden);
        if (zrtz()) {
          je();
          {
            KlSlovo (":");
            if (zrtz()) {
              Type (u);
              JinakCh ("Ocekavan typ");
              if (r->trida == C_auto) {
                ErrSynt ("Synonymum k parametru !");
              }
            }
            if (neb()) {
              nic();
              strcpy (u,r->dtyp);
            }
          }
          vse();
        }
      }
    }
  }
  vse();
  if (pkk()) {
    p.podtyp = UrciPtyp (u)     ;
    strcpy (p.dtyp   , u)  ;
    p.size   = UrciSize (u)     ;
    p.hloubka= r->hloubka      ;
    p.trida  = r->trida        ;
    p.atr    = r->atr          ;
    SezUloz (&p);
  }
}

/*---------------------------------------------------------------------------*/

static void OtaznikVar (char * t) {
  LPOLOZKA p= {"",I_var,0,"",0,0,0,-1,0.,0,0,0,0};
//   char u[ID_DEL];
//   POLOZKA *r;

  je();
  {
    je();
    {
      KlSlovo ("D?");
      if (pkk()) {
        strcpy (t,"WORD");
      }
      if (neb()) {
        KlSlovo ("M?");
        if (pkk()) {
          strcpy (t,"BIT");
        }
      }
    }
    vse();
    if (zrtz()) {
      NKlSlovo ("#");
      if (zrtz()) {
        NovyId (p.iden);
        if (pkk()) {
          SemPush ( (ATRIBUT) SezUlozId (&p));
        }
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/

void DeklaraceVar (unsigned long * s) {
  char u[ID_DEL];

  je();
  {
    KlSlovo ("VAR");
    if (zrtz()) {
      _sled();
      do {
        OtaznikVar (u);
        if (zrtz()) {
          NKlSlovo (";");
          if (pkk()) {
            SaveVars (u,s);
          }
        }
        if (neb()) {
          SynonymumVar();
          if (zrtz()) {
            NKlSlovo (";");
          }
          if (neb()) {
            IdList ("");
            if (zrtz()) {
              KlSlovo (":");
              if (zrtz()) {
                Type (u);
                if (zrtz()) {
                  NKlSlovo (";");
                  if (pkk()) {
                    SaveVars (u,s);
                  }
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
