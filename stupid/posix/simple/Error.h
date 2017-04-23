// Error.h: Trida pro chybovou vyjimku.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERROR_H__A950CE30_6855_4A9E_8678_9AF6F2EB67E1__INCLUDED_)
#define AFX_ERROR_H__A950CE30_6855_4A9E_8678_9AF6F2EB67E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <sstream>

using namespace std;

class CError {
  protected:
    string  m_strMessage; // obsahuje tet chyby
  public:
    CError (const string msg) {
      if (msg.empty())
        m_strMessage = "unknow";
      else
        m_strMessage = "Error : " + msg;
    };
    CError (const char * msg) {
      if (msg == NULL)
        m_strMessage = "unknow";
      else {
        m_strMessage = "Error : ";
        m_strMessage += msg;
      }
    };
    virtual ~CError() {};
    const char * GetError() { // vraci retezec s chybou
      return  m_strMessage.c_str();
    };

};

class CFileError : public CError {
  public:
    CFileError (const string msg) :
      CError ("") {
      if (msg.empty())
        m_strMessage = "unknow";
      else {
        m_strMessage = "\nError : Soubor " + msg;
        m_strMessage += " nelze otevrit.\n";

      }
    };
    CFileError (const char * msg) :
      CError ("") {
      if (msg == NULL)
        m_strMessage = "unknow";
      else {
        stringstream  out;
        out << endl << "Chyba : Soubor " << msg << " nelze otevrit." << endl;
        m_strMessage  = out.str();
      }
    };
    virtual ~CFileError() {};
};

class CSyntaxError : public CError {
  public:
    CSyntaxError (CLexem * lex, const char * msg) :
      CError ("") {
      char out[254];
      sprintf (out, "\nChybna syntaxe na radku %d : %s.\n", lex->GetLine(), msg);
      m_strMessage = out;
    };
    virtual ~CSyntaxError() {};
};
#endif // !defined(AFX_ERROR_H__A950CE30_6855_4A9E_8678_9AF6F2EB67E1__INCLUDED_)
