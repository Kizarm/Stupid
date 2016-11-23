// Soubor : Synan.h
//
// Popis  : Obsahuje deklaraci tridy CSynan, ktera provadi
//        syntaktickou analyzu.
//
// Autor  : Martin Kucera
//
// Historie : 07.12.03  - vytvorena
//
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYNAN_H__BDF2F304_3940_4939_A768_27E6A1C2AA0B__INCLUDED_)
#define AFX_SYNAN_H__BDF2F304_3940_4939_A768_27E6A1C2AA0B__INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include "Lexem.h"
#include <string>
#include <vector>

using namespace std;

typedef enum {
  opNULL = -1,
  opA, opR, opDirect, opIndirectR, opImmediate,
  opDPTR, opAB, opIndirectADPTR, opIndirectAPC,
  opIndirectDPTR, opBit, opCplBit, opC, opAddress,
  opRelativeAddr, opVar, opDPL
} Operands;


class CSynan : public CLexem {
  public:
    // struktura pro uchovani navestich
    /*typedef*/
    struct tagLABEL {
      string        strName;  // jmeno navesti
      long        lDefLine; // radka s definici
      long        lDefEndLine;// radka s RET - konec deklarace
      int         nUsed;    // kolikrat je navesti pouzito
      vector<string>    vecCalling; // seznam navesti, ktere vola
    };

    // struktura pro uchovani operandu
    /*typedef*/ struct tagOPERAND {
      Operands  Type; // typ operandu
      int     nAddr;  // pokud se jedna o primou adresu, tak tady je
    };


    // struktura pro uchovani instrukce
    /*typedef*/ struct tagINSTRUCTION {
      Instruction Instr;    // instrukce
      char    nOperands;  // pocet operandu
      tagOPERAND  Operand[3]; // typ operandu 1,2,3
    };

    vector<tagINSTRUCTION>  m_vecInstMap; // seznam dovolench instrukci
    vector<string>      m_vecVariables; // seznam deklarovanych promennych
  public:
    CSynan (const char * name = NULL);
    virtual ~CSynan();

    vector<tagLABEL>  m_vecLabels;  // pole navesti

    void Optimize();  // provede optimalizaci - odstrani ze seznamu nepotrebne navesti

    void    Program();    // rozpoznava program
  protected:
    void    Command();    // rozpoznava prikaz
    tagOPERAND  Operand();    // rozpoznava operand
    void    Declaration();  // rozpoznava deklaraci


    Operands TranslateReg (Register r);     // Preklad registru na typ operandu
    Operands TranslateIndirectReg (Register r); // Preklad neprime adresace pres registr na typ operandu (napr. @A, @R1)
    Operands FindVariable (string id);    // Vyhledani identifikatoru v seznamu promennych,
    // pokud neni nalezna vraci opNULL
    void TestInstructionSyntax (tagINSTRUCTION instr); // Otestuje spravnou syntaxi instrukce

    string  m_strActualLabel; // aktualni navesti
    void AddLabelUse (string name); // test navesti - uziti
    void AddLabel (string name, long line); // test navesti - definice
    void AddLabelToActual (vector<tagLABEL>::iterator label);
    bool FindLabelByName (vector<tagLABEL>::iterator & itLabels, string name); // Nalezne navesti podle jmena a vrati na nej odkaz, jinak vraci end()
    vector<tagLABEL>  m_vecNotUsedLabels; // pole nadbytecnych navesti

    bool IsRET (int addr); // Testuje,jestli se jedna o FW funkci RET

};

#endif // !defined(AFX_SYNAN_H__BDF2F304_3940_4939_A768_27E6A1C2AA0B__INCLUDED_)
