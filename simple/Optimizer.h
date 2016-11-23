// Soubor : Optimizer.h
//
// Popis  : Obsahuje deklaraci tridy COptimizer, ktera provadi
//        optimalizaci zdrojovych kodu v ASM51.
//
// Autor  : Martin Kucera
//
// Historie : 10.12.03  - vytvorena
//
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIMIZER_H__FA867829_CBFB_42D5_B0A6_E674221F1CBA__INCLUDED_)
#define AFX_OPTIMIZER_H__FA867829_CBFB_42D5_B0A6_E674221F1CBA__INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include "SynAn.h"

class COptimizer : public CSynan {
    void CreateOptimizeFile();  // vytvoreni noveho optimalizovaneho souboru

  public:
    COptimizer (const char * name);
    virtual ~COptimizer();

    bool RunOptimize(); // spusti syntaktickou analyzu a optimalizaci, odchytava vyjimky
};

#endif // !defined(AFX_OPTIMIZER_H__FA867829_CBFB_42D5_B0A6_E674221F1CBA__INCLUDED_)
