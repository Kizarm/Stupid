#ifndef _LOADERDLG_H__
#define _LOADERDLG_H__

#include "wintypes.h"
/////////////////////////////////////////////////////////////////////////////
// CLoaderDlg dialog
class CPesNet;
//! ???
class StationDescriptor {
  public:
    BYTE stanice;
    char * dnlname;
    char * typ;
  public:
    StationDescriptor () {};
};
#include <string>

class CLoaderDlg {
// Construction
  public:
    CLoaderDlg  (); // standard constructor
    void Status   (const char * pszFormat, ...);
    bool Download (const char * filename);
    bool Verify   (const char * filename);
    StationDescriptor * Station;

// Implementation
  protected:
    bool Download ();
    bool GetStatus();
    bool SetAutoRun();  // nastaveni rezimu autorun
    //bool ReadPassword (string & psw);
    bool IsPassword();
    bool Lock (bool l);
    bool SaveInfo();
    bool Verifi();
    bool Verifi (unsigned int Offset, unsigned int Length);
    bool InitPES();
    bool GetPESInfo();
    bool ReadDNLFile();
    bool Reboot();
    bool ReadPassword (std::string & psw);
  private:
    int m_nDownloadTime;
    unsigned long m_nDataSize;
    unsigned long m_nDataBegin;
    unsigned long m_nBdRate;
    CPesNet * m_PES;
    bool m_bGo;
    int  m_nBlockSize;
    BYTE m_nPageBegin;
    unsigned int   m_nStartAddress;
    BYTE   m_nVirtualPageStart;
    bool  m_bNoLock;  // informace, zda automat podporuje zamek pameti
    
    bool m_bSTOP;
    std::string m_strFilename;

    //string m_csPassword;
    bool  m_bPassword;
    bool m_bAutoUnlock;
    bool m_bAutoLock;
    bool m_bRegistred;
    bool m_bOnlyVerifi;
    bool m_bVerify;
    unsigned long m_nCodeLen;
    BYTE * m_nBlok;
    BYTE m_nStAddr;
    BYTE m_nCom;
    
};

#endif // _LOADERDLG_H__
