/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
//-#include "stdafx.h"
#include "Compiler.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/procesor.h"
#include "inc/fce.h"
#include "inc/chyby.h"
#include "inc/CfgTypes.h"

static int SetTab[N_last]= {

  /*  N_int   N_word  N_wori  N_bool  N_boli  N_real */
  /*-------------------------------------------------*/
  -1    , -1    , -1    , N_bool, -1    , -1
};

extern void blok();
extern long int RamBegin;
extern int  VarSaved;



/*---------------------------------------------------------------------------*/

static void Oddel() {
  je();
  {
    KlSlovo (";");
    if (neb()) {
      nic();
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Direktivy                                                               */
/*---------------------------------------------------------------------------*/


static void Pragma() {
  je();
  {
    KlSlovo ("#PRAGMA");
    if (zrtz()) {
      je();
      {
        KlSlovo ("$LJ");
        if (pkk()) {
          BaseProcessorPtr->c_PragmaLj();
        }
        if (neb()) {
          KlSlovo ("$SJ");
          if (pkk()) {
            BaseProcessorPtr->c_PragmaSj();
          }
        }
      }
      vse();
      JinakCh ("Nezname pragma");
      if (zrtz()) {
        Oddel();
      }
    }
  }
  vse();
}

static void DataBegin() {
  je();
  {
    KlSlovo ("#DATABEGIN");
    if (zrtz()) {
      if (VarSaved) {
        ErrSynt ("Direktiva DataBegin musi byt uvedena pred deklaraci promenne");
      }
      KladneCislo();
      JinakCh ("Parametrem direktivy DataBegin musi byt kladne cislo");
      if (pkk()) {
        RamBegin=SemPop() <<3;
        RamPtr=RamBegin;
      }
    }
    if (zrtz()) {
      Oddel();
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Prikazy                                                                 */
/*---------------------------------------------------------------------------*/

static void Prirazeni (char * t) {
  char tn[ID_DEL],s[ID_DEL + 16];
  int  a;
  int  poz;
  a=1;

  je();
  {
    poz = ZnakLine();
    VarReference (tn);
    if (zrtz()) {
      _sled();
      do {
        KlSlovo ("'");
        if (pkk()) {
          a^=1;
        }
      } while (sled_());
      if (zrtz()) {
        je();
        {
          KlSlovo ("=");
          if (pkk()) {
            poz = ZnakLine();
          }
          if (zrtz()) {
            VyrazTypu (tn);
            if (pkk()) {
              WriteMyLine2MF (poz);
              if (!a) {
                NotOper (tn,"<NOT>");
              }
              sprintf (s,"<R>%s",tn);
              StrAdd (s);
              SwapOper();
              CopyOper (tn,":=");
            }
            JinakCh ("Chyba ve vyrazu");
          }
          if (neb()) {
            nic();
            if (a) {
              OneOper (tn,"<SET>",SetTab);
            } else {
              OneOper (tn,"<RES>",SetTab);
            }
          }
        }
        vse();
      }
      WriteMyLine2MF (poz);
    }
  }
  (void) tn;
  vse();
  if (pkk()) {
    strcpy (t,tn);
  }
}

/*---------------------------------------------------------------------------*/
#define  UVOZ     (0x22)

static void Displej() {
  char tn[RET_DEL],s[ID_DEL + 16];

  je();
  {
    KlSlovo ("DISPLAY");
    if (zrtz()) {
      WriteLine2MF (0);
      NKlSlovo ("=");
      if (zrtz()) {
        je();
        {
          Vyraz (tn);
          if (pkk()) {
            sprintf (s,"<DIS>%s",tn);
            StrAdd (s);
          }
          if (neb()) {
            Retezec (tn);
            if (pkk()) {
              sprintf (s,"<D.STR>%c%s%c",UVOZ,tn,UVOZ);
              StrPush (s);
            }
          }
        }
        vse();
        JinakCh ("Za 'DISPLAY=' ocekavan retezec nebo vyraz");
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
static void Vytisk() {
  char tn[RET_DEL],s[ID_DEL + 16];

  je();
  {
    KlSlovo ("PRINT");
    if (zrtz()) {
      WriteLine2MF (0);
      NKlSlovo ("=");
      if (zrtz()) {
        je();
        {
          Vyraz (tn);
          if (pkk()) {
            sprintf (s,"<PRN>%s",tn);
            StrAdd (s);
          }
          if (neb()) {
            Retezec (tn);
            if (pkk()) {
              sprintf (s,"<P.STR>%c%s%c",UVOZ,tn,UVOZ);
              StrPush (s);
            }
          }
        }
        vse();
        JinakCh ("Za 'PRINT=' ocekavan retezec nebo vyraz");
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
static void Inverze() {
  char t[ID_DEL];

  je();
  {
    KlSlovo ("!");
    if (zrtz()) {
      WriteLine2MF (0);
      VarReference (t);
      JinakCh ("Za '!' ocekavana promenna");
      if (zrtz()) {
        _sled();
        do {
          KlSlovo ("'");
        } while (sled_());
      }
    }
  }
  vse();
  if (pkk()) {
    OneOper (t,"<INV>",SetTab);
  }
}

extern void WordOper (char * tl, char * tn, const char * o);

static void StandardProc() {
  char tn[RET_DEL],s[ID_DEL],t[ID_DEL];

  je();
  {
    KlSlovo ("MEMWRITE");
    if (zrtz()) {
      WriteLine2MF (0);
      KlSlovo ("(");
      if (zrtz()) {
        Vyraz (s);
        if (pkk()) {
          KlSlovo (",");
          if (zrtz()) {
            Vyraz (t);
            if (pkk()) {
              WordOper (s,t,"<MEMWRITE>");
              if (zrtz()) {
                KlSlovo (")");
              }
            }
          }
        }
      }
      /*      NKlSlovo("(");
            if(zrtz())
            {
              je();
              {
                Vyraz(tn);
                if(pkk())
                {
                  NKlSlovo(")");
                  if (zrtz())
                  {
                    sprintf(s,"<MEMWRITE>%s",tn);
                    StrAdd(s);
                  }
                }
              }vse();
            }
      */      JinakCh ("Za 'MemWrite()' ocekavany celociselne operandy");
    }
  }
  vse();
  (void) tn;
}
/*---------------------------------------------------------------------------*/
/*   JednoduchyPrikaz                                                        */
/*---------------------------------------------------------------------------*/
static void JedP() {
  char t[ID_DEL];
  je();
  {
    Prirazeni (t);
    if (neb()) {
      Inverze();
      if (neb()) {
        ProcedureCall();
        if (neb()) {
          StandardProc();
          if (neb()) {
            Displej();
            if (neb()) {
              Vytisk();
            }
          }
        }
      }
    }
  }
  vse();
}

void JednoduchyPrikaz() {
//   char t[ID_DEL];
  int poz;

  je();
  {
    je();
    {
      poz = ZnakLine();
      KlSlovo ("(");
      if (zrtz()) {
        WriteMyLine2MF (poz);
        JednoduchyPrikaz();
        if (zrtz()) {
          KlSlovo (")");
        }
      }
      if (neb()) {
        JedP();
      }
    }
    vse();
    if (zrtz()) {
      Oddel();
    }
  }
  vse();
}

void podminka() {
  VyrazTypu ("BIT");
  JinakCh ("Za 'IF' ocekavan logicky vyraz");
  vyhodnot();
}

/*---------------------------------------------------------------------------*/
/*   Struktura IF THEN ELSE                                                  */
/*---------------------------------------------------------------------------*/

static void elsify() {
  int  poz;

  je();
  {
    poz = ZnakLine();
    KlSlovo ("ELSEIF");
    if (pkk()) {
      WriteMyLine2MF (poz);
      BaseProcessorPtr->c_else();
    }
    if (zrtz()) {
      podminka();
      if (zrtz()) {
        poz = ZnakLine();
        KlSlovo ("THEN");
        JinakCh ("Chyba v logickem vyrazu nebo chybi 'THEN'");
        WriteMyLine2MF (poz);
        BaseProcessorPtr->c_if();
        if (zrtz()) {
          blok();
          if (zrtz()) {
            elsify();
            BaseProcessorPtr->c_endif();
          }
        }
      }
    }
    if (neb()) {
      je();
      {
        poz = ZnakLine();
        KlSlovo ("ELSE");
        if (pkk()) {
          WriteMyLine2MF (poz);
          BaseProcessorPtr->c_else();
        }
        if (zrtz()) {
          blok();
        }
        if (neb()) {
          nic();
        }
      }
      vse();
      if (zrtz()) {
        poz = ZnakLine();
        KlSlovo ("ENDIF");
        JinakCh ("Chyba v prikazu nebo chybi 'ENDIF'");
        WriteMyLine2MF (poz);
        BaseProcessorPtr->c_endif();
      }
    }
  }
  vse();
}

void IfThenElse() {
  int poz;

  je();
  {
    KlSlovo ("IF");
    if (zrtz()) {
      WriteLine2MF (0);
      podminka();
      if (zrtz()) {
        poz = ZnakLine();
        KlSlovo ("THEN");
        JinakCh ("Chyba v logickem vyrazu nebo chybi 'THEN'");
        WriteMyLine2MF (poz);
        BaseProcessorPtr->c_if();
        if (zrtz()) {
          blok();
          if (zrtz()) {
            elsify();
          }
        }
      }
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/
/*   Struktura CASE                                                          */
/*---------------------------------------------------------------------------*/
static void IntKonstantniV() {
  char t1[30];

  KonstantniVyraz (t1);
  if ( (!uspech) || (t1[0]!='I')) {
    ErrSynt ("Ocekavan konstantni vyraz typu INT");
  }
}

static void caseofy() {
  je();
  {
    WriteLine2MF (0);
    KlSlovo ("OF");
    if (pkk()) {
      BaseProcessorPtr->c_else();
    }
    if (zrtz()) {
      IntKonstantniV();
      if (zrtz()) {
        WriteLine2MF (0);
        KlSlovo ("THEN");
        JinakCh ("Chyba v konstantnim vyrazu nebo chybi 'THEN'");
        BaseProcessorPtr->c_caseof();
        if (zrtz()) {
          blok();
          if (zrtz()) {
            caseofy();
            BaseProcessorPtr->c_endif();
          }
        }
      }
    }
    if (neb()) {
      je();
      {
        KlSlovo ("ELSE");
        if (pkk()) {
          WriteLine2MF (0);
          BaseProcessorPtr->c_else();
        }
        if (zrtz()) {
          blok();
        }
        if (neb()) {
          nic();
        }
      }
      vse();
      if (zrtz()) {
        KlSlovo ("ENDCASE");
        WriteLine2MF (0);
        JinakCh ("Chyba v prikazu nebo chybi 'ENDCASE'");
        BaseProcessorPtr->c_endif();
      }
    }
  }
  vse();
}

void CaseOf() {
  je();
  {
    KlSlovo ("CASE");
    if (pkk()) WriteLine2MF (0);
    if (zrtz()) {
      VyrazTypu ("INT");
      JinakCh ("Ocekavan vyraz");
      vyhodnot();
      if (zrtz()) {
        KlSlovo ("OF");
        JinakCh ("Chyba ve vyrazu nebo chybi 'OF'");
        if (zrtz()) {
          IntKonstantniV();
          if (zrtz()) {
            KlSlovo ("THEN");
            WriteLine2MF (0);
            JinakCh ("Chyba v konstantnim vyrazu nebo chybi 'THEN'");
            BaseProcessorPtr->c_caseof();
            if (zrtz()) {
              blok();
              if (zrtz()) {
                caseofy();
                BaseProcessorPtr->c_endcase();
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
/*   Prikaz                                                                  */
/*---------------------------------------------------------------------------*/

void Prikaz() {
  char temp = deklarace;
  deklarace = 0;
  je();
  {
    JednoduchyPrikaz();
    if (pkk()) {
      vyhodnot();
      deklarace = 0;
    }
    if (neb()) {
      deklarace = 0;
      IfThenElse();
      if (neb()) {
        deklarace = 0;
        CaseOf();
        if (neb()) {
          deklarace = 0;
          Pragma();
          if (neb()) {
            deklarace = 0;
            DataBegin();
          }
        }
      }
    }
  }
  vse();
  deklarace = temp;
}


void blok() {
//  deklarace = 0;
  _sled();
  do {
    Prikaz();
  } while (sled_());
//   deklarace = ~0;
}

/*---------------------------------------------------------------------------*/
/*   NetAdr                                                                  */
/*---------------------------------------------------------------------------*/

static void NetAdres() {
  je();
  {
    KlSlovo ("NETADDR");
    if (zrtz()) {
      NKlSlovo ("(");
      if (zrtz()) {
        KladneCislo();
        if (zrtz()) {
          NKlSlovo (")");
          if (zrtz()) {
            Oddel();
          }
        }
      }
    }
  }
  vse();
  if (pkk()) {
    NetAdr=SemPop();
  }
}

/*---------------------------------------------------------------------------*/
/*   SystemDefinition()                                                      */
/*---------------------------------------------------------------------------*/

void SystemDefinition() {
  mezery();
  _sled();
  do {
    DeklaraceVar (&RamPtr);
    if (neb()) {
      DeklaraceKonst();
      if (neb()) {
        DeklaraceMacro();
      }
    }
  } while (sled_());
}


/*---------------------------------------------------------------------------*/
/*   Program                                                                 */
/*---------------------------------------------------------------------------*/

static void FileInclude() {
  char fn[RET_DEL];
  je();
  {
    KlSlovo ("#INCLUDE");
    if (zrtz()) {
      Retezec (fn);
    }
  }
  vse();
  if (pkk()) {
    FileInit (fn);
    mezery();
  }
}

void SetConfiguration (char * name) {
  char typ[255], *ptr = name, *typ_ptr = typ;
  char msg[255 + 256];

  while (*ptr != 0) {
    if (*ptr == ' ' || *ptr == '-' || *ptr == '_') {
      *typ_ptr = 0;
      while (*ptr == ' ' || *ptr == '-' || *ptr == '_')
        ptr++;
      break;
    } else
      *typ_ptr++ = *ptr++;
  }
  *typ_ptr = 0;
  if (!SetConfig (typ)) {
    sprintf (msg, "konfigurace \'%s\' nenalezena", typ);
    ErrSynt (msg);
  }

  while (*ptr != 0) {
    if (isalpha (*ptr)) {
      typ[0] = *ptr;
      typ[1] = 0;
      if (!SetConfig (typ)) {
        sprintf (msg, "konfigurace \'%s\' nenalezena", typ);
        Warning (msg);
      }
    }
    ++ptr;
  }

}
bool config_error = false;

void Configuration() {
  char name[255];

  je();
  {
    KlSlovo ("#CONFIGURATION");
    config_error = true;
    JinakCh ("Na zacatku programu musi byt uvedena direktiva \"#Configuration\"");
    config_error = false;
    if (zrtz()) {
      Retezec (name);
      JinakCh ("Parametrem direktivy DataBegin musi byt retezec");
      if (pkk()) {
        SetConfiguration (name);
      }
    }
    if (zrtz()) {
      Oddel();
    }
  }
  vse();
}

void Program() {
  je();
  {
    mezery();
    if (zrtz()) {

      Configuration();
      BaseProcessorPtr->c_procinit();

      _sled();
      do {
        DeklaraceVar (&RamPtr);
        if (neb()) {
          DeklaraceKonst();
          if (neb()) {
            DeklaraceMacro();
            if (neb()) {
              ProcedureDef();
              if (neb()) {
                FunctionDef();
                if (neb()) {
                  Prikaz();
                  if (neb()) {
                    NetAdres();
                    if (neb()) {
                      FileInclude();
                    }
                  }
                }
              }
            }
          }
        }
      } while (sled_());
      if (zrtz()) {
        KlSlovo ("END");
        WriteLine2MF (0);
        JinakCh ("Neznamy prikaz, nebo chybi 'END'");
      }
    }
  }
  vse();
  BaseProcessorPtr->c_procend();
}
/*---------------------------------------------------------------------------*/
