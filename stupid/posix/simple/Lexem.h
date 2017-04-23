// Soubor : Lexem.h
//
// Popis  : Obsahuje deklaraci tridy CLexem, ktera provadi zakladni
//        lexikalni analyzu.
//
// Autor  : Martin Kucera
//
// Historie : 27.11.03  - vytvorena
//
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_LEXEM_H__67619B40_06FA_4F1F_B7B9_4DDB5AFCBE09__INCLUDED_)
#define AFX_LEXEM_H__67619B40_06FA_4F1F_B7B9_4DDB5AFCBE09__INCLUDED_

#if 0
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#endif // 0

#include <fstream>
#include <stack>
#include <map>

#define CR 0x0a         /* line feed */
#define MAX_IDENT_LEN 25    /* maximalni delka identifikatoru */
#define BUFFER_SIZE   1024  /* velikost bufferu pro ulozeni bloku */
using namespace std;


// Definice lexikalnich symbolu
typedef enum {
  UNKNOW = -1,//
  NUMBER,   // 0  cislo
  COLON,    // 1  :
  DOT,    // 2  .
  HASH,   // 3  #
  COMMA,    // 4  ,
  AT,     // 5  @
  DOLLAR,   // 6  $
  PLUS,   // 7  +
  SEMI,   // 8  /
  EOL,    // 9  konec radky
  EOI,    // 10 konec souboru
  INSTRUCTION,// 11 instrukce(mov, add, ...)
  REGISTER, // 12 registr(R1, R2, A)
  IDENT,    // 13 identifikator
  END,    // 14 "END" konec zdrojaku
  DATA,   // 15
  EQU,    // 16
  DB,     // 17
  ORG     // 18
} LexSymbol;

typedef enum {
  iNULL = -1,
  iADD, iADDC, iANL, iCLR, iCPL,
  iDEC, iINC, iJC, iJNC, iJNB, iJNZ, iJZ,
  iLCALL, iLJMP, iMOV, iMOVX, iORL, iRET,
  iSETB, iSUBB, iXCH, iXRL, iMUL, iDIV, iDAA,
  iRL, iRLC, iRR, iRRC, iSWAP, iMOVC, iPUSH,
  iPOP, iXHD, iACALL, iRETI, iAJMP, iSJMP,
  iJMP, iJB, iJBC, iCJNE, iDJNZ, iNOP
} Instruction;

typedef enum {
  rNULL = -1,
  rA, rACC, rAB, rC, rCY, rDPTR, rPC, rDPL, rDPH,
  rR0, rR1, rR2, rR3, rR4, rR5, rR6, rR7, rAR0
} Register;

// definice mapy pro rozeznavani klicovych slov
typedef map<const char *, LexSymbol> STRING2LEX;
// definice mapy pro rozeznavani instrukci
typedef map<const char *, Instruction> STRING2INS;
// definice mapy pro rozeznavani registru
typedef map<const char *, Register> STRING2REG;



class CLexem {
    // prevod zakladnich slov jazyka
    STRING2LEX  m_lexMap; // mapa slov jazyka
    // prevod podkategorii slov jazyka
    STRING2INS  m_iMap;   // mapa instrukci
    STRING2REG  m_regMap; // mapa registru

    // zprostredkovani znaku
    ifstream  m_inFile;
    char    m_pBuffer[BUFFER_SIZE];   // buffer pro jednu radku
    long    m_nIndex;         // aktualni pozice v souboru
    long    m_nOffset;          // offset v souboru
    int     m_nLen;           // delka nacteneho bufferu
  protected:
    string    m_strFileName;      // nazev souboru
    void    FileInit (string file); // inicializace souboroveho lex. analyzatoru
    void    FileClose();      // uzavreni vstupniho souboru
    char    CharIn();       // cteni aktualniho znaku ze souboru
    char    CharRead();       // cteni znaku podle ukazovatka na zasobniku
    char    CharReadLU();     // cteni znaku podle ukazovatka na zasobniku
    char    GetInput();       // cteni znaku a jeho prevod na symbol

    void  InitLexem();  // provede inicializaci lexikalniho analyzatoru
    char  m_nLastInput; // posledni nacteny symbol
    int   m_nLineNbr;   // aktualni cislo radku
  private:
    char  m_cLastChar;  // posledni nacteny znak
    //- long  m_nCharPtr;   // aktualni pointer
    //- int   m_nCharFn;    // cislo aktualniho souboru

    bool  m_bInitialized;

    LexSymbol KeyWord (const char * id); // pokud je id klicove slovo, vrati jeho id,
    // nebo vraci neznami symbol. Jmeno uklada na zasobnik
    bool IsNumber();  // sleduje text, dokud se jedna cislo
  public:
    // Konstruktor destruktor
    CLexem (const char * filename = NULL);
    virtual ~CLexem();

    // Pomocne metody
    const char * GetFileName() {
      return  m_strFileName.c_str();
    };
    const int GetLine() {
      return m_nLineNbr;
    }

  protected:
    bool  Gaps(); // rozpoznava sled ignorovanych znaku a posouva ukazatel na prvni odlisny


    struct tagTOKEN {
      LexSymbol Class;  // symbol
      string    Repr; // string s hodnotou
      int     SubClass; // instrukce, registr, ...
    } m_Token;

    // Prace se symboly a znaky
    void    GetNextToken();// vraci nasledujici token
    void    Require (LexSymbol symb); // provede srovnani aktualniho symbolu
};


#endif // !defined(AFX_LEXEM_H__67619B40_06FA_4F1F_B7B9_4DDB5AFCBE09__INCLUDED_)

