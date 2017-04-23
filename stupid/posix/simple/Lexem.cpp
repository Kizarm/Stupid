// Soubor : Lexem.cpp
//
// Popis  : Obsahuje implementaci tridy CLexem, ktera provadi zakladni
//        lexikalni analyzu.
//
// Autor  : Martin Kucera
//
// Historie : 27.11.03  - vytvorena
//
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//-#include "stdafx.h"
#include "Lexem.h"
#include "Error.h"

#include <stdio.h> // fopen, fread
#include <stdlib.h>
#include <string.h>

#define RET(a)      m_Token.Class = a; \
  GetInput(); \
  break;
/** TODO: iterator ? NULL */

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLexem::CLexem (const char * filename /*= NULL*/) :
  m_strFileName (filename), m_nLineNbr (1) {
  m_bInitialized = false;
  // mapa instrukci
  m_iMap["ADD"] = iADD;
  m_iMap["ADDC"]  = iADDC;
  m_iMap["ANL"] = iANL;
  m_iMap["CLR"] = iCLR;
  m_iMap["CPL"] = iCPL;
  m_iMap["DEC"] = iDEC;
  m_iMap["INC"] = iINC;
  m_iMap["JC"]  = iJC;
  m_iMap["JNC"] = iJNC;
  m_iMap["JNB"] = iJNB;
  m_iMap["JNZ"] = iJNZ;
  m_iMap["JZ"]  = iJZ;
  m_iMap["LCALL"] = iLCALL;
  m_iMap["LJMP"]  = iLJMP;
  m_iMap["MOV"] = iMOV;
  m_iMap["MOVX"]  = iMOVX;
  m_iMap["ORL"] = iORL;
  m_iMap["RET"] = iRET;
  m_iMap["SETB"]  = iSETB;
  m_iMap["SUBB"]  = iSUBB;
  m_iMap["XCH"] = iXCH;
  m_iMap["XRL"] = iXRL;
  m_iMap["MUL"] = iMUL;
  m_iMap["iDIV"]  = iDIV;
  m_iMap["DAA"] = iDAA;
  m_iMap["RL"]  = iRL;
  m_iMap["RLC"] = iRLC;
  m_iMap["RR"]  = iRR;
  m_iMap["RRC"] = iRRC;
  m_iMap["SWAP"]  = iSWAP;
  m_iMap["MOVC"]  = iMOVC;
  m_iMap["PUSH"]  = iPUSH;
  m_iMap["POP"] = iPOP;
  m_iMap["XHD"] = iXHD;
  m_iMap["ACALL"] = iACALL;
  m_iMap["RETI"]  = iRETI;
  m_iMap["AJMP"]  = iAJMP;
  m_iMap["SJMP"]  = iSJMP;
  m_iMap["JMP"] = iJMP;
  m_iMap["JB"]  = iJB;
  m_iMap["JBC"] = iJBC;
  m_iMap["CJNE"]  = iCJNE;
  m_iMap["DJNZ"]  = iDJNZ;
  m_iMap["NOP"] = iNOP;

  // mapa slov jazyka
  m_lexMap["EQU"] = EQU;
  m_lexMap["DB"]  = DB;
  m_lexMap["END"] = END;
  m_lexMap["ORG"] = ORG;
  m_lexMap["DATA"]= DATA;

  // mapa registru
  m_regMap["A"] = rA;
  m_regMap["ACC"] = rACC;
  m_regMap["AB"]  = rAB;
  m_regMap["C"] = rC;
  m_regMap["CY"]  = rCY;
  m_regMap["DPTR"]= rDPTR;
  m_regMap["PC"]  = rPC;
  m_regMap["DPL"] = rDPL;
  m_regMap["DPH"] = rDPH;
  m_regMap["R0"]  = rR0;
  m_regMap["R1"]  = rR1;
  m_regMap["R2"]  = rR2;
  m_regMap["R3"]  = rR3;
  m_regMap["R4"]  = rR4;
  m_regMap["R5"]  = rR5;
  m_regMap["R6"]  = rR6;
  m_regMap["R7"]  = rR7;
  m_regMap["AR0"] = rAR0;
}

CLexem::~CLexem() {
  FileClose();
}

//////////////////////////////////////////////////////////////////////
// Provede inicializaci lexikalniho analyzatoru
//////////////////////////////////////////////////////////////////////
void CLexem::InitLexem() {
  FileInit (m_strFileName); // inicializace vstupniho souboru
  GetInput();         // inicializace analyzatoru
  GetNextToken();     // nactu prvni symbol
}

//////////////////////////////////////////////////////////////////////
//  Inicializace vstupniho souboru
//////////////////////////////////////////////////////////////////////
void CLexem::FileInit (string file) {
  m_inFile.open (file.c_str());

  if (!m_inFile.is_open()) {
    throw CFileError (file);
  }

  m_strFileName = file;
  m_nOffset = 0L;
  m_nIndex = 0L;
  m_inFile.read (m_pBuffer, BUFFER_SIZE);
  m_nLen = m_inFile.gcount();
}

//////////////////////////////////////////////////////////////////////
//  Uzavreni vstupniho souboru
//////////////////////////////////////////////////////////////////////
void CLexem::FileClose() {
  if (m_inFile != NULL) {
    m_inFile.close();
  }
}

//////////////////////////////////////////////////////////////////////
//  Cteni znaku a jeho prevod na symbol
//////////////////////////////////////////////////////////////////////
char CLexem::GetInput() {
  CharRead();

  if (isalpha (m_cLastChar)) {
    m_nLastInput = 'c';
  } else if (isdigit (m_cLastChar)) {
    m_nLastInput = 'n';
  } else if (m_cLastChar == ' ' || m_cLastChar == '\t' || m_cLastChar == '\r') {
    m_nLastInput = 's';
  } else if (m_cLastChar == EOF) {
    m_nLastInput = 'e';
  } else {
    m_nLastInput = m_cLastChar;
  }

//  printf("%c", m_cLastChar);

  return m_nLastInput;
}

//////////////////////////////////////////////////////////////////////
//  Cteni znaku
//////////////////////////////////////////////////////////////////////
char CLexem::CharRead() { // konvertuje znak na velke pismeno
  m_cLastChar = toupper (CharIn());
  return m_cLastChar;
}

char CLexem::CharReadLU() {
  m_cLastChar = CharIn();
  return m_cLastChar;
}

//////////////////////////////////////////////////////////////////////
//  Vraci aktualni precteny symbol
//////////////////////////////////////////////////////////////////////
void CLexem::GetNextToken() {
  int     nIDLen; // delka identifikatoru
  char    error[255];
  //-printf ("GetNextToken\n");

  m_Token.Class = UNKNOW;
  m_Token.Repr = "";
  m_Token.SubClass = 0;

  Gaps(); // dostane se, az na prvni vyznamny znak
  switch (m_nLastInput) {
      // konec vstupu
    case  'e' :
      m_Token.Class = EOI;
      m_Token.Repr = "<EOI>";
      break;
      // cislo
    case  'n' :
      if (IsNumber()) {
        m_Token.Class = NUMBER;
      }
      break;
      // komentar - dojde az na novou radku
    case  ';' :
      while (GetInput() != '\n' && m_nLastInput != 'e');
      if (m_nLastInput == 'e') { // konec souboru
        m_Token.Class = EOI;
        m_Token.Repr = "<EOI>";
      } else
        GetNextToken();
      break;
      // konec radky
    case  '\n':
      ++m_nLineNbr;
      m_Token.Class = EOL;
      m_Token.Repr = "<EOL>";
      GetInput();
      break;
    case  ',' :
      RET (COMMA)
    case  '#' :
      RET (HASH)
    case  ':' :
      RET (COLON)
    case  '.' :
      RET (DOT)
    case  '@' :
      RET (AT)
    case  '$' :
      RET (DOLLAR)
    case  '+' :
      RET (PLUS)
    case  '/' :
      RET (SEMI)
      // identifikator
    case  '_' :
    case  'c' :
      nIDLen = 1;
      m_Token.Repr = m_cLastChar;
      while (GetInput() == 'c' || m_nLastInput == 'n' || m_nLastInput == '_') {
        if (nIDLen < MAX_IDENT_LEN) {
          m_Token.Repr += m_cLastChar;
          ++nIDLen;
        }
      }
      m_Token.Class = KeyWord (m_Token.Repr.c_str());
      break;
      // nesmysl
    default:
      sprintf (error, "Nepovoleny vstupni znak \'%c\'", m_cLastChar);
      throw CSyntaxError (this, error);
  }
}

//////////////////////////////////////////////////////////////////////
//  Pokud je id klicove slovo, vrati jeho enum, nebo vraci neznami symbol.
//  Jmeno uklada na zasobnik.
//////////////////////////////////////////////////////////////////////
LexSymbol CLexem::KeyWord (const char * id) {
  // test na registr
  STRING2REG::iterator  regIterator = m_regMap.begin();
  while (regIterator != m_regMap.end()) {
    if (strcmp ( (*regIterator).first, id) == 0) {
      m_Token.SubClass = (*regIterator).second;
      return REGISTER;
    }
    ++regIterator;
  }

  // test na instrukci
  STRING2INS::iterator  insIterator = m_iMap.begin();
  while (insIterator != m_iMap.end()) {
    if (strcmp ( (*insIterator).first, id) == 0) {
      m_Token.SubClass = (*insIterator).second;
      return INSTRUCTION;
    }
    ++insIterator;
  }

  // test na slova jazyka
  STRING2LEX::iterator  lexIterator = m_lexMap.begin();
  while (lexIterator != m_lexMap.end()) {
    if (strcmp ( (*lexIterator).first, id) == 0)
      return (*lexIterator).second;
    ++lexIterator;
  }

  m_Token.Repr = id;
  return IDENT;
}

//////////////////////////////////////////////////////////////////////
//  Rozpoznava sled ignorovanych znaku a posouva ukazatel na prvni odlisny
//////////////////////////////////////////////////////////////////////
bool CLexem::Gaps() {
  if (m_nLastInput != 's')
    return false;
  while (m_nLastInput == 's')
    GetInput();
  return true;
}


//////////////////////////////////////////////////////////////////////
//  Cteni akualniho znaku ze souboru
//////////////////////////////////////////////////////////////////////
char CLexem::CharIn() {
  long  dif;
  int   numread;

  dif = m_nIndex - m_nOffset;
  if ( (dif < 0L) || (dif >= BUFFER_SIZE)) {
    m_nOffset += BUFFER_SIZE;
    m_inFile.read (m_pBuffer, BUFFER_SIZE);
    numread = m_inFile.gcount();
    dif = m_nIndex - m_nOffset;
    m_nLen += numread;
    if (m_nIndex >= m_nLen)
      return EOF;
  }

  if (m_nIndex >= m_nLen)
    return EOF;

  ++m_nIndex;

  return * (m_pBuffer + dif);
}

//////////////////////////////////////////////////////////////////////
// Sleduje text, dokud se jedna cislo
//////////////////////////////////////////////////////////////////////
bool CLexem::IsNumber() {
  char  firstnum = m_cLastChar;
  bool  hex = false, isHex = false;

  m_Token.Repr = m_cLastChar;

  while (GetInput() == 'n' || m_nLastInput == 'c') {
    m_Token.Repr += m_cLastChar;
    if (m_nLastInput == 'c') {
      if (m_cLastChar >= 'A' && m_cLastChar <= 'F') {
        hex = true;
      } else {
        if (m_cLastChar == 'H') {
          isHex = true;
          GetInput();
          break;
        }
        throw CSyntaxError (this, "cislo obsahuje neplatny znak");
      }
    }
  }

  if (isHex) { // ukoncuje hexa cislo
    if (firstnum != '0') {
      throw CSyntaxError (this, "hexadecimalni cislo musi zacinat nulou");
    }
  } else if (hex) {
    throw CSyntaxError (this, "dekadicke cislo obsahuje hexadecimalni znak");
  }

  //string::iterator it = m_Token.Repr.end();
  if (!m_Token.Repr.empty()) {
    //-printf ("HEX=%d,TOKEN:%s\n", isHex, m_Token.Repr.c_str());
    /*! Pouzito jen pri optimalizaci mac, funguje zrejme i tak */
    m_Token.SubClass = strtoul (m_Token.Repr.c_str(), /*& (--it, --it)*/ NULL, isHex ? 16 : 10);
  }

//  GetInput();

  return true;  // dekadicke cislo
}

//////////////////////////////////////////////////////////////////////
// Provede srovnani aktualniho symbolu
//////////////////////////////////////////////////////////////////////
void CLexem::Require (LexSymbol symb) {
  if (symb == m_Token.Class)
    GetNextToken();
  else {
    char  buff[255];
    sprintf (buff, "Ocekavany symbol %d", symb);
    throw CSyntaxError (this, buff);
  }
}
/*
//////////////////////////////////////////////////////////////////////
// Provede srovnani aktualniho symbolu(ocekava identifikator),
// vraci jeho nazev
//////////////////////////////////////////////////////////////////////
const char* CLexem::CompareID()
{
  const char* ret = NULL;
  if (m_Symbol == IDENT)
  {
    ret = m_strAttribut.c_str();
    GetNextToken();
    return ret;
  }
  else
  {
    throw CSyntaxError(this, "Ocekavam identifikator");
  }
}

//////////////////////////////////////////////////////////////////////
// Provede srovnani aktualniho symbolu(ocekava instrukci),
// vraci jeji symbol
//////////////////////////////////////////////////////////////////////
Instruction CLexem::CompareIns()
{
  Instruction ret = iNULL;
  if (m_Symbol == INSTRUCTION)
  {
    ret = (Instruction)m_;
    GetNextToken();
    return ret;
  }
  else
  {
    throw CSyntaxError(this, "Ocekavam instrukci");
  }
}

//////////////////////////////////////////////////////////////////////
// Provede srovnani aktualniho symbolu(ocekava registr),
// vraci jeho symbol
//////////////////////////////////////////////////////////////////////
Register CLexem::CompareReg()
{
  Register  ret = rNULL;
  if (m_Symbol == REGISTER)
  {
    ret = (Register)m_nAttribut;
    GetNextToken();
    return ret;
  }
  else
  {
    throw CSyntaxError(this, "Ocekavam registr");
  }
}

//////////////////////////////////////////////////////////////////////
// Provede srovnani aktualniho symbolu(ocekava cislo),
// vraci hodnotu cisla
//////////////////////////////////////////////////////////////////////
int CLexem::CompareNbr()
{
  int ret = -1;
  string::iterator  it = m_strAttribut.end();
  if (m_Symbol == NUMBER)
  {
    if (!m_strAttribut.empty())
    {
      ret = strtoul(m_strAttribut.c_str(), &(--it, --it),
      (*(++it) == 'H') ? 16 : 10);
    }
    GetNextToken();
    return ret;
  }
  else
  {
    throw CSyntaxError(this, "Ocekavam instrukci");
  }
}
*/
