// Error.h: Trida pro chybovou vyjimku.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ERROR_H__A950CE30_6855_4A9E_8678_9AF6F2EB67E1__INCLUDED_)
#define AFX_ERROR_H__A950CE30_6855_4A9E_8678_9AF6F2EB67E1__INCLUDED_
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
#include <string>
#include <sstream>

using namespace std;

class CError {
  protected:
    string  m_strMessage; // obsahuje tet chyby
    int   m_nCode;    // kod chyby -- pro navaznost na HypEd
  public:
    CError (const string msg) :m_nCode (-1) {
      if (msg.empty())
        m_strMessage = "unknow";
      else
        m_strMessage = "\nVazna chyba : " + msg;
    };
    CError (const char * msg) :m_nCode (-1) {
      if (msg == NULL)
        m_strMessage = "\nunknow";
      else {
        m_strMessage = "\nVazna chyba : ";
        m_strMessage += msg;
      }
    };
    CError (const char * msg, int error) :m_nCode (error) {
      if (msg == NULL)
        m_strMessage = "\nunknow";
      else {
        m_strMessage = "\nVazna chyba : ";
        m_strMessage += msg;
      }
    };
    virtual ~CError() {};
    const char * GetError() { // vraci retezec s chybou
      return  m_strMessage.c_str();
    };

    const int GetCode() { // vraci kod chyby
      return m_nCode;
    };
};

class CFileError : public CError {
  public:
    CFileError (const string msg) :
      CError ("") {
      m_nCode = -1;
      if (msg.empty())
        m_strMessage = "unknow";
      else {
        m_strMessage = "\nNelze otevrit soubor " + msg;
      }
    };
    CFileError (const char * msg) :
      CError ("") {
      m_nCode = -1;
      if (msg == NULL)
        m_strMessage = "unknow";
      else {
        stringstream  out;
        out << "\nNelze otevrit soubor " << msg << " ." << endl;
        m_strMessage  = out.str();
      }
    };
    virtual ~CFileError() {};
};

class CSyntaxError : public CError {
  public:
    CSyntaxError (int line, const char * msg) :
      CError ("") {
      m_nCode = 1;
      stringstream  out;
      out << "\nChyba na radku " << line << ": " << msg << "." << endl;
      m_strMessage = out.str();
    };
    virtual ~CSyntaxError() {};
};

class CSyntMissErr : public CError {
  public:
    CSyntMissErr (int line, const char * msg) :
      CError ("") {
      m_nCode = 1;
      stringstream  out;
      out << "\nChyba na radku " << line << ": ocekavam " << msg << "." << endl;
      m_strMessage = out.str();
    };
    virtual ~CSyntMissErr() {};
};

#endif // !defined(AFX_ERROR_H__A950CE30_6855_4A9E_8678_9AF6F2EB67E1__INCLUDED_)
