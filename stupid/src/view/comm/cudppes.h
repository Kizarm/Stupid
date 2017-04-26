#ifndef CUDPPES_H
#define CUDPPES_H

#include "wintypes.h"
#include "PesNet.h"

class  MainWindow;

class CUDPPes : public CPesNet {
  public:
    CUDPPes();
    virtual ~CUDPPes();
    virtual bool ReadCodeBlock (unsigned char* XBuffer, WORD Address, WORD Size);
    virtual BYTE DownloadBlock (WORD Address, WORD Size, unsigned char* Data);
    virtual bool MakeCmd (CmType Cm);
    virtual bool SendCmd (CmType Cm);
    virtual void SendToken();
    virtual BYTE EEWrite (WORD Address, BYTE Dato);
    virtual BYTE EERead (WORD Address, BYTE& Dato);
    virtual bool FastToken();
    virtual DWORD BaudScan();
    virtual void SetBaudRate (DWORD Baud);
    virtual bool NetBreak();
    virtual void COM_Close();
    virtual void COM_Init (int nVerify);
    // jen test
    bool GetStpVersion (char * buf);
    bool GetNetVersion (char * buf);
    bool GetPioName    (char * buf);
};

#endif // CUDPPES_H
