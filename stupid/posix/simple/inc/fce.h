#if !defined(AFX_FCE_H__INCLUDED_)
#define AFX_FCE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
//#include "INCLUDES\\list.h"
/*------ cmdline.c -------------------------*/

extern void FName(char *s,char *e);
extern void ExPath(char *s);
extern void AnalCmdLine( void );

/*------ konst.c -------------------------*/

extern void Konstanta(char *t);
extern void KonstantniVyraz(char *t );
extern void KonstantniCinitel(char *t );
extern void KonstantniSoucin( char *t );

/*------ konstdef.c -------------------------*/

extern void DeklaraceKonst( void );

/*------ macrodef.c -------------------------*/

extern void DeklaraceMacro( void );

/*------ predef.c -------------------------*/
extern void PredefInit(void);
extern void PredefVar(char *t);
extern void PredefAdr(unsigned long a);
extern void PredefVypis(FILE *ff);

/*------ prikaz.c -------------------------*/

extern void JednoduchyPrikaz( void );
extern void podminka( void );
extern void IfThenElse( void );
extern void CaseOf( void );
extern void Prikaz( void );
extern void blok( void );
extern void SystemDefinition( void );
extern void Program( void );

/*------ procdef.c -------------------------*/

extern void ProcedureDef( void );
extern void FunctionDef( void );
extern void VyrazTypu(const char *typ);
extern void ProcedureCall( void );
extern void FunctionCall(char *t);

/*------ typdef.c -------------------------*/

extern void IdList(const char *pref);
extern void OznacujeTyp( void );
extern void TypeId(char *t);
extern void OrdinalTypeId(char *t);
extern int  UrciPtyp(char *t);
extern long UrciSize(char *t);
extern long UrciBSize(char *t);
extern void Type(char *t);
extern int  TypeNum(const char * t);
extern void TypesInit();

/*------ vardef.c -------------------------*/

extern void DeklaraceVar(unsigned long int *s);

/*------ varref.c -------------------------*/

extern int  NotTypId(char *s);
extern void ExpandType(char *s);
extern void VyrazTypu(const char *typ);
extern void Qualifier(char *v,ATRIBUT a,unsigned long si);
extern void VarReference(char *t);

/*------ vyhodnot.c -------------------------*/

extern void vyhodnot( void );

/*------ vypis.c -------------------------*/
extern void SeznamList(FILE *sf,POLOZKA *ls);
extern void VarsVypis(FILE *sf,POLOZKA *ls);
extern void CSourceGen( void );

/*------ vyraz.c -------------------------*/

extern void ConvType( const char * to, char * from);
extern void DualOper( char *tl, char *tn, const char *o, int tab[N_last][N_last] );
extern void SwapOper( void );
extern void InsOper(char *o);
extern void OneOper(char *tl, const char *o, int tab[N_last] );
extern void MathOper(char *tl, char *tn, const char *o);
extern void LogOper (char *tl, char *tn, const char *o);
extern void ChsOper (char *tl, const char *o);
extern void RealOper(char *tl, const char *o);
extern void NotOper (char *tl, const char *o);
extern void CopyOper(char *tl, const char *o);
extern void VyrazVZ(char *typ);
extern void Vyraz(char *typ);
extern void VyrazTypu(const char *typ);
extern void StandardFce(char *t);
extern void Macro( void );
extern void JPrvek(char *typ);
extern void Prvek(char *typ);
extern void JednoduchyVyraz(char *typ);
extern void ChsWordOper (char *tl, const char *o);

#endif // AFX_FCE_H__INCLUDED_