// Soubor : Optimizer.cpp
//
// Popis  : Obsahuje implementci tridy COptimizer, ktera provadi
//        optimalizaci zdrojovych kodu v ASM51.
//
// Autor  : Martin Kucera
//
// Historie : 10.12.03  - vytvorena
//
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//-#include "stdafx.h"
#include "Optimizer.h"
#include "Error.h"
#include <iostream>
#include "inc/chyby.h"
#include <fstream>


using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COptimizer::COptimizer (const char * name) :
  CSynan (name) {

}

COptimizer::~COptimizer() {

}

//////////////////////////////////////////////////////////////////////
// spusti syntaktickou analyzu a optimalizaci, odchytava vyjimky
//////////////////////////////////////////////////////////////////////
bool COptimizer::RunOptimize() {
  try {
    // inicializace lexalniho analyzatoru
    InitLexem();

    // syntakticka analyza
    AddInfo ("\nProbiha syntakticka analyza ... ");
    Program();
    AddInfo ("v poradku");

    // uzavreni vstupniho souboru
    FileClose();

    // optimalizace
    AddInfo ("\nProbiha optimalizace ... ");
    Optimize();
    AddInfo ("v poradku");

    // vytvoreni noveho souboru
    AddInfo ("\nVytvarim optimalizovany soubor ... ");
    CreateOptimizeFile();
  } catch (CError & err) {
    AddInfo ("\n%s", err.GetError());
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////
// vytvoreni noveho optimalizovaneho souboru
//////////////////////////////////////////////////////////////////////
void COptimizer::CreateOptimizeFile() {
  // otevreni vstupniho souboru
  ifstream  in (m_strFileName.c_str());

  // vytvoreni optimalizovaneho  souboru
  * (m_strFileName.end() - 1) = 'o';
  ofstream  out (m_strFileName.c_str());

  string  strLine;
  long  line = 1, delline = 0;
  vector<tagLABEL>::iterator  it;
  bool  bWrite;

  while (getline (in, strLine)) {
    if (strLine.empty() || strLine[0] == ';') {
      ++line;
      out << strLine << endl;
      continue;
    }

    it = m_vecNotUsedLabels.begin();
    bWrite = true;
    while (it != m_vecNotUsedLabels.end()) {
      if (line >= it->lDefLine && line <= it->lDefEndLine) {
        bWrite = false;
        break;
      }
      ++it;
    }
    if (!bWrite) {
      delline++;
      out << ";; deleted ";
    }
    out << strLine << endl;
    ++line;
  }

  AddInfo ("odstraneno %d radek",delline);
}
