#if !defined(AFX_LEXEM_H__INCLUDED_)
#define AFX_LEXEM_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#define  RET_DEL   (81)	
#define  RET_DEL   (600)	
//#define  ID_DEL   (81)
#define  ID_DEL   (600)

extern void WriteLine2MF(char write);
extern void WriteMyLine2MF(int pos);
extern char deklarace;
extern char aktualnisoubor[RET_DEL];

extern void Mezery();
extern void JinakCh   (const char *str );
extern void JinakChybi(const char *str );
extern void KlSlovo   (const char *str );
extern void NKlSlovo  (const char *str );
extern void Cislo();
extern void KladneCislo();
extern void RealCislo();
extern void Identifikator(char *s);
extern void Retezec   ( char *s );
extern void KonecSouboru();
extern void NovyId(char *t);
extern void IdentVSeznamu(char *s);

#endif // AFX_LEXEM_H__INCLUDED_
