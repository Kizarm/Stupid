/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   Vypis symbolu do souboru *.sym                                          */
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

extern FILE * of,*sf;
#define  UVOZ     (0x22)

/*---------------------------------------------------------------------------*/
/*    Ladici prostredky                                                      */
/*---------------------------------------------------------------------------*/

static void ZobrTyp (POLOZKA * u,char * t) {
  switch (u->typ) {
    case I_ntyp   :
      strcpy (t,"ntyp");
      break;
    case I_type   :
      strcpy (t,"type");
      break;
    case I_cons   :
      strcpy (t,"cons");
      break;
    case I_var    :
      strcpy (t," var");
      break;
    case I_mac    :
      strcpy (t," mac");
      break;
    default       :
      strcpy (t,".?!.");
      break;
  }
}
static void ZobrPodtyp (POLOZKA * u,char * t) {
  switch (u->podtyp) {
    case T_subr   :
      strcpy (t,"subr");
      break;
    case T_enum   :
      strcpy (t,"enum");
      break;
    case T_ord    :
      strcpy (t," ord");
      break;
    case T_real   :
      strcpy (t,"real");
      break;
    case T_str    :
      strcpy (t," str");
      break;
    case T_arr    :
      strcpy (t," arr");
      break;
    case T_rec    :
      strcpy (t," rec");
      break;
    case T_set    :
      strcpy (t," set");
      break;
    case T_ptr    :
      strcpy (t," ptr");
      break;
    case T_proc   :
      strcpy (t,"proc");
      break;
    case T_fce    :
      strcpy (t," fce");
      break;
    case T_mac    :
      strcpy (t," mac");
      break;
    default       :
      strcpy (t,".?!.");
      break;
  }
}
/*
static void ZobrTridu (POLOZKA * u,char * t) {
  switch (u->trida) {
    case C_undef  :
      strcpy (t,"undf");
      break;
    case C_ext    :
      strcpy (t," ext");
      break;
    case C_stat   :
      strcpy (t,"stat");
      break;
    case C_auto   :
      strcpy (t,"auto");
      break;
    default       :
      strcpy (t,".?!.");
      break;
  }
}
*/
extern int BitFlg;
static void ZobrAtr (POLOZKA * u,char * t) {
  unsigned long a;
  char          s[5];
  strcpy (t,"");
  if ( (u->typ) ==I_var) {
    UrciSize (u->dtyp);
    a=u->atr;
    if (BitFlg) {
      sprintf (t,"%4lX",a>>3);
      sprintf (s,".%ldH",a&7);
      strcat (t,s);
      return;
    } else      {
      sprintf (t,"%4lXH"   ,a>>3);
      return;
    }
  }
  if ( (u->typ) ==I_cons) {
    switch (u->podtyp) {
      case T_subr   :
        sprintf (t,"%ld",u->atr);
        break;
      case T_enum   :
        sprintf (t,"%ld",u->atr);
        break;
      case T_ord    :
        sprintf (t,"%ld",u->atr);
        break;
      case T_real   :
        sprintf (t,"%12.6g",u->fl);
        break;
      case T_str    :
        break;
      case T_arr    :
        break;
      case T_rec    :
        break;
      case T_set    :
        break;
      case T_ptr    :
        break;
      case T_proc   :
        sprintf (t,"%ld",u->atr);
        break;
      case T_fce    :
        sprintf (t,"%ld",u->atr);
        break;
      case T_mac    :
        break;
      default       :
        strcpy (t,".?!.");
        break;
    }
    return;
  }
}

static void Vypis (POLOZKA * u) {
  //  int i;
  char s[20], v[10]/*,t[20]*/,atr[20];
  ZobrTyp (u,s);
  ZobrPodtyp (u,v);
  ZobrAtr (u,atr);
  fprintf (sf,"%20s  ",u->iden);
  fprintf (sf,"%4s  %4s  %12s  %s",s,v,atr,u->dtyp);
  fprintf (sf,"\n");
}


void SeznamList (FILE * sf,POLOZKA * ls) {
  POLOZKA * u;
  u=Last;
  fprintf (sf,"\n\n");
  fprintf (sf,"%20s  ","identifikator");
  fprintf (sf,"%4s  %4s  %12s  %s","typ","    ","hodnota","popis");
  fprintf (sf,"\n--------------------------------------------------------------------------\n");

  while (u!=ls) {
    Vypis (u);
    u=u->prev;
  }
}

/*============================================================================*/

static void VypisArr (FILE * sf,POLOZKA * u) {
//   int  min,max;
  char s[40];

  StringInit (u->dtyp);
  je();
  {
    KlSlovo ("[");
    if (pkk()) {
      fprintf (sf,"[");
    }
    if (zrtz()) {
      OrdinalTypeId (s);
      if (pkk()) {
        fprintf (sf,"%ld",sez_uk->atr-1);
      }
      if (zrtz()) {
        _sled();
        do {
          KlSlovo ("][");
          if (pkk()) {
            fprintf (sf,",");
          }
          if (zrtz()) {
            OrdinalTypeId (s);
            if (pkk()) {
              fprintf (sf,"%ld",sez_uk->atr-1);
            }
          }
        } while (sled_());
        if (zrtz()) {
          KlSlovo ("]");
          if (pkk()) {
            fprintf (sf,"] ");
          }
          if (zrtz()) {
            TypeId (s);
            if (pkk()) {
              fprintf (sf,"%s\n",s);
            }
          }
        }
      }
    }
  }
  vse();

  if (!uspech) {
    AddInfo ("\n:%s:%d",u->iden,u->podtyp);
    ErrFatal ("Internal error 'VypisArr'");
  }
  StringEnd();
}

static void VarVypis (FILE * sf,POLOZKA * u) {
  unsigned long a;

  if ( (u->typ) ==I_var) {
    a=u->atr;
    UrciSize (u->dtyp);
    if ( (u->podtyp) ==T_arr) {
      fprintf (sf,"#ARR ");
      if (BitFlg) {
        fprintf (sf,"%5ld"  , a>>3);
        fprintf (sf,".%ld " , a&7);
      } else      {
        fprintf (sf,"%5ld   " ,a>>3);
      }
      fprintf (sf,"%s  \t" ,u->iden);
      VypisArr (sf,u);
    } else {
      fprintf (sf,"#VAR ");
      if (BitFlg) {
        fprintf (sf,"%5ld"  , a>>3);
        fprintf (sf,".%ld " , a&7);
      } else      {
        fprintf (sf,"%5ld   " ,a>>3);
      }
      fprintf (sf,"%s  \t",u->iden);
      fprintf (sf,"%s\n",u->dtyp);
    }
  }
}

void VarsVypis (FILE * sf,POLOZKA * ls) {
  POLOZKA * u;
  u=Last;

  fprintf (sf,"\n\n");
  while (u!=ls) {
    VarVypis (sf,u);
    u=u->prev;
  }
}

/*============================================================================*/

void CSourceGen() {
  POLOZKA * u;
  char t[30];
  u=Last;
  do {
    fprintf (sf,"\n\t{");
    sprintf (t ,"%c%s%c",UVOZ,u->iden,UVOZ);
    fprintf (sf,"%15s,",t);
    fprintf (sf,"%2d,",u->typ);
    fprintf (sf,"%2d,",u->podtyp);
    sprintf (t ,"%c%s%c",UVOZ,u->dtyp,UVOZ);
    fprintf (sf,"%12s,",t);
    fprintf (sf,"%2ld,",u->size);
    fprintf (sf,"%2d,",u->hloubka);
    fprintf (sf,"%2d,",u->trida);
    fprintf (sf,"0x%05lX,",u->atr);
    fprintf (sf,"0.,0,0,0,0},");
    u=u->prev;
  } while (u!=NULL);
}
