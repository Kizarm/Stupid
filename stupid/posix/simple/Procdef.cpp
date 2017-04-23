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
#include "inc/procesor.h"
#include "inc/fce.h"
#include "inc/chyby.h"

#define  MAX_ZANORENI (10)
extern   void blok();
static   int zanoreni;
extern int VarSaved;

/*===========================================================================*/
/*                                                                           */
/* PROCEDURY                                                                 */
/*                                                                           */
/*===========================================================================*/

extern int BitFlg;

static void ParameterDeclaration (char * t) {
  LPOLOZKA p= {"",I_var ,T_ptr,"",0L,0,0,0L,0,0,0,0};
  POLOZKA * r;
  char     u[ID_DEL],v[ID_DEL];
  unsigned int     n,m;
  unsigned long    a;

  je();
  {
    je();
    {
      KlSlovo ("VAR");
      if (pkk()) {
        strcpy (u,"@");
      }
      if (neb()) {
        nic();
        if (pkk()) {
          strcpy (u,"");
        }
      }
    }
    vse();
    if (zrtz()) {
      IdList ("");
      if (zrtz()) {
        KlSlovo (":");
        if (zrtz()) {
          TypeId (v);
        }
      }
    }
  }
  vse();

  if (pkk()) {
    strcpy (p.dtyp,u);
    strcat (p.dtyp,v);
    m=n=SemPocetAtr();
    strcpy (t,"");
    a=UrciSize (p.dtyp);
    if (BitFlg) {
      a=16;
    }
    p.trida = C_auto;
    p.size  = a;
    while (n--) {
      strcat (t,p.dtyp);
      if (n) {
        strcat (t,",");
      }
      p.atr=AutoPtr+ (n+1) *a;
      r = (POLOZKA *) SemPop();
      SezDopln (r ,&p);
    }
    AutoPtr+=m*a;
    VarSaved = ~0;
  }
}

/*---------------------------------------------------------------------------*/

static void FormParList (char * t) {
  char u[ID_DEL],v[ID_DEL];

  AutoPtr=0;
  je();
  {
    KlSlovo ("(");
    if (pkk()) {
      strcpy (u,"(");
    }
    if (zrtz()) {
      ParameterDeclaration (v);
      if (pkk()) {
        strcat (u,v);
      }
      if (zrtz()) {
        _sled();
        do {
          KlSlovo (";");
          if (pkk()) {
            strcat (u,",");
          }
          if (zrtz()) {
            ParameterDeclaration (v);
            if (pkk()) {
              strcat (u,v);
            }
          }
        } while (sled_());
        if (zrtz()) {
          KlSlovo (")");
          if (pkk()) {
            strcat (u,")");
          }
          JinakCh ("Chyba v seznamu parametru");
        }
      }
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}


/*---------------------------------------------------------------------------*/

void ProcedureDef() {
  char u[ID_DEL];
  LPOLOZKA p= {"",I_cons,T_proc,"!",L_ptr,0,0,-1,0.,0,0,0,0};
  int  pos = -1;
//   POLOZKA *r;

  zanoreni = 0 ;
  je();
  {
    je();
    {
      KlSlovo ("PROCEDURE");
      if (neb()) {
        KlSlovo ("SUBROUTINE");
      }
      pos = ZnakLine();
    }
    vse();
    if (zrtz()) {
      NovyId (p.iden);
      if (pkk()) {
        Hloubka++;
      }
      if (zrtz()) {
        je();
        {
          FormParList (u);
          if (neb()) {
            nic();
            if (pkk()) {
              strcpy (u,"()");
            }
          }
        }
        vse();
        if (pkk()) {
          /*WriteMyLine2MF(pos); */BaseProcessorPtr->c_proced (&p, pos);
        }
        if (zrtz()) {
          je();
          {
            DeklaraceVar (&RamPtr);
            if (neb()) {
              nic();
            }
          }
          vse();
          if (zrtz()) {
            blok();
            if (zrtz()) {
              WriteLine2MF (~0);
              NKlSlovo ("RETURN");
              if (pkk()) {
                SezZapomenHladinu (Hloubka--);
                strcat (p.dtyp,u);
                p.trida = C_stat  ;
                p.fpl   = AutoPtr ;
                p.hz    = zanoreni;
                p.hst   = MaxSpInModule;
                SezUloz (&p);
                BaseProcessorPtr->c_preturn (&p);
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

void FunctionDef() {
  char u[ID_DEL],v[ID_DEL];
  LPOLOZKA p= {"",I_cons,T_fce ,"!",L_ptr,0,0,-1,0.,0,0,0,0};
  LPOLOZKA q= {"",I_var ,T_ord ,"" ,0    ,0,0, 0,0.,0,0,0,0};
  int  pos = -1;

  zanoreni = 0 ;
  je();
  {
    KlSlovo ("FUNCTION");
    pos = ZnakLine();
    if (zrtz()) {
      NovyId (p.iden);
      if (pkk()) {
        Hloubka++;
      }
      if (zrtz()) {
        je();
        {
          FormParList (u);
          if (neb()) {
            nic();
            if (pkk()) {
              strcpy (u,"()");
            }
          }
        }
        vse();
        if (zrtz()) {
          KlSlovo (":");
          if (zrtz()) {
            TypeId (v);
            JinakCh ("Ocekavan identifikator typu");
            if (zrtz()) {
              NKlSlovo (";");
              if (pkk()) {
                strcpy (q.iden,p.iden);
                strcpy (q.dtyp,v);
                q.size  = UrciSize (v);
                q.trida = C_auto;
                q.atr   = BaseProcessorPtr->c_retval();
                SezUloz (&q);
                BaseProcessorPtr->c_func (&p, pos);
              }
              if (zrtz()) {
                je();
                {
                  DeklaraceVar (&RamPtr);
                  if (neb()) {
                    nic();
                  }
                }
                vse();
                if (zrtz()) {
                  blok();
                  if (zrtz()) {
                    NKlSlovo ("RETURN");
                    if (pkk()) {
                      WriteLine2MF (0);
                      SezZapomenHladinu (Hloubka--);
                      strcat (p.dtyp,u);
                      strcat (p.dtyp,v);
                      p.trida = C_stat  ;
                      p.rvs   = q.size  ;
                      p.fpl   = AutoPtr ;
                      p.hz    = zanoreni;
                      p.hst   = MaxSpInModule;
                      SezUloz (&p);
                      BaseProcessorPtr->c_freturn (&p);
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


/*===========================================================================*/
/*===========================================================================*/

static void SubType (char * v,const char * z) {
  char * t;
  t=v;
  do {
    if (*v!=*z) {
      AddInfo ("\n:%s:%s",v,z);
      ErrSynt ("Rozdilny pocet nebo typy parametru");
    }
    v++;
    z++;
  } while (*z!='\0');
  //strcpy (t,v);
  ByteCopy (t,v);
}

static void TypePick (char * t,char * type) {
  do {
    *t++=*type++;
  } while ( (*type != ',') && (*type != ')') && (*type != '\0'));
  *t='\0';
}

/*===========================================================================*/
/*                                                                           */
/* Skutecne parametry                                                        */
/*                                                                           */
/*===========================================================================*/

extern void VyrazTypu (const char * typ);

static void SkutecnyParametr (char * t) {
  char u[ID_DEL],v[ID_DEL];
  memset (u, 0, ID_DEL);
  memset (v, 0, ID_DEL);

  if (*t=='@') {
    //strcpy (t,t+1);
    ByteCopy (t,t+1);
    VarReference (u);
    if (pkk()) {
      SubType (t,u);
      StrApp();
    }
  } else {
    TypePick (u,t);
    VyrazTypu (u);
    if (pkk()) {
      SubType (t,u);
      StrApp();
      StrAdd (v);
    }
  }
}

static void SkutecneParametry (char * t) {

  je();
  {
    KlSlovo ("()");
    if (pkk()) {
      SubType (t,"()");
    }
    if (neb()) {
      KlSlovo ("(");
      if (pkk()) {
        SubType (t,"(");
      }
      if (zrtz()) {
        je();
        {
          SkutecnyParametr (t);
          if (zrtz()) {
            _sled();
            do {
              KlSlovo (",");
              if (pkk()) {
                //-printf ("1---SubType:%s\n", t);
                SubType (t,",");
                //-printf ("2---SubType:%s\n", t);
              }
              if (zrtz()) {
                SkutecnyParametr (t);
              }
            } while (sled_());
          }
          if (neb()) {
            nic();
          }
        }
        vse();
        if (zrtz()) {
          KlSlovo (")");
          if (pkk()) {
            SubType (t,")");
          }
        }
      }
      if (neb()) {
        nic();
        if (pkk()) {
          SubType (t,"()");
        }
      }
    }
  }
  vse();
}

/*===========================================================================*/
/*                                                                           */
/* VOLANI PROCEDURY                                                          */
/*                                                                           */
/*===========================================================================*/
static void KontrolaZanoreni() {
  if ( (sez_uk->hz+1) >zanoreni)  {
    zanoreni=sez_uk->hz+1;
  }
  if (zanoreni > MAX_ZANORENI) {
    ErrSynt ("Prekrocena povolena hloubka zanoreni procedur");
  }
}

void ProcedureCall() {
  char u[ID_DEL],v[ID_DEL];//, w[ID_DEL];

  je();
  {
    WriteLine2MF (0);
    IdentVSeznamu (u);
    if (zrtz()) {
      SezTypu (I_cons);
      if (zrtz()) {
        SezPodtypu (T_proc);
        if (pkk()) {
          strcpy (v,sez_uk->dtyp+1);
          KontrolaZanoreni();
          StrPush (" ");
        }
        if (zrtz()) {
          SkutecneParametry (v);
          if (pkk()) {
            StrAdd ("$");
            StrAdd (u); /*WriteLine2MF(0);*/
          }
        }
      }
    }
  }
  vse();

}

void FunctionCall (char * t) {
  char u[ID_DEL],v[ID_DEL],s[ID_DEL];
  memset (u, 0, ID_DEL);
  memset (v, 0, ID_DEL);
  memset (s, 0, ID_DEL);

  je();
  {
    WriteLine2MF (0);
    IdentVSeznamu (u);
    if (zrtz()) {
      SezTypu (I_cons);
      if (zrtz()) {
        SezPodtypu (T_fce);
        if (pkk()) {
          strcpy (v,sez_uk->dtyp+1);
          KontrolaZanoreni();
          sprintf (s,"~%d",sez_uk->rvs);
          StrPush (s);
        }
        if (zrtz()) {
          SkutecneParametry (v);
          //-printf ("SkutecneParametry:%s\n", v);
          if (pkk()) {
            StrAdd ("$");
            StrAdd (u);
          }
        }
      }
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,v);
    sprintf (s,"<RV>%s",v);
    StrAdd (s);
  }
  //-printf ("FunctionCall:%s\n", t);
}
