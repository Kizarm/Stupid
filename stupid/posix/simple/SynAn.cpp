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

//-#include "stdafx.h"
#include "SynAn.h"
#include "Error.h"

// vlozi prototyp instrukce do m_vecInstMap
#define ADD_INST(i, c, o1, o2, o3)  inst.Instr = i;           \
  inst.nOperands = c;         \
  inst.Operand[0].Type = o1;      \
  inst.Operand[1].Type = o2;      \
  inst.Operand[2].Type = o3;      \
  m_vecInstMap.push_back(inst);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSynan::CSynan (const char * name) :
  CLexem (name) {
  tagINSTRUCTION  inst;
  //////////////////////////////
  // naplnim seznam prototypu //
  //////////////////////////////
  // Aritmeticke instrukce
  ADD_INST (iADD , 2, opA    , opR     , opNULL)
  ADD_INST (iADD , 2, opA    , opDirect    , opNULL)
  ADD_INST (iADD , 2, opA    , opIndirectR , opNULL)
  ADD_INST (iADD , 2, opA    , opImmediate , opNULL)
  ADD_INST (iADDC  , 2, opA    , opR     , opNULL)
  ADD_INST (iADDC  , 2, opA    , opDirect    , opNULL)
  ADD_INST (iADDC  , 2, opA    , opIndirectR , opNULL)
  ADD_INST (iADDC  , 2, opA    , opImmediate , opNULL)
  ADD_INST (iSUBB  , 2, opA    , opR     , opNULL)
  ADD_INST (iSUBB  , 2, opA    , opDirect    , opNULL)
  ADD_INST (iSUBB  , 2, opA    , opIndirectR , opNULL)
  ADD_INST (iSUBB  , 2, opA    , opImmediate , opNULL)
  ADD_INST (iINC , 1, opA    , opNULL    , opNULL)
  ADD_INST (iINC , 1, opR    , opNULL    , opNULL)
  ADD_INST (iINC , 1, opDirect , opNULL    , opNULL)
  ADD_INST (iINC , 1, opIndirectR, opNULL    , opNULL)
  ADD_INST (iDEC , 1, opA    , opNULL    , opNULL)
  ADD_INST (iDEC , 1, opR    , opNULL    , opNULL)
  ADD_INST (iDEC , 1, opDirect , opNULL    , opNULL)
  ADD_INST (iDEC , 1, opIndirectR, opNULL    , opNULL)
  ADD_INST (iINC , 1, opDPTR   , opNULL    , opNULL)
  ADD_INST (iMUL , 1, opAB   , opNULL    , opNULL)
  ADD_INST (iDIV , 1, opAB   , opNULL    , opNULL)
  ADD_INST (iDAA , 1, opA    , opNULL    , opNULL)
  // Logicke instrukce
  ADD_INST (iANL , 2, opA    , opR     , opNULL)
  ADD_INST (iANL , 2, opA    , opDirect    , opNULL)
  ADD_INST (iANL , 2, opA    , opIndirectR , opNULL)
  ADD_INST (iANL , 2, opA    , opImmediate , opNULL)
  ADD_INST (iANL , 2, opDirect , opA     , opNULL)
  ADD_INST (iANL , 2, opDirect , opImmediate , opNULL)
  ADD_INST (iORL , 2, opA    , opR     , opNULL)
  ADD_INST (iORL , 2, opA    , opDirect    , opNULL)
  ADD_INST (iORL , 2, opA    , opIndirectR , opNULL)
  ADD_INST (iORL , 2, opA    , opImmediate , opNULL)
  ADD_INST (iORL , 2, opDirect , opA     , opNULL)
  ADD_INST (iORL , 2, opDirect , opImmediate , opNULL)
  ADD_INST (iXRL , 2, opA    , opR     , opNULL)
  ADD_INST (iXRL , 2, opA    , opDirect    , opNULL)
  ADD_INST (iXRL , 2, opA    , opIndirectR , opNULL)
  ADD_INST (iXRL , 2, opA    , opImmediate , opNULL)
  ADD_INST (iXRL , 2, opDirect , opA     , opNULL)
  ADD_INST (iXRL , 2, opDirect , opImmediate , opNULL)
  ADD_INST (iCLR , 1, opA    , opNULL    , opNULL)
  ADD_INST (iCPL , 1, opA    , opNULL    , opNULL)
  ADD_INST (iRL  , 1, opA    , opNULL    , opNULL)
  ADD_INST (iRLC , 1, opA    , opNULL    , opNULL)
  ADD_INST (iRR  , 1, opA    , opNULL    , opNULL)
  ADD_INST (iRRC , 1, opA    , opNULL    , opNULL)
  ADD_INST (iSWAP  , 1, opA    , opNULL    , opNULL)
  // Instrukce prenosu dat
  ADD_INST (iMOV , 2, opA      , opR     , opNULL)
  ADD_INST (iMOV , 2, opA      , opDirect    , opNULL)
  ADD_INST (iMOV , 2, opA      , opIndirectR , opNULL)
  ADD_INST (iMOV , 2, opA      , opImmediate , opNULL)
  ADD_INST (iMOV , 2, opR      , opA     , opNULL)
  ADD_INST (iMOV , 2, opR      , opR     , opNULL)
  ADD_INST (iMOV , 2, opR      , opDirect    , opNULL)
  ADD_INST (iMOV , 2, opR      , opImmediate , opNULL)
  ADD_INST (iMOV , 2, opDirect   , opA     , opNULL)
  ADD_INST (iMOV , 2, opDirect   , opR     , opNULL)
  ADD_INST (iMOV , 2, opDirect   , opDirect    , opNULL)
  ADD_INST (iMOV , 2, opDirect   , opIndirectR , opNULL)
  ADD_INST (iMOV , 2, opDirect   , opImmediate , opNULL)
  ADD_INST (iMOV , 2, opIndirectR  , opA     , opNULL)
  ADD_INST (iMOV , 2, opIndirectR  , opDirect    , opNULL)
  ADD_INST (iMOV , 2, opIndirectR  , opImmediate , opNULL)
  ADD_INST (iMOV , 2, opDPTR     , opImmediate , opNULL)
  ADD_INST (iMOV , 2, opAddress    , opAddress   , opNULL)
  ADD_INST (iMOV , 2, opAddress    , opA     , opNULL)
  ADD_INST (iMOV , 2, opAddress    , opImmediate , opNULL)
  ADD_INST (iMOVC  , 2, opA      , opIndirectADPTR,opNULL)
  ADD_INST (iMOVC  , 2, opA      , opIndirectAPC , opNULL)
  ADD_INST (iMOVC  , 2, opA      , opIndirectR , opNULL)
  ADD_INST (iMOVC  , 2, opA      , opIndirectDPTR, opNULL)
  ADD_INST (iMOVX  , 2, opA      , opIndirectR , opNULL)
  ADD_INST (iMOVX  , 2, opA      , opIndirectDPTR, opNULL)
  ADD_INST (iMOVX  , 2, opIndirectR  , opA     , opNULL)
  ADD_INST (iMOVX  , 2, opIndirectDPTR , opA     , opNULL)
  ADD_INST (iPUSH  , 1, opDirect   , opNULL    , opNULL)
  ADD_INST (iPOP , 1, opDirect   , opNULL    , opNULL)
  ADD_INST (iXCH , 2, opA      , opR     , opNULL)
  ADD_INST (iXCH , 2, opA      , opDirect    , opNULL)
  ADD_INST (iXCH , 2, opA      , opIndirectR , opNULL)
  ADD_INST (iXHD , 2, opA      , opIndirectR , opNULL)
  // Logicke operace
  ADD_INST (iCLR , 1, opC    , opNULL    , opNULL)
  ADD_INST (iCLR , 1, opBit    , opNULL    , opNULL)
  ADD_INST (iSETB  , 1, opC    , opNULL    , opNULL)
  ADD_INST (iSETB  , 1, opBit    , opNULL    , opNULL)
  ADD_INST (iCPL , 1, opC    , opNULL    , opNULL)
  ADD_INST (iCPL , 1, opBit    , opNULL    , opNULL)
  ADD_INST (iANL , 2, opC    , opBit     , opNULL)
  ADD_INST (iANL , 2, opC    , opCplBit    , opNULL)
  ADD_INST (iORL , 2, opC    , opBit     , opNULL)
  ADD_INST (iORL , 2, opC    , opCplBit    , opNULL)
  ADD_INST (iMOV , 2, opC    , opBit     , opNULL)
  ADD_INST (iMOV , 2, opBit    , opC   , opNULL)
  // Rizeni behu programu
  ADD_INST (iACALL , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iLCALL , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iRET , 0, opNULL     , opNULL    , opNULL)
  ADD_INST (iRETI  , 0, opNULL     , opNULL    , opNULL)
  ADD_INST (iAJMP  , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iLJMP  , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iSJMP  , 1, opRelativeAddr , opNULL    , opNULL)
  ADD_INST (iSJMP  , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iJMP , 1, opIndirectADPTR, opNULL    , opNULL)
  ADD_INST (iJZ  , 1, opRelativeAddr , opNULL    , opNULL)
  ADD_INST (iJZ  , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iJNZ , 1, opRelativeAddr , opNULL    , opNULL)
  ADD_INST (iJNZ , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iJC  , 1, opRelativeAddr , opNULL    , opNULL)
  ADD_INST (iJC  , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iJNC , 1, opRelativeAddr , opNULL    , opNULL)
  ADD_INST (iJNC , 1, opAddress    , opNULL    , opNULL)
  ADD_INST (iJB  , 2, opBit      , opRelativeAddr, opNULL)
  ADD_INST (iJNB , 2, opBit      , opRelativeAddr, opNULL)
  ADD_INST (iJNB , 2, opBit      , opAddress   , opNULL)
  ADD_INST (iJBC , 2, opBit      , opRelativeAddr, opNULL)
  ADD_INST (iCJNE  , 3, opA      , opDirect    , opRelativeAddr)
  ADD_INST (iCJNE  , 3, opA      , opImmediate , opRelativeAddr)
  ADD_INST (iCJNE  , 3, opR      , opImmediate , opRelativeAddr)
  ADD_INST (iCJNE  , 3, opIndirectR  , opImmediate , opRelativeAddr)
  ADD_INST (iDJNZ  , 2, opR      , opRelativeAddr, opNULL)
  ADD_INST (iDJNZ  , 2, opDirect   , opRelativeAddr, opNULL)
  ADD_INST (iNOP , 0, opNULL     , opNULL    , opNULL)
}

CSynan::~CSynan() {
}

//////////////////////////////////////////////////////////////////////
// Rozpoznava program
//////////////////////////////////////////////////////////////////////
void CSynan::Program() {
  while (m_Token.Class != END) {
    Command();
  }
  Require (END);
  while (m_Token.Class == EOL) {
    GetNextToken();
  }

  Require (EOI);
}

//////////////////////////////////////////////////////////////////////
// Rozpoznava prikaz
//////////////////////////////////////////////////////////////////////
void CSynan::Command() {
  string      id;
  tagINSTRUCTION  instr;
  long      line = m_nLineNbr;

  switch (m_Token.Class) {
      // instrukce
    case  INSTRUCTION :
      instr.nOperands = 0;
      instr.Instr = (Instruction) m_Token.SubClass;
      //Require(INSTRUCTION);
      GetNextToken();
      if (m_Token.Class != EOL) {
        ++ (instr.nOperands);
        instr.Operand[0] = Operand();
        if (m_Token.Class == COMMA) {
          ++ (instr.nOperands);
          //Require(COMMA);
          GetNextToken();
          instr.Operand[1] = Operand();
          if (m_Token.Class == COMMA) {
            ++ (instr.nOperands);
            //Require(COMMA);
            GetNextToken();
            instr.Operand[2] = Operand();
          }
        }
      }
      TestInstructionSyntax (instr);
      if ( (instr.Instr == iRET) || (instr.Instr == iLJMP && IsRET (instr.Operand[0].nAddr))) {
        // nastavim aktualnimu navesti konec
        vector<tagLABEL>::iterator it;
        if (FindLabelByName (it, m_strActualLabel))
          it->lDefEndLine = line;
      }
      break;
      // identifikator
    case  IDENT :
      id = m_Token.Repr;
      //Require(IDENT);
      GetNextToken();
      switch (m_Token.Class) {
          // navesti
        case  COLON :
          //Require(COLON);
          GetNextToken();
          if (m_Token.Class == DB) {
            // deklarace
            Declaration();
            m_vecVariables.push_back (id);
          } else {
            // navesti
            AddLabel (id, line);
            Command();
            return;
          }
          break;
          // data
        case  DATA  :
          m_vecVariables.push_back (id);
          //Require(DATA);
          GetNextToken();
          Require (NUMBER);
          break;
        case  EQU :
          //Require(EQU);
          GetNextToken();
          Require (NUMBER);
          break;
        default:
          throw CSyntaxError (this, "Neocekavany znak za identifikatorem");
      }
      break;
      // ORG
    case  ORG :
      GetNextToken();
      //Require(ORG);
      Require (NUMBER);
      break;
      // prazdny prikaz
    case  EOL :
      GetNextToken();
      //Require(EOL);
      return;
    default:
      throw CSyntaxError (this, "Ocekavan prikaz");
  }
  Require (EOL);
}

//////////////////////////////////////////////////////////////////////
// Rozpozna operand
//////////////////////////////////////////////////////////////////////
CSynan::tagOPERAND CSynan::Operand() {
  string    id;
  tagOPERAND  op;
  op.Type = opNULL;

  switch (m_Token.Class) {
    case  NUMBER  :
      op.Type = opAddress;
      op.nAddr = m_Token.SubClass;
      GetNextToken();
      //Require(NUMBER);
      break;
    case  REGISTER:
      op.Type = TranslateReg ( (Register) m_Token.SubClass);
      GetNextToken();
      //Require(REGISTER);
      if (m_Token.Class == DOT) {
        GetNextToken();
        //Require(DOT);
        Require (NUMBER);
        op.Type = opBit;
      }
      break;
    case  HASH  :
      GetNextToken();
      //Require(HASH);
      switch (m_Token.Class) {
        case  NUMBER  :
          GetNextToken();
          //Require(NUMBER);
          break;
        case  IDENT :
          GetNextToken();
          //Require(IDENT);
          break;
        default:
          throw CSyntaxError (this, "Ocekavan identifikator, nebo cislo");
      }
      op.Type = opImmediate;
      break;
    case  AT    :
      GetNextToken();
      //Require(AT);
      op.Type = TranslateIndirectReg ( (Register) m_Token.SubClass);
      Require (REGISTER);
      if (m_Token.Class == PLUS) {
        if (op.Type != opA) {
          throw CSyntaxError (this, "Ocekavan registr A");
        }
        GetNextToken();
        //Require(PLUS);
        switch ( (Register) m_Token.SubClass) {
          case  rPC   :
            op.Type = opIndirectAPC;
            break;
          case  rDPTR :
            op.Type = opIndirectADPTR;
            break;
          default     :
            throw CSyntaxError (this, "Ocekavan registr PC, nebo DPTR");
        }
        Require (REGISTER);
      }
      break;
    case  DOLLAR    :
      GetNextToken();
      //Require(DOLLAR);
      Require (PLUS);
      Require (NUMBER);
      op.Type = opRelativeAddr;
      break;
    case  SEMI    :
      GetNextToken();
      //Require(SEMI);
      if (m_Token.Class == IDENT) {
        id = m_Token.Repr;
        GetNextToken();
        //Require(IDENT);
        if (FindVariable (id) != opNULL) {
          Require (DOT);
          Require (NUMBER);
        } else {
          throw CSyntaxError (this, "Ocekavana promenna");
        }
      } else {
        Require (REGISTER);
        Require (DOT);
        Require (NUMBER);
      }
      op.Type = opCplBit;
      break;
    case  IDENT:
      id = m_Token.Repr;
      GetNextToken();
      //Require(IDENT);
      if (m_Token.Class == DOT) {
        if (FindVariable (id) != opNULL) {
          op.Type = opDirect;
          //if (m_Token.Class == DOT)
          {
            GetNextToken();
            //Require(DOT);
            Require (NUMBER);
            op.Type = opBit;
          }
          break;
        } else {
          throw CSyntaxError (this, "Ocekavana promenna");
        }
      }
      op.Type = opAddress;
      AddLabelUse (id);
      break;
    default:
      throw CSyntaxError (this, "Ocekavan operand");
  }

  return op;
}

//////////////////////////////////////////////////////////////////////
// Rozpoznava deklaraci
//////////////////////////////////////////////////////////////////////
void CSynan::Declaration() {
  GetNextToken();
  //Require(DB);

  Require (NUMBER);

  m_vecVariables.push_back (m_Token.Repr);

  while (m_Token.Class != EOL) {
    Require (COMMA);
    Require (NUMBER);
  }
}

//////////////////////////////////////////////////////////////////////
// Preklad registru na typ operandu (napr. A, B, R1)
//////////////////////////////////////////////////////////////////////
Operands CSynan::TranslateReg (Register r) {
  switch (r) {
    case  rA    :
      return opA;
    case  rAB   :
      return opAB;
    case  rDPTR :
      return opDPTR;
    case  rC    :
      return opC;
    case  rDPL  :
    case  rDPH  :
    case  rAR0  :
    case  rR0   :
    case  rR1   :
    case  rR2   :
    case  rR3   :
    case  rR4   :
    case  rR5   :
    case  rR6   :
    case  rR7   :
      return opR;
    default     :
      break;
  }
  return opNULL;
}

//////////////////////////////////////////////////////////////////////
// Preklad neprime adresace pres registr na typ operandu (napr. @A, @R1)
//////////////////////////////////////////////////////////////////////
Operands CSynan::TranslateIndirectReg (Register r) {
  switch (r) {
    case  rA    :
      return opA;
    case  rR0   :
    case  rR1   :
    case  rR2   :
    case  rR3   :
    case  rR4   :
    case  rR5   :
    case  rR6   :
    case  rR7   :
      return opIndirectR;
    case  rDPTR :
      return opIndirectDPTR;
    default     :
      break;
  }
  return opNULL;
}

//////////////////////////////////////////////////////////////////////
// Vyhledani identifikatoru v seznamu promennych,
// pokud neni nalezna vraci opNULL
//////////////////////////////////////////////////////////////////////
Operands CSynan::FindVariable (string id) {
  vector<string>::iterator  theIt = m_vecVariables.begin();
  while (theIt != m_vecVariables.end()) {
    if (*theIt == id) {
      return opVar;
    }
  }

  return opNULL;
}

//////////////////////////////////////////////////////////////////////
// Otestuje spravnou syntaxi instrukce
//////////////////////////////////////////////////////////////////////
void CSynan::TestInstructionSyntax (tagINSTRUCTION instr) {
  vector<tagINSTRUCTION>::iterator theIt = m_vecInstMap.begin();
  int i;

  while (theIt != m_vecInstMap.end()) {
    if ( ( (*theIt).Instr == instr.Instr) && ( (*theIt).nOperands == instr.nOperands)) {
      if (instr.nOperands == 0)
        return;
      for (i = 0; i < instr.nOperands; i++) {
        if (instr.Operand[i].Type == (*theIt).Operand[i].Type) {
          if (i == (instr.nOperands - 1))
            return;
        } else {
          break;
        }
      }
    }
    ++theIt;
  }

  throw CSyntaxError (this, "Chybny operand intrukce");
}

//////////////////////////////////////////////////////////////////////
// Otestuje, jestli se navesti nachazi v seznamu navesti, pokud ne,
// tak ho prida a inkrementuje pocet uziti.
//////////////////////////////////////////////////////////////////////
void CSynan::AddLabelUse (string name) {
  if (name[0] != '_' || (name[1] != 'P' && name[1] != 'F'))
    return;
  vector<tagLABEL>::iterator  theIt = m_vecLabels.begin();

  while (theIt != m_vecLabels.end()) {
    if (theIt->strName == name) { // navesti nalezeno
      ++ (theIt->nUsed); // zvisim pocet uziti o jednicku
      AddLabelToActual (theIt); // a pridam ho k seznamu
      return;

    }
    ++theIt;
  }

  // navesti nebylo nalezeno
  // musim ho pridat
  tagLABEL  label;
  label.strName = name;
  label.nUsed = 1;
  label.lDefLine = -1;
  label.vecCalling.erase (label.vecCalling.begin(), label.vecCalling.end());

  m_vecLabels.push_back (label);
}

//////////////////////////////////////////////////////////////////////
// Otestuje, jestli se navesti nachazi v seznamu navesti, pokud ne,
// tak ho prida a nastavi cislo radeku, kde je definovano
//////////////////////////////////////////////////////////////////////
void CSynan::AddLabel (string name, long line) {
  if (name[0] != '_' || (name[1] != 'P' && name[1] != 'F'))
    return;

  vector<tagLABEL>::iterator  theIt = m_vecLabels.begin();

  while (theIt != m_vecLabels.end()) {
    if (theIt->strName == name) { // navesti nalezeno
      if (theIt->lDefLine >= 0)
        throw CSyntaxError (this, "Redefinice navesti");
      theIt->lDefLine = line;
      m_strActualLabel = theIt->strName;  // nastaveni aktualniho navesti
      return;

    }
    ++theIt;
  }

  // navesti nebylo nalezeno
  // musim ho pridat
  tagLABEL  label;
  label.strName = name;
  label.nUsed = 0;
  label.lDefLine = line;
  label.vecCalling.erase (label.vecCalling.begin(), label.vecCalling.end());

  m_vecLabels.push_back (label);
  m_strActualLabel = (m_vecLabels.end() - 1)->strName;  // nastaveni aktualniho navesti
}

//////////////////////////////////////////////////////////////////////
// Otestuje, jestli se navesti nachazi v seznamu navesti, pokud ne,
// tak ho prida
//////////////////////////////////////////////////////////////////////
void CSynan::AddLabelToActual (vector<tagLABEL>::iterator label) {
  if (m_strActualLabel.empty())
    return; // jeste neexistuje aktualni navesti

  // najdu odkaz na aktualni navesti
  vector<tagLABEL>::iterator  theIt = m_vecLabels.begin();
  while (theIt != m_vecLabels.end()) {
    if (m_strActualLabel == theIt->strName)
      break;
    ++theIt;
  }

  vector<string>::iterator  theCallIt = theIt->vecCalling.begin();

  while (theCallIt != theIt->vecCalling.end()) {
    if ( (*theCallIt) == label->strName) { // navesti nalezeno
      return;
    }
    ++theCallIt;
  }

  theIt->vecCalling.push_back (label->strName);
}

//////////////////////////////////////////////////////////////////////
// Provede optimalizaci - odstrani ze seznamu nepotrebne navesti
//////////////////////////////////////////////////////////////////////
void CSynan::Optimize() {
  vector<tagLABEL>::iterator      itLabels = m_vecLabels.begin();
  vector<tagLABEL>::iterator      itCalled = m_vecLabels.begin();
  vector<string>::iterator      itCalling;
  stack<vector<tagLABEL>::iterator> stackErased;
  bool  bChanged = true;

  while (bChanged) { // zkousim to, dokud je zmena
    bChanged = false;
    itLabels = m_vecLabels.begin();
    while (itLabels != m_vecLabels.end()) {
      if (itLabels->nUsed <= 0) {
        bChanged = true;
        // najdu procedury, ktere vola a snizim u nich pocet volani o 1
        itCalling = itLabels->vecCalling.begin();
        while (itCalling != itLabels->vecCalling.end()) {
          if (FindLabelByName (itCalled, *itCalling)) {
            --itCalled->nUsed;
          }
          ++itCalling;
        }
        // odstranim polozku
        stackErased.push (itLabels);
        m_vecNotUsedLabels.push_back (*itLabels);
      }
      ++itLabels;
    }

    while (!stackErased.empty())
      m_vecLabels.erase (stackErased.top()), stackErased.pop();

  }
}

//////////////////////////////////////////////////////////////////////
// Nalezne navesti podle jmena a vrati na nej odkaz, jinak vraci end()
//////////////////////////////////////////////////////////////////////
bool CSynan::FindLabelByName (vector<tagLABEL>::iterator & itLabels, string name) {
  itLabels = m_vecLabels.begin();
  while (itLabels != m_vecLabels.end()) {
    if (itLabels->strName == name)
      return true;
    ++itLabels;
  }

  return false;
}

//////////////////////////////////////////////////////////////////////
// Testuje,jestli se jedna o FW funkci RET
//////////////////////////////////////////////////////////////////////
bool CSynan::IsRET (int addr) {
  switch (addr) {
    case  0x42A :// FPRETW
    case  0x42D :// FPRETR
    case  0x430 :// FRETW
    case  0x433 :// FRETW
    case  0x436 :// PRET
      return true;
    default :
      return false;
  }
}
