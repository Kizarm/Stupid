/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   Rozeznavani typu                                                        */
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

static int  TypeCnt;
extern void Type (char * t);
extern int VarSaved; // indikuje pouziti funkce SaveVars

/*---------------------------------------------------------------------------*/

static void SaveId (LPOLOZKA * p) {
  SemPush ( (ATRIBUT) SezUlozId (p));
}
static void HiddenType (LPOLOZKA * p) {
  sprintf (p->iden,"_%d",TypeCnt++);
}

/*---------------------------------------------------------------------------*/

void IdList (const char * pref) {
  char  u[ID_DEL];
  LPOLOZKA p= {"",0,0,"",0L,0,0,0L,0,0,0,0};

  je();
  {
    NovyId (u);
    if (pkk()) {
      strcpy (p.iden,pref);
      strcat (p.iden,u);
      SaveId (&p);
    }
    if (zrtz()) {
      _sled();
      do {
        KlSlovo (",");
        if (zrtz()) {
          NovyId (u);
          if (pkk()) {
            strcpy (p.iden,pref);
            strcat (p.iden,u);
            SaveId (&p);
          }
        }
      } while (sled_());
    }
  }
  vse();
}

/*---------------------------------------------------------------------------*/

void OznacujeTyp() {
  switch (sez_uk->typ) {
    case I_ntyp   :
    case I_type   :
      Pravda();
      break;
    case I_cons   :
    case I_mac    :
    case I_var    :
      Nepravda();
      break;

    default       :
      AddInfo ("\n:%s:%d",sez_uk->iden,sez_uk->typ);
      ErrFatal ("Internal error 'Oznacuje Typ'");
  }
}

/*---------------------------------------------------------------------------*/

void TypeId (char * t) {
  char u[ID_DEL];

  je();
  {
    IdentVSeznamu (u);
    if (zrtz()) {
      OznacujeTyp();
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}

/*---------------------------------------------------------------------------*/

void OrdinalTypeId (char * t) {
  char u[ID_DEL];

  je();
  {
    TypeId (u);
    if (zrtz()) {
      switch (sez_uk->podtyp) {
        case  T_subr :
        case  T_enum :
        case  T_ord  :
          Pravda();
          break;
        case  T_real :
        case  T_str  :
        case  T_arr  :
        case  T_rec  :
        case  T_set  :
        case  T_ptr  :
        case  T_proc :
        case  T_fce  :
          Nepravda();
          break;

        default      :
          AddInfo ("\n:%s:%d",sez_uk->iden,sez_uk->podtyp);
          ErrFatal ("Internal error 'OrdinalTypeId'");
      }
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}

/*===========================================================================*/
/*                                                                           */
/* ROZBOR VNITRNI REPREZENTACE TYPU                                          */
/*                                                                           */
/*===========================================================================*/

int UrciPtyp (char * t) {
  char u[ID_DEL];
  int p;

  StringInit (t);
  p=-1;
  je();
  {
    KlSlovo ("{");
    if (pkk()) {
      p=T_set;
    }
    if (neb()) {
      KlSlovo ("<");
      if (pkk()) {
        p=T_subr;
      }
      if (neb()) {
        KlSlovo ("[");
        if (pkk()) {
          p=T_arr;
        }
        if (neb()) {
          KlSlovo ("^");
          if (pkk()) {
            p=T_ptr;
          }
          if (neb()) {
            KlSlovo ("/");
            if (pkk()) {
              p=T_enum;
            }
            if (neb()) {
              KlSlovo ("'");
              if (pkk()) {
                p=T_str;
              }
              if (neb()) {
                KlSlovo ("%");
                if (pkk()) {
                  p=T_rec;
                }
                if (neb()) {
                  KlSlovo ("(");
                  if (pkk()) {
                    p=T_proc;
                  }
                  if (neb()) {
                    TypeId (u);
                    if (pkk()) {
                      p=sez_uk->podtyp;
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
  StringEnd();
  /*
     if(p==-2){ StringEnd(); ErrFatal("Internal error - Recursive definition"); }
  */
  if (p<  0) {
    AddInfo ("\n%s",t);
    ErrFatal ("Internal error 'UrciPtyp'");
  }
  return p;
}

/*---------------------------------------------------------------------------*/
/*
static int UrciAtr (char * t) {
  char u[ID_DEL];
  int p;

  StringInit (t);
  p=-1;
  je();
  {
    KlSlovo ("<");
    if (pkk()) {
      p=0;
    }
    if (neb()) {
      KlSlovo ("[");
      if (pkk()) {
        p=0;
      }
      if (neb()) {
        KlSlovo ("^");
        if (pkk()) {
          p=0;
        }
        if (neb()) {
          KlSlovo ("/");
          if (pkk()) {
            p=0;
          }
          if (neb()) {
            KlSlovo ("%");
            if (pkk()) {
              p=0;
            }
            if (neb()) {
              KlSlovo ("(");
              if (pkk()) {
                p=0;
              }
              if (neb()) {
                TypeId (u);
                if (pkk()) {
                  p=sez_uk->atr;
                }
              }
            }
          }
        }
      }
    }
  }
  vse();
  StringEnd();
  if (p<0) {
    AddInfo ("\n%s",t);
    ErrFatal ("Internal error 'UrciAtr'");
  }
  return p;
}
*/
/*---------------------------------------------------------------------------*/

static unsigned NIndex() {
  char u[ID_DEL];
  unsigned a/*,b*/;

  a=0;
  OrdinalTypeId (u);
  if (pkk()) {
    a=sez_uk->atr;
  }

  return a;
}

int BitFlg ;

static long UrciSizeA (char * t) {
  char u[ID_DEL];
  long i,j,p;

  p=0;
  je();
  {
    KlSlovo ("<");
    if (pkk()) {
      p=L_enum;
    }
    if (neb()) {
      KlSlovo ("[");
      if (zrtz()) {
        i=NIndex();
        if (zrtz()) {
          NKlSlovo ("]");
          if (zrtz()) {
            j=UrciSizeA (t);
            if (pkk()) {
              p=i*j;
            }
          }
        }
      }
      if (neb()) {
        KlSlovo ("^");
        if (pkk()) {
          p=L_ptr;
        }
        if (neb()) {
          KlSlovo ("@");
          if (zrtz()) {
            je();
            {
              KlSlovo ("BIT");
              if (pkk()) {
                p=L_bptr;
              }
              if (neb()) {
                KlSlovo ("IN_BIT");
                if (pkk()) {
                  p=L_bptr;
                }
                if (neb()) {
                  nic();
                  if (pkk()) {
                    p=L_ptr;
                  }
                }
              }
            }
            vse();
          }
          if (neb()) {
            KlSlovo ("(");
            if (pkk()) {
              p=L_ptr;
            }
            if (neb()) {
              TypeId (u);
              if (pkk()) {
                p=sez_uk->size;
                if (p<8) {
                  BitFlg=~0;
                }
              }
            }
          }
        }
      }
    }
  }
  vse();
  if (p==0) {
    ErrFatal ("Internal error 'UrciSizeA'");
  }
  return p;
}

long UrciSize (char * t) {
  long p;
  BitFlg=0;
  StringInit (t);
  p=UrciSizeA (t);
  StringEnd();
  if (p<0) {
    ErrFatal ("Internal error - Recursive definition 'UrciSize'");
  }
//   printf("UrciSize : %d\n",p);
  return p;
}

/*===========================================================================*/
/*                                                                           */
/* ORDINALNI TYPY                                                            */
/*                                                                           */
/*===========================================================================*/

static void SubrangeType (char * t) {
  LPOLOZKA q= {"",I_ntyp,T_subr,"<",L_enum,0,0,0L,0,0,0,0};
  char u[ID_DEL]/*,v[ID_DEL]*/;
  unsigned a,b;

  je();
  {
    KladneCislo();
    if (neb()) {
      IdentVSeznamu (u);
      if (zrtz()) {
        SezTypu (I_cons);
        if (pkk()) {
          if (strcmp (sez_uk->dtyp,"!INT")) ErrSynt ("Ocekavana INT konstanta");
          if (sez_uk->atr < 0)          ErrSynt ("Dimenze <0");
          strcpy (t, (sez_uk->dtyp) +1);
          SemPush (sez_uk->atr);
        }
      }
    }
  }
  vse();

  if (pkk()) {
    b= (unsigned) SemPop();
    a=0;
    sprintf (q.dtyp,"<%d..%d>%s",a,b,"WORD");
    HiddenType (&q);
    q.atr=b-a+1;
    SezUloz (&q);
    strcpy (t,q.iden);
  }
}

/*---------------------------------------------------------------------------*/

static void OrdinalType (char * t) {
  char u[ID_DEL];

  je();
  {
    OrdinalTypeId (u);
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}

/*===========================================================================*/
/*                                                                           */
/* REALNE TYPY                                                               */
/*                                                                           */
/*===========================================================================*/

static void RealType (char * t) {
  char u[ID_DEL];

  je();
  {
    TypeId (u);
    if (zrtz()) {
      SezPodtypu (T_real);
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}

/*===========================================================================*/
/*                                                                           */
/* JEDNODUCHE TYPY                                                           */
/*                                                                           */
/*===========================================================================*/

static void SimpleType (char * t) {
  char u[ID_DEL];

  je();
  {
    OrdinalType (u);
    if (neb()) {
      RealType (u);
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}


/*===========================================================================*/
/*                                                                           */
/* STRUKTUROVANE TYPY                                                        */
/*                                                                           */
/*===========================================================================*/

static void ArrayType (char * t) {
  char u1[ID_DEL],u2[ID_DEL];

  je();
  {
    KlSlovo ("ARRAY");
    if (zrtz()) {
      NKlSlovo ("[");
      if (pkk()) {
        strcpy (u2,"[");
      }
      if (zrtz()) {
        SubrangeType (u1);
        strcat (u2, u1);
        JinakCh ("Ocekavana dimenze");
        if (zrtz()) {
          _sled();
          do {
            KlSlovo (",");
            if (pkk()) {
              strcat (u2,"][");
            }
            if (zrtz()) {
              SubrangeType (u1);
              strcat (u2, u1);
              JinakCh ("Ocekavana dimenze");
            }
          } while (sled_());
          if (zrtz()) {
            NKlSlovo ("]");
            if (pkk()) {
              strcat (u2,"]");
            }
            if (zrtz()) {
              NKlSlovo ("OF");
              if (zrtz()) {
                TypeId (u1);
              }
            }
          }
        }
      }
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,u2);
    strcat (t,u1);
  }
}

/*---------------------------------------------------------------------------*/

static void StructuredType (char * t) {
  char u[ID_DEL];

  je();
  {
    ArrayType (u);
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}

/*===========================================================================*/
/*                                                                           */
/* VSECHNY TYPY                                                              */
/*                                                                           */
/*===========================================================================*/

void Type (char * t) {
  char u[ID_DEL];

  je();
  {
    SimpleType (u);
    if (neb()) {
      StructuredType (u);
      if (neb()) {
        TypeId (u);
      }
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,u);
  }
}

/*---------------------------------------------------------------------------*/
/*       identif   ,typ   , podtyp, dtyp ,size   ,h,trida  ,atr   ,fl        */
/*---------------------------------------------------------------------------*/

LPOLOZKA TypesTab[N_last]
= {
  {"INT"      ,I_type, T_ord ,""    ,L_int  ,0,C_undef,N_int ,0.,0,0,0,0},
  {"WORD"     ,I_type, T_ord ,""    ,L_word ,0,C_undef,N_word,0.,0,0,0,0},
  {"IN_WORD"  ,I_type, T_ord ,""    ,L_word ,0,C_undef,N_wori,0.,0,0,0,0},
  {"BIT"      ,I_type, T_ord ,""    ,L_bool ,0,C_undef,N_bool,0.,0,0,0,0},
  {"IN_BIT"   ,I_type, T_ord ,""    ,L_bool ,0,C_undef,N_boli,0.,0,0,0,0},
  {"REAL"     ,I_type, T_real,""    ,L_real ,0,C_undef,N_real,0.,0,0,0,0}
};

/*---------------------------------------------------------------------------*/
void ladeni (const char * type) {
  printf ("type=%s\n", type);
}

int TypeNum (const char * t) {
  int i;
  char s[ID_DEL];

  for (i=0; i<N_last; i++) {
    if (!strcmp (t,TypesTab[i].iden)) {
      return TypesTab[i].atr;
    }
  }
  switch (*t) {
    case '[':
      snprintf (s, ID_DEL,"Nedostatecny pocet indexu pole");
      break;
    case '(':
      snprintf (s, ID_DEL,"Chybi argumenty funkce nebo procedury");
      break;
    default :
      snprintf (s, ID_DEL,"Neni jednoduchy typ :%s",t);
  }
  ladeni (t);
  ErrSynt (s);
  return -1;
}

#define PREDEFNUM 167
void TypesInit() {
  int i;
  SPOLOZKA PredefTab[PREDEFNUM]
  = {
    {       "EELOAD", 3, 2,       "BIT", 1, 0, 2,0x010F7,0.,0,0,0,0},
    {       "EESAVE", 3, 2,       "BIT", 1, 0, 2,0x010F6,0.,0,0,0,0},
    {       "COUNT1", 3, 2,      "WORD",16, 0, 2,0x00A90,0.,0,0,0,0},
    {       "COUNT0", 3, 2,      "WORD",16, 0, 2,0x00A80,0.,0,0,0,0},
    {   "EEWR_COUNT", 3, 2,      "WORD",16, 0, 2,0x00A70,0.,0,0,0,0},
    {          "DB7", 3, 2,      "WORD",16, 0, 2,0x01130,0.,0,0,0,0},
    {          "DB6", 3, 2,      "WORD",16, 0, 2,0x01120,0.,0,0,0,0},
    {          "DB5", 3, 2,      "WORD",16, 0, 2,0x01110,0.,0,0,0,0},
    {          "DB4", 3, 2,      "WORD",16, 0, 2,0x01100,0.,0,0,0,0},
    {          "DB3", 3, 2,      "WORD",16, 0, 2,0x010F0,0.,0,0,0,0},
    {          "DB2", 3, 2,      "WORD",16, 0, 2,0x010E0,0.,0,0,0,0},
    {          "DB1", 3, 2,      "WORD",16, 0, 2,0x010D0,0.,0,0,0,0},
    {          "DB0", 3, 2,      "WORD",16, 0, 2,0x010C0,0.,0,0,0,0},
    {          "DM7", 3, 2,      "WORD",16, 0, 2,0x010B0,0.,0,0,0,0},
    {          "DM6", 3, 2,      "WORD",16, 0, 2,0x010A0,0.,0,0,0,0},
    {          "DM5", 3, 2,      "WORD",16, 0, 2,0x01090,0.,0,0,0,0},
    {          "DM4", 3, 2,      "WORD",16, 0, 2,0x01080,0.,0,0,0,0},
    {          "DM3", 3, 2,      "WORD",16, 0, 2,0x01070,0.,0,0,0,0},
    {          "DM2", 3, 2,      "WORD",16, 0, 2,0x01060,0.,0,0,0,0},
    {          "DM1", 3, 2,      "WORD",16, 0, 2,0x01050,0.,0,0,0,0},
    {          "DM0", 3, 2,      "WORD",16, 0, 2,0x01040,0.,0,0,0,0},
    {         "DY15", 3, 2,      "WORD",16, 0, 2,0x01EF0,0.,0,0,0,0},
    {         "DY14", 3, 2,      "WORD",16, 0, 2,0x01EE0,0.,0,0,0,0},
    {         "DY13", 3, 2,      "WORD",16, 0, 2,0x01ED0,0.,0,0,0,0},
    {         "DY12", 3, 2,      "WORD",16, 0, 2,0x01EC0,0.,0,0,0,0},
    {         "DY11", 3, 2,      "WORD",16, 0, 2,0x01EB0,0.,0,0,0,0},
    {         "DY10", 3, 2,      "WORD",16, 0, 2,0x01EA0,0.,0,0,0,0},
    {          "DY9", 3, 2,      "WORD",16, 0, 2,0x01E90,0.,0,0,0,0},
    {          "DY8", 3, 2,      "WORD",16, 0, 2,0x01E80,0.,0,0,0,0},
    {          "DY7", 3, 2,      "WORD",16, 0, 2,0x01E70,0.,0,0,0,0},
    {          "DY6", 3, 2,      "WORD",16, 0, 2,0x01E60,0.,0,0,0,0},
    {          "DY5", 3, 2,      "WORD",16, 0, 2,0x01E50,0.,0,0,0,0},
    {          "DY4", 3, 2,      "WORD",16, 0, 2,0x01E40,0.,0,0,0,0},
    {          "DY3", 3, 2,      "WORD",16, 0, 2,0x01E30,0.,0,0,0,0},
    {          "DY2", 3, 2,      "WORD",16, 0, 2,0x01E20,0.,0,0,0,0},
    {          "DY1", 3, 2,      "WORD",16, 0, 2,0x01030,0.,0,0,0,0},
    {          "DY0", 3, 2,      "WORD",16, 0, 2,0x01020,0.,0,0,0,0},
    {         "DX15", 3, 2,   "IN_WORD",16, 0, 2,0x01E10,0.,0,0,0,0},
    {         "DX14", 3, 2,   "IN_WORD",16, 0, 2,0x01E00,0.,0,0,0,0},
    {         "DX13", 3, 2,   "IN_WORD",16, 0, 2,0x01DF0,0.,0,0,0,0},
    {         "DX12", 3, 2,   "IN_WORD",16, 0, 2,0x01DE0,0.,0,0,0,0},
    {         "DX11", 3, 2,   "IN_WORD",16, 0, 2,0x01DD0,0.,0,0,0,0},
    {         "DX10", 3, 2,   "IN_WORD",16, 0, 2,0x01DC0,0.,0,0,0,0},
    {          "DX9", 3, 2,   "IN_WORD",16, 0, 2,0x01DB0,0.,0,0,0,0},
    {          "DX8", 3, 2,   "IN_WORD",16, 0, 2,0x01DA0,0.,0,0,0,0},
    {          "DX7", 3, 2,   "IN_WORD",16, 0, 2,0x01D90,0.,0,0,0,0},
    {          "DX6", 3, 2,   "IN_WORD",16, 0, 2,0x01D80,0.,0,0,0,0},
    {          "DX5", 3, 2,   "IN_WORD",16, 0, 2,0x01D70,0.,0,0,0,0},
    {          "DX4", 3, 2,   "IN_WORD",16, 0, 2,0x01D60,0.,0,0,0,0},
    {          "DX3", 3, 2,   "IN_WORD",16, 0, 2,0x01D50,0.,0,0,0,0},
    {          "DX2", 3, 2,   "IN_WORD",16, 0, 2,0x01D40,0.,0,0,0,0},
    {          "DX1", 3, 2,   "IN_WORD",16, 0, 2,0x01010,0.,0,0,0,0},
    {          "DX0", 3, 2,   "IN_WORD",16, 0, 2,0x01000,0.,0,0,0,0},
    {      "LOADGEN", 3, 2,       "BIT", 1, 0, 2,0x010F4,0.,0,0,0,0},
    {      "KBSOUND", 3, 2,       "BIT", 1, 0, 2,0x010F3,0.,0,0,0,0},
    {      "KBREPEN", 3, 2,       "BIT", 1, 0, 2,0x010F2,0.,0,0,0,0},
    {     "KBREPEAT", 3, 2,      "WORD",16, 0, 2,0x00A60,0.,0,0,0,0},
    {      "KBDELAY", 3, 2,      "WORD",16, 0, 2,0x00A50,0.,0,0,0,0},
    {       "KBCODE", 3, 2,      "WORD",16, 0, 2,0x00A40,0.,0,0,0,0},
    {       "FORMAT", 3, 2,      "WORD",16, 0, 2,0x00A30,0.,0,0,0,0},
    {     "POSITION", 3, 2,      "WORD",16, 0, 2,0x00A20,0.,0,0,0,0},
    {       "CLRSEC", 3, 2,       "BIT", 1, 0, 2,0x010F1,0.,0,0,0,0},
    {         "HOLD", 3, 2,       "BIT", 1, 0, 2,0x010F0,0.,0,0,0,0},
    {         "WEEK", 3, 2,      "WORD",16, 0, 2,0x008E0,0.,0,0,0,0},
    {         "YEAR", 3, 2,      "WORD",16, 0, 2,0x008D0,0.,0,0,0,0},
    {        "MONTH", 3, 2,      "WORD",16, 0, 2,0x008C0,0.,0,0,0,0},
    {          "DAY", 3, 2,      "WORD",16, 0, 2,0x008B0,0.,0,0,0,0},
    {         "HOUR", 3, 2,      "WORD",16, 0, 2,0x008A0,0.,0,0,0,0},
    {       "MINUTE", 3, 2,      "WORD",16, 0, 2,0x00890,0.,0,0,0,0},
    {       "SECOND", 3, 2,      "WORD",16, 0, 2,0x00880,0.,0,0,0,0},
    {      "FASTADC", 3, 2,       "BIT", 1, 0, 2,0x010F5,0.,0,0,0,0},
    {     "ADCMODE7", 3, 2,      "WORD",16, 0, 2,0x00A10,0.,0,0,0,0},
    {     "ADCMODE6", 3, 2,      "WORD",16, 0, 2,0x00A00,0.,0,0,0,0},
    {     "ADCMODE5", 3, 2,      "WORD",16, 0, 2,0x009F0,0.,0,0,0,0},
    {     "ADCMODE4", 3, 2,      "WORD",16, 0, 2,0x009E0,0.,0,0,0,0},
    {     "ADCMODE3", 3, 2,      "WORD",16, 0, 2,0x009D0,0.,0,0,0,0},
    {     "ADCMODE2", 3, 2,      "WORD",16, 0, 2,0x009C0,0.,0,0,0,0},
    {     "ADCMODE1", 3, 2,      "WORD",16, 0, 2,0x009B0,0.,0,0,0,0},
    {     "ADCMODE0", 3, 2,      "WORD",16, 0, 2,0x009A0,0.,0,0,0,0},
    {      "ADCMODE", 3, 2,      "WORD",16, 0, 2,0x009A0,0.,0,0,0,0},
    {      "OFFSET7", 3, 2,      "WORD",16, 0, 2,0x00A10,0.,0,0,0,0},
    {      "OFFSET6", 3, 2,      "WORD",16, 0, 2,0x00A00,0.,0,0,0,0},
    {      "OFFSET5", 3, 2,      "WORD",16, 0, 2,0x009F0,0.,0,0,0,0},
    {      "OFFSET4", 3, 2,      "WORD",16, 0, 2,0x009E0,0.,0,0,0,0},
    {      "OFFSET3", 3, 2,      "WORD",16, 0, 2,0x009D0,0.,0,0,0,0},
    {      "OFFSET2", 3, 2,      "WORD",16, 0, 2,0x009C0,0.,0,0,0,0},
    {      "OFFSET1", 3, 2,      "WORD",16, 0, 2,0x009B0,0.,0,0,0,0},
    {      "OFFSET0", 3, 2,      "WORD",16, 0, 2,0x009A0,0.,0,0,0,0},
    {       "CALIB7", 3, 2,      "WORD",16, 0, 2,0x00990,0.,0,0,0,0},
    {       "CALIB6", 3, 2,      "WORD",16, 0, 2,0x00980,0.,0,0,0,0},
    {       "CALIB5", 3, 2,      "WORD",16, 0, 2,0x00970,0.,0,0,0,0},
    {       "CALIB4", 3, 2,      "WORD",16, 0, 2,0x00960,0.,0,0,0,0},
    {       "CALIB3", 3, 2,      "WORD",16, 0, 2,0x00950,0.,0,0,0,0},
    {       "CALIB2", 3, 2,      "WORD",16, 0, 2,0x00940,0.,0,0,0,0},
    {       "CALIB1", 3, 2,      "WORD",16, 0, 2,0x00930,0.,0,0,0,0},
    {       "CALIB0", 3, 2,      "WORD",16, 0, 2,0x00920,0.,0,0,0,0},
    {         "CLK7", 3, 2,       "BIT", 1, 0, 2,0x010EF,0.,0,0,0,0},
    {         "TOF7", 3, 2,       "BIT", 1, 0, 2,0x010E7,0.,0,0,0,0},
    {         "TOE7", 3, 2,       "BIT", 1, 0, 2,0x010DF,0.,0,0,0,0},
    {         "TPA7", 3, 2,       "BIT", 1, 0, 2,0x010D7,0.,0,0,0,0},
    {         "TDM7", 3, 2,       "BIT", 1, 0, 2,0x010CF,0.,0,0,0,0},
    {         "TEN7", 3, 2,       "BIT", 1, 0, 2,0x010C7,0.,0,0,0,0},
    {           "T7", 3, 2,      "WORD",16, 0, 2,0x00870,0.,0,0,0,0},
    {         "CLK6", 3, 2,       "BIT", 1, 0, 2,0x010EE,0.,0,0,0,0},
    {         "TOF6", 3, 2,       "BIT", 1, 0, 2,0x010E6,0.,0,0,0,0},
    {         "TOE6", 3, 2,       "BIT", 1, 0, 2,0x010DE,0.,0,0,0,0},
    {         "TPA6", 3, 2,       "BIT", 1, 0, 2,0x010D6,0.,0,0,0,0},
    {         "TDM6", 3, 2,       "BIT", 1, 0, 2,0x010CE,0.,0,0,0,0},
    {         "TEN6", 3, 2,       "BIT", 1, 0, 2,0x010C6,0.,0,0,0,0},
    {           "T6", 3, 2,      "WORD",16, 0, 2,0x00860,0.,0,0,0,0},
    {         "CLK5", 3, 2,       "BIT", 1, 0, 2,0x010ED,0.,0,0,0,0},
    {         "TOF5", 3, 2,       "BIT", 1, 0, 2,0x010E5,0.,0,0,0,0},
    {         "TOE5", 3, 2,       "BIT", 1, 0, 2,0x010DD,0.,0,0,0,0},
    {         "TPA5", 3, 2,       "BIT", 1, 0, 2,0x010D5,0.,0,0,0,0},
    {         "TDM5", 3, 2,       "BIT", 1, 0, 2,0x010CD,0.,0,0,0,0},
    {         "TEN5", 3, 2,       "BIT", 1, 0, 2,0x010C5,0.,0,0,0,0},
    {           "T5", 3, 2,      "WORD",16, 0, 2,0x00850,0.,0,0,0,0},
    {         "CLK4", 3, 2,       "BIT", 1, 0, 2,0x010EC,0.,0,0,0,0},
    {         "TOF4", 3, 2,       "BIT", 1, 0, 2,0x010E4,0.,0,0,0,0},
    {         "TOE4", 3, 2,       "BIT", 1, 0, 2,0x010DC,0.,0,0,0,0},
    {         "TPA4", 3, 2,       "BIT", 1, 0, 2,0x010D4,0.,0,0,0,0},
    {         "TDM4", 3, 2,       "BIT", 1, 0, 2,0x010CC,0.,0,0,0,0},
    {         "TEN4", 3, 2,       "BIT", 1, 0, 2,0x010C4,0.,0,0,0,0},
    {           "T4", 3, 2,      "WORD",16, 0, 2,0x00840,0.,0,0,0,0},
    {         "CLK3", 3, 2,       "BIT", 1, 0, 2,0x010EB,0.,0,0,0,0},
    {         "TOF3", 3, 2,       "BIT", 1, 0, 2,0x010E3,0.,0,0,0,0},
    {         "TOE3", 3, 2,       "BIT", 1, 0, 2,0x010DB,0.,0,0,0,0},
    {         "TPA3", 3, 2,       "BIT", 1, 0, 2,0x010D3,0.,0,0,0,0},
    {         "TDM3", 3, 2,       "BIT", 1, 0, 2,0x010CB,0.,0,0,0,0},
    {         "TEN3", 3, 2,       "BIT", 1, 0, 2,0x010C3,0.,0,0,0,0},
    {           "T3", 3, 2,      "WORD",16, 0, 2,0x00830,0.,0,0,0,0},
    {         "CLK2", 3, 2,       "BIT", 1, 0, 2,0x010EA,0.,0,0,0,0},
    {         "TOF2", 3, 2,       "BIT", 1, 0, 2,0x010E2,0.,0,0,0,0},
    {         "TOE2", 3, 2,       "BIT", 1, 0, 2,0x010DA,0.,0,0,0,0},
    {         "TPA2", 3, 2,       "BIT", 1, 0, 2,0x010D2,0.,0,0,0,0},
    {         "TDM2", 3, 2,       "BIT", 1, 0, 2,0x010CA,0.,0,0,0,0},
    {         "TEN2", 3, 2,       "BIT", 1, 0, 2,0x010C2,0.,0,0,0,0},
    {           "T2", 3, 2,      "WORD",16, 0, 2,0x00820,0.,0,0,0,0},
    {         "CLK1", 3, 2,       "BIT", 1, 0, 2,0x010E9,0.,0,0,0,0},
    {         "TOF1", 3, 2,       "BIT", 1, 0, 2,0x010E1,0.,0,0,0,0},
    {         "TOE1", 3, 2,       "BIT", 1, 0, 2,0x010D9,0.,0,0,0,0},
    {         "TPA1", 3, 2,       "BIT", 1, 0, 2,0x010D1,0.,0,0,0,0},
    {         "TDM1", 3, 2,       "BIT", 1, 0, 2,0x010C9,0.,0,0,0,0},
    {         "TEN1", 3, 2,       "BIT", 1, 0, 2,0x010C1,0.,0,0,0,0},
    {           "T1", 3, 2,      "WORD",16, 0, 2,0x00810,0.,0,0,0,0},
    {         "CLK0", 3, 2,       "BIT", 1, 0, 2,0x010E8,0.,0,0,0,0},
    {         "TOF0", 3, 2,       "BIT", 1, 0, 2,0x010E0,0.,0,0,0,0},
    {         "TOE0", 3, 2,       "BIT", 1, 0, 2,0x010D8,0.,0,0,0,0},
    {         "TPA0", 3, 2,       "BIT", 1, 0, 2,0x010D0,0.,0,0,0,0},
    {         "TDM0", 3, 2,       "BIT", 1, 0, 2,0x010C8,0.,0,0,0,0},
    {         "TEN0", 3, 2,       "BIT", 1, 0, 2,0x010C0,0.,0,0,0,0},
    {           "T0", 3, 2,      "WORD",16, 0, 2,0x00800,0.,0,0,0,0},
    {        "RESET", 3, 2,       "BIT", 1, 0, 2,0x0113E,0.,0,0,0,0},
    {      "POINTER", 3, 2,      "WORD",16, 0, 2,0x00910,0.,0,0,0,0},
    {  "PAGEPOINTER", 3, 2,      "WORD",16, 0, 2,0x00900,0.,0,0,0,0},
    {        "SPEED", 3, 2,      "WORD",16, 0, 2,0x008F0,0.,0,0,0,0},
    {        "TURBO", 3, 2,       "BIT", 1, 0, 2,0x0113D,0.,0,0,0,0},
    {     "SENDCODE", 3, 2,      "WORD",16, 0, 2,0x00AA0,0.,0,0,0,0},
    {     "SENDADDR", 3, 2,      "WORD",16, 0, 2,0x00AB0,0.,0,0,0,0},
    {      "MEMADDR", 3, 2,      "WORD",16, 0, 2,0x00AC0,0.,0,0,0,0},
    {      "BITADDR", 3, 2,      "WORD",16, 0, 2,0x00AD0,0.,0,0,0,0},
    {     "SENDDATA", 3, 2,      "WORD",16, 0, 2,0x00AE0,0.,0,0,0,0},
    {       "MYADDR", 3, 2,      "WORD",16, 0, 2,0x00B20,0.,0,0,0,0},
    {      "SENDBIT", 3, 2,       "BIT", 1, 0, 2,0x010F9,0.,0,0,0,0},
    {    "CALIBSAVE", 3, 2,       "BIT", 1, 0, 2,0x010FA,0.,0,0,0,0},
    {    "SLAVEMODE", 3, 2,       "BIT", 1, 0, 2,0x010FB,0.,0,0,0,0}
  };

  SezUloz (TypesTab+0);
  SezUloz (TypesTab+1);
  SezUloz (TypesTab+2);
  SezUloz (TypesTab+3);
  SezUloz (TypesTab+4);
  SezUloz (TypesTab+5);

  for (i=PREDEFNUM-1; i>=0; i--) {
    SezSUloz (PredefTab+i);
  }

  VarSaved = 0;
}

