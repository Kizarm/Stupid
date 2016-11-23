#if !defined(AFX_ZNAK_H__INCLUDED_)
#define AFX_ZNAK_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define POLOZEK 100  /* maximalni pocet polozek na zasobniku   */


extern int		GetLineNbr();
extern void		GetLineNbrS(char* v);
extern int      ZnakFn;
extern long int znak_uk;
extern long int pos_znak_uk   ;
extern char     znak_posl;
extern void     ZnakInit();
extern void     ZnakEnd();
extern void     ZnakKde();
extern int      ZnakLine();
extern char     ZnakCti();
extern char     ZnakCtiMV();
extern void     StringInit(const char* str);
extern void		StringEnd();
extern void		FileInit(char* file);
extern void		MacroInit(char* a);

struct f_sez {
    char   fname[800]                 ; /* jmeno file          */
    void (*re)()                     ; /* jak znovuotevrit    */
    char (*cl)()                     ; /* jak zavrit          */
    char (*rd)(long)                 ; /* jak cist            */
    char  *buff                      ; /* buffer              */
    long   zuk                       ; /* kde jsme            */
    long   off                       ; /* offset bufferu      */
    long   len                       ; /* delka souboru       */
    };

extern f_sez FSez[POLOZEK];

#endif // AFX_ZNAK_H__INCLUDED_