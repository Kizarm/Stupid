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
#include "inc/chyby.h"


/*===========================================================================*/
/*                                                                           */
/* Variable reference                                                        */
/*                                                                           */
/*===========================================================================*/

int  NotTypId (char * s) {
  switch (*s) {
    case '[':
    case '<':
    case '{':
    case '(':
    case '^':
    case '/':
    case '%':
      return ~0;
    default :
      return  0;
  }
}

void ExpandType (char * s) {
  char id[ID_DEL],*t;
  int f;

  /*--- pokud zacina znaky [,(,^,cislici nemuze byt Id. typu (urychleni) ---*/

  if (NotTypId (s)) {
    return;
  }

  do {
    f=0;
    StringInit (s);
    je();
    {
      TypeId (id);
      if (pkk()) {
        if (sez_uk->typ == I_ntyp) {
          if ( (*sez_uk->dtyp!='%')
               && (*sez_uk->dtyp!='/')
             ) {
            t=s+strlen (id);
            strcpy (id,sez_uk->dtyp);
            strcat (id,t);
            strcpy (s,id);
            f=1;
          }
        }
      }
    }
    vse();
    StringEnd();
  } while (f);
  Pravda();
}

/*===========================================================================*/

static void ExtractInterval (char * s,int * min,int * max) {
  char id[ID_DEL];

  ExpandType (s);

  StringInit (s);
  je();
  {
    OrdinalTypeId (id);
    if (pkk()) {
      *min=0;
      *max=sez_uk->atr-1;
    }
    if (neb()) {
      KlSlovo ("<");
      if (pkk()) {
        sscanf (sez_uk->dtyp,"<%d..%d>",min,max);
      }
    }
  }
  vse();
  StringEnd();

  if (!uspech) {
    AddInfo ("\n:%s:%d",sez_uk->iden,sez_uk->podtyp);
    ErrFatal ("Internal error 'ExtractInterval'");
  }
}

extern int BitFlg ;
static void SubIndex (char * v) {
  char u[ID_DEL];
  char * t,*s;
  int /*i,*/min,max;
  long li;

  ExpandType (v);
  t=v;
  s=u;
  if (*v!='[') {
    ErrSynt ("Neocekavany index");
  }
  t++;
  do {
    *s++=*t++;
  } while (*t!=']');
  t++;
  *s='\0';
  ByteCopy (v,t);
  //strcpy (v,t);

  ExtractInterval (u,&min,&max);
  sprintf (u,"<%d..%d>",min,max);
  StrAdd (u);

  li=UrciSize (v);
  if (!BitFlg) {
    li=li/8;
  }
  sprintf (u,"[%ld]",li);
  StrAdd (u);
  StrApp();
//  printf("li:%d\n", li);
}

extern void VyrazTypu (const char * typ);
static void Index (char * v) {
  je();
  {
    KlSlovo ("[");
    if (zrtz()) {
      VyrazTypu ("WORD");
      if (pkk()) {
        SubIndex (v);
      }
      if (zrtz()) {
        _sled();
        do {
          KlSlovo (",");
          if (zrtz()) {
            VyrazTypu ("WORD");
            if (pkk()) {
              SubIndex (v);
            }
          }
        } while (sled_());
        if (zrtz()) {
          NKlSlovo ("]");
        }
      }
    }
  }
  vse();
}


/*---------------------------------------------------------------------------*/

void Qualifier (char * v,ATRIBUT a,unsigned long si) {
  char u[ID_DEL];
  je();
  {
    Index (v);
  }
  vse();
  if (pkk()) {
    sprintf (u,"<T.%ld.%ld>%s",a,si-UrciSize (v),v);
    StrAdd (u);
  }
//   printf("Q:%s,%d\n", u, si);
}

/*---------------------------------------------------------------------------*/

void VarReference (char * t) {
  char u[ID_DEL],v[ID_DEL],w[ID_DEL];
  char c='\0';
  ATRIBUT a=0L;
  unsigned long  si=0;

  Macro();

  je();
  {
    IdentVSeznamu (u);
    if (zrtz()) {
      SezTypu (I_var);
      if (pkk()) {
        strcpy (v,sez_uk->dtyp);
        StrPush (" ");
        if (*v=='@') {
          //strcpy (v,v+1);
          ByteCopy (v,v+1);
          c='?';
        } else {
          c='.';
          if (sez_uk->trida==C_auto) {
            c='^';
          }
        }
        a=sez_uk->atr;
        si=sez_uk->size;
      }
      if (zrtz()) {
        _sled();
        do {
          Qualifier (v,a,si);
        } while (sled_());
        sprintf (w,"{%s},%ld%c%s",u,a,c,v);
        InsOper (w);
      }
    }
    if (neb()) {
      PredefVar (u);
      if (pkk()) {
        strcpy (v,sez_uk->dtyp);
        a=sez_uk->atr;
        sprintf (w,"{%s},%ld.%s",u,a,v);
        StrPush (w);
      }
    }
  }
  vse();

  if (pkk()) {
    strcpy (t,v);
  }
}

