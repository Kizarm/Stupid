/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*  Preklad vyrazu pro SIMPLE                                                */
/*                                                                           */
/*  20.09.97 - precedence operatoru unarni > arit > relace > logicke         */
/*                                                                           */
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
#include "inc/chyby.h"

static char pl[SSL],pn[SSL];

extern LPOLOZKA TypesTab[5];

/*---------------------------------------------------------------------------*/

static int CopyTab[N_last][N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  /* int   */  {N_int , -1    , -1    , -1    , -1     , -1}    ,
  /* word  */  {-1    , N_word, -1    , -1    , -1     , -1}    ,
  /* wori  */  {-1    , -1    , -1    , -1    , -1     , -1}    ,
  /* bool  */  {-1    , -1    , -1    ,N_bool , -1     , -1}    ,
  /* boli  */  {-1    , -1    , -1    , -1    , -1     , -1}    ,
  /* real  */  {-1    , -1    , -1    , -1    , -1     , N_real }
};

/*---------------------------------------------------------------------------*/

static int NeedTab[N_last][N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  /* int   */   {0    ,  0    , 0     , -1    , -1     , N_real },
  /* word  */   {0    ,  0    , 0     , -1    , -1     , N_real },
  /* wori  */   {0    ,  0    , 0     , -1    , -1     , N_real },
  /* bool  */  {-1    , -1    , -1    ,  0    ,  0     , -1     },
  /* boli  */  {-1    , -1    , -1    ,  0    ,  0     , -1     },
  /* real  */  {N_real, N_real, N_real, -1    , -1     ,  0}
};


/*---------------------------------------------------------------------------*/

static int ConvTab[N_last][N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  /* int   */  {N_int , N_int , N_int , -1    , -1     , N_int  },
  /* word  */  {N_word, N_word, N_word, -1    , -1     , N_word },
  /* wori  */  {-1    , -1    , -1    , -1    , -1     , -1     },
  /* bool  */  {-1    , -1    , -1    , N_bool, N_bool , -1     },
  /* boli  */  {-1    , -1    , -1    , -1    , -1     , -1     },
  /* real  */  {N_real, N_real, N_real, -1    , -1     , N_real}
};


/*---------------------------------------------------------------------------*/

static int MathTab[N_last][N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  /* int   */  {N_int , N_word, N_word, -1    , -1     , N_real },
  /* word  */  {N_word, N_word, N_word, -1    , -1     , N_real },
  /* wori  */  {N_word, N_word, N_word, -1    , -1     , N_real },
  /* bool  */  {-1    , -1    , -1    , -1    , -1     , -1     },
  /* boli  */  {-1    , -1    , -1    , -1    , -1     , -1     },
  /* real  */  {N_real, N_real, N_real, -1    , -1     , N_real}
};


/*---------------------------------------------------------------------------*/

static int ChsTab[N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  N_int , -1    , -1    , -1    , -1    , N_real
};

//static int WordTab[N_last]={

/*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
/*-------------------------------------------------*/
//              -1   , N_word, N_word, -1    , -1    , -1     };


/*---------------------------------------------------------------------------*/
static int RealTab[N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  -1    , -1    , -1    , -1    , -1    , N_real
};


/*---------------------------------------------------------------------------*/

static int LogTab[N_last][N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  /* int   */  {-1    , -1    , -1    , -1    , -1    , -1     },
  /* word  */  {-1    , -1    , -1    , -1    , -1    , -1     },
  /* wori  */  {-1    , -1    , -1    , -1    , -1    , -1     },
  /* bool  */  {-1    , -1    , -1    , N_bool, N_bool, -1     },
  /* boli  */  {-1    , -1    , -1    , N_bool, N_bool, -1     },
  /* real  */  {-1    , -1    , -1    , -1    , -1    , -1}
};

/*---------------------------------------------------------------------------*/

static int WordTab[N_last][N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  /* int   */  {N_word, N_word, N_word, -1    , -1     , -1    },
  /* word  */  {N_word, N_word, N_word, -1    , -1     , -1    },
  /* wori  */  {-1    , -1    , -1    , -1    , -1     , -1    },
  /* bool  */  {-1    , -1    , -1    , -1    , -1     , -1    },
  /* boli  */  {-1    , -1    , -1    , -1    , -1     , -1    },
  /* real  */  {-1    , -1    , -1    , -1    , -1     , -1}
};

/*---------------------------------------------------------------------------*/

static int NotTab[N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  -1    , -1    , -1    , N_bool, N_bool, -1
};

static int OneWordTab[N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  N_word, N_word, -1    , -1    , -1    , -1
};


/*---------------------------------------------------------------------------*/

void ConvType (const char * to, char * from) {
  char s[ID_DEL];
  int nl,nn,nv;
  memset (s, 0, ID_DEL);

  nl=TypeNum (from);
  nn=TypeNum (to);
  nv=ConvTab[nn][nl];
  if (nv==-1) {
    sprintf (s,"Nelze konvertovat <%s> na <%s>",from,to);
    ErrSynt (s);
  }

  if (NeedTab[nl][nv]) {
    sprintf (s,"(%s:%s) ",TypesTab[nl].iden,TypesTab[nv].iden);
    StrAdd (s);
    strcpy (from,to);
  }
}

/*---------------------------------------------------------------------------*/

void DualOper (char * tl, char * tn, const char * o, int tab[N_last][N_last]) {
  int nl,nn,nv;
  char s[ID_DEL];

  memset (s, 0, ID_DEL);
  StrPop (pn);

  nl=TypeNum (tl);
  nn=TypeNum (tn);
  nv=tab[nl][nn];

  if (nv==-1) {
    sprintf (s,"Nekompatibilni typy <%s><%s> pro '%s'",tl,tn,o);
    ErrSynt (s);
  }

  if (NeedTab[nl][nv] != 0) {
    sprintf (s,"(%s:%s) ",TypesTab[nl].iden,TypesTab[nv].iden);
    StrAdd (s);
    strcpy (tl,tn);
  }

  StrAdd (pn);
  if (NeedTab[nn][nv] != 0) {
    sprintf (s,"(%s:%s) ",TypesTab[nn].iden,TypesTab[nv].iden);
    StrAdd (s);
  }

  if (strcmp (o,"")) {
    sprintf (s,"%s%s ",o,TypesTab[nv].iden);
    StrAdd (s);
  }
}

void SwapOper() {
  StrPop (pn);
  StrPop (pl);
  StrPush (pn);
  StrPush (pl);
}

void InsOper (char * o) {
  StrPop (pn);
  StrPush (o);
  StrAdd (pn);
}

/*---------------------------------------------------------------------------*/

void OneOper (char * tl, const char * o, int tab[]) {
  char s[ID_DEL];
  int nl,nv;
  memset (s, 0, ID_DEL);

  nl=TypeNum (tl);
  nv=tab[nl];
  if (nv==-1) {
    sprintf (s,"Nekompatibilni typ <%s> pro '%s'",tl,o);
    ErrSynt (s);
  }

  sprintf (s,"%s%s",o,TypesTab[nv].iden);
  StrAdd (s);

  if (nv!=nl) {
    strcpy (tl,TypesTab[nv].iden);
  }
}

void MathOper (char * tl, char * tn, const char * o) {
  DualOper (tl,tn,o,MathTab);
}
void LogOper (char * tl, char * tn, const char * o) {
  DualOper (tl,tn,o,LogTab);
}
void ChsOper (char * tl, const char * o)           {
  OneOper (tl,   o,ChsTab);
}
void RealOper (char * tl, const char * o)           {
  OneOper (tl,   o,RealTab);
}
void NotOper (char * tl, const char * o)           {
  OneOper (tl,   o,NotTab);
}
void CopyOper (char * tl, const char * o)           {
  DualOper (tl,tl,o,CopyTab);
}
void WordOper (char * tl, char * tn, const char * o) {
  DualOper (tl,tl,o,WordTab);
}
void ChsWordOper (char * tl, const char * o)       {
  OneOper (tl,   o,OneWordTab);
}

/*---------------------------------------------------------------------------*/
/*   Vyraz                                                                   */
/*---------------------------------------------------------------------------*/

extern void Vyraz (char * typ);
extern void VyrazTypu (const char * typ);

void VyrazVZ (char * typ) {
  je();
  {
    KlSlovo ("(");
    if (zrtz()) {
      Vyraz (typ);
      if (zrtz()) {
        NKlSlovo (")");
      }
    }
  }
  vse();
}

void VyrazOdkVZ (char * typ) {
  je();
  {
    KlSlovo ("(");
    if (zrtz()) {
      Vyraz (typ);
      if (zrtz()) {
        NKlSlovo (")");
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Standardni funkce                                                       */
/*---------------------------------------------------------------------------*/

void StandardFce (char * t) {
  char u[ID_DEL];
//char v[ID_DEL];
  memset (u, 0, ID_DEL);

  je();
  {
    KlSlovo ("ABS");
    if (zrtz()) {
      VyrazVZ (u);
      if (pkk()) {
        ChsOper (u,"<ABS>");
      }
    }
    if (neb()) {
      KlSlovo ("TRUNC");
      if (zrtz()) {
        VyrazVZ (u);
        if (pkk()) {
          RealOper (u,"<TRUNC>");
        }
      }
    }
    if (neb()) {
      KlSlovo ("MEMREAD");
      if (zrtz()) {
        VyrazVZ (u);
        if (pkk()) {
          ChsWordOper (u,"<MEMREAD>");
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
/*   prvek vyrazu                                                            */
/*---------------------------------------------------------------------------*/

void Macro() {
  char u[ID_DEL];
  memset (u, 0, ID_DEL);
  je();
  {
    IdentVSeznamu (u);
    if (zrtz()) {
      SezTypu (I_mac);
    }
  }
  vse();
  if (pkk()) {
    MacroInit (sez_uk->dtyp);
  }
}

void JPrvek (char * typ) {
  char t[ID_DEL],u[ID_DEL];
  int  ki;
  memset (t, 0, ID_DEL);
  memset (u, 0, ID_DEL);

  Macro();
  je();
  {
    FunctionCall (t);
    if (neb()) {
      StandardFce (t);
      if (neb()) {
        KonstantniCinitel (t);
        if (pkk()) {
          if (t[0]=='I') {
            ki = SemPop();
            if (ki > 0xFFFF) ErrSynt ("Konstanta presahujici rozsah typu");
            sprintf (u,"#I%d ", ki);
          }
          if (t[0]=='R') {
            sprintf (u,"#R%f ",RSemPop());
          }
          StrPush (u);
        }
        if (neb()) {
          VarReference (t);
          if (pkk()) {
            sprintf (u,"@%s ",t);
            StrAdd (u);
          }
          if (neb()) {
            KlSlovo ("(");
            if (zrtz()) {
              Vyraz (t);
              if (zrtz()) {
                NKlSlovo (")");
              }
            }
          }
        }
      }
    }
  }
  vse();
  if (pkk()) {
    strcpy (typ,t);
    //-printf ("JPrvek:%s\n", typ);
  }
}

void Prvek (char * typ) {

  int  a;
  a=0;

  je();
  {
    JPrvek (typ);
    if (zrtz()) {
      _sled();
      do {
        KlSlovo ("'");
        if (pkk()) {
          a^=1;
        }
      } while (sled_());
      if (pkk()) {
        if (a) {
          NotOper (typ,"<NOT>");
        }
      }
    }
  }
  vse();
  //-printf ("Prvek:%s\n", typ);
}

static void mult (char * typ) {
  char tl[ID_DEL],tn[ID_DEL];
  memset (tl, 0, ID_DEL);
  memset (tn, 0, ID_DEL);

  je();
  {
    Prvek (tl);
    if (zrtz()) {
      _sled();
      do {
        KlSlovo ("*");
        if (zrtz()) {
          Prvek (tn);
          if (pkk()) {
            MathOper (tl,tn,"*");
          }
        }
        if (neb()) {
          KlSlovo ("/");
          if (zrtz()) {
            Prvek (tn);
            if (pkk()) {
              MathOper (tl,tn,"/");
            }
          }
          if (neb()) {
            KlSlovo ("&");
            if (zrtz()) {
              Prvek (tn);
              if (pkk()) {
                WordOper (tl,tn,"&");
              }
            }
          }
        }
      } while (sled_());
    }
  }
  vse();
  if (pkk()) {
    strcpy (typ,tl);
    //-printf ("mult:%s\n", typ);
  }
}

void JednoduchyVyraz (char * typ) {
  char tl[ID_DEL],tn[ID_DEL];
  memset (tl, 0, ID_DEL);
  memset (tn, 0, ID_DEL);

  je();
  {
    je();
    {
      mult (tl);
      if (neb()) {
        KlSlovo ("-");
        if (zrtz()) {
          mult (tl);
          if (pkk()) {
            ChsOper (tl,"<CHS>");
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
          mult (tn);
          if (pkk()) {
            MathOper (tl,tn,"+");
          }
        }
        if (neb()) {
          KlSlovo ("-");
          if (zrtz()) {
            mult (tn);
            if (pkk()) {
              MathOper (tl,tn,"-");
            }
          }
          if (neb()) {
            KlSlovo ("|");
            if (zrtz()) {
              mult (tn);
              if (pkk()) {
                WordOper (tl,tn,"|");
              }
            }
            if (neb()) {
              KlSlovo ("^");
              if (zrtz()) {
                mult (tn);
                if (pkk()) {
                  WordOper (tl,tn,"WXOR");
                }
              }
            }
          }
        }
      } while (sled_());
    }
  }
  vse();
  if (pkk()) {
    strcpy (typ,tl);
    //-printf ("JednoduchyVyraz:%s\n", typ);
  }
}

static void Relace (char * typ) {
  char tl[ID_DEL],tn[ID_DEL],o[3];
  memset (tl, 0, ID_DEL);
  memset (tn, 0, ID_DEL);

  je();
  {
    JednoduchyVyraz (tl);
    if (zrtz()) {
      je();
      {
        je();
        {
          KlSlovo ("<>");
          if (pkk()) {
            strcpy (o,"<>");
          }
          if (neb()) {
            KlSlovo ("<=");
            if (pkk()) {
              strcpy (o,"<=");
            }
            if (neb()) {
              KlSlovo ("=<");
              if (pkk()) {
                strcpy (o,"<=");
              }
              if (neb()) {
                KlSlovo (">=");
                if (pkk()) {
                  strcpy (o,">=");
                }
                if (neb()) {
                  KlSlovo ("=>");
                  if (pkk()) {
                    strcpy (o,">=");
                  }
                  if (neb()) {
                    KlSlovo ("<");
                    if (pkk()) {
                      strcpy (o,"<");
                    }
                    if (neb()) {
                      KlSlovo (">");
                      if (pkk()) {
                        strcpy (o,">");
                      }
                      if (neb()) {
                        KlSlovo ("=");
                        if (pkk()) {
                          strcpy (o,"=");
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
        if (zrtz()) {
          JednoduchyVyraz (tn);
          if (pkk()) {
            MathOper (tl,tn,o);
            strcpy (typ,"BIT");
          }
        }
        if (neb()) {
          nic();
          strcpy (typ,tl);
        }
      }
      vse();
    }
  }
  vse();
}

static void Lmult (char * typ) {
  char tl[ID_DEL],tn[ID_DEL];
  memset (tl, 0, ID_DEL);
  memset (tn, 0, ID_DEL);

  je();
  {
    Relace (tl);
    if (zrtz()) {
      _sled();
      do {
        KlSlovo ("AND");
        if (pkk()) {
          StrAdd ("<PUSHL>");
        }
        if (zrtz()) {
          Relace (tn);
          if (pkk()) {
            LogOper (tl,tn,"<AND>");
          }
        }
      } while (sled_());
    }
  }
  vse();
  if (pkk()) {
    strcpy (typ,tl);
    //-printf ("Lmult:%s\n", typ);
  }
}

void Vyraz (char * typ) {
  char tl[ID_DEL],tn[ID_DEL];
  memset (tl, 0, ID_DEL);
  memset (tn, 0, ID_DEL);
  je();
  {
    Lmult (tl);
    if (zrtz()) {
      _sled();
      do {
        KlSlovo ("OR");
        if (pkk()) {
          StrAdd ("<PUSHL>");
        }
        if (zrtz()) {
          Lmult (tn);
          if (pkk()) {
            LogOper (tl,tn,"<OR>");
          }
        }
        if (neb()) {
          KlSlovo ("XOR");
          if (pkk()) {
            StrAdd ("<PUSHL>");
          }
          if (zrtz()) {
            Lmult (tn);
            if (pkk()) {
              LogOper (tl,tn,"<XOR>");
            }
          }
        }
      } while (sled_());
    }
  }
  vse();
  if (pkk()) {
    strcpy (typ,tl);
    //-printf ("Vyraz:%s\n", typ);
  }
}

/*---------------------------------------------------------------------------*/
/*   Vyraz                                                                   */
/*---------------------------------------------------------------------------*/

void VyrazTypu (const char * tn) {
  char tl[ID_DEL];
  memset (tl, 0, ID_DEL);

  je();
  {
    Vyraz (tl);
    if (pkk()) {
      ConvType (tn,tl);
    }
  }
  vse();
  //-printf ("VyrazTypu:%s\n", tn);
}

