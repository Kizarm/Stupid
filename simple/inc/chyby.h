/*###########################################################################*/
/*    Hlaseni chyb                                                           */
/*###########################################################################*/

#if !defined(AFX_CHYBY_H__INCLUDED_)
#define AFX_CHYBY_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern void ErrFatal   (const char *hlaseni);
extern void ErrFile    (const char *hlaseni);
extern void ErrSynt    (const char *hlaseni);
extern void ErrSyntMiss(const char *CoChybi);
extern void AddInfo(const char *pszFormat, ...);
extern void Warning(char *s);

extern FILE *ef;
extern void (*LogFce)(const char *buff);

extern bool config_error;

extern "C" char * ByteCopy (char * dst, const char * src);

#define EXITFATAL (3)
#define EXITFILE  (2)
#define EXITSYNT  (1)

#endif // AFX_CHYBY_H__INCLUDED_