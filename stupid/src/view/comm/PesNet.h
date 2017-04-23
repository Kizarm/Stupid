//////////////////////////////////////////////////////////////////////
//
// Soubor : PESNET.H
// Popis  : Interface tridy pro komunikaci v siti PesNet
// Vytvoril : Martin Kucera / 02/03/2006
// Historie :
//
//
// Copyright (c) 2006 by Martin Kucera
// All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PESNET_H__INCLUDED_)
#define AFX_PESNET_H__INCLUDED_

#include "wintypes.h"
/*
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
typedef struct {
  unsigned char Cm;
  unsigned char Tx;
  unsigned char Rx;
} CmType;

#define RTS_9600  1876
#define RTS_19200 950
#define RTS_57600 220

enum StupidStates {
  StupidReset = 130,
  StupidStep,
  StupidStop,
  StupidRun,
};
class ViewDialog;

/**
 * @class CPesNet
 * @brief Základní komunikace po PESNET.
 * 
 * Celkem převzato ze starých zdrojáků.
 * */

class CPesNet {
  public:
    // promenne
    //
    bool  Success;      // vraci stav pro posledni operaci se siti
    //   -  true ... posledni operace se siti se zdarila
    //   -  false ... nastala chyba pri posledni operaci se siti
    BYTE  COM_Error;          // cislo chyby
    BYTE  IOData     [10];    // IObuffer
    BYTE  StationList[32];    // seznam dostupnych stanic
    BYTE  StationNum;         // pocet detekovanych stanic
    BYTE  ComPort;
    unsigned long BaudRate;   // prenosova rychlost
    BYTE  AddrTarget;         // cislo stanice
    
    bool  COM_Disable;        // stav otevreni portu
    bool  COM_Lock;           // stav uzamceni portu
    // funkce
    //
    CPesNet (/*MainWindow * p*/);
    virtual ~CPesNet() {};
    virtual void COM_Init (int nVerify)   = 0;  // inicializuje komunikacni port
    virtual void COM_Close()              = 0;  // uzavira komunikacni port
    virtual bool NetBreak()               = 0;  // schodi sit
    virtual void SetBaudRate (DWORD Baud) = 0;  // nastavi komunikacni rychlost
    virtual DWORD BaudScan()              = 0;  // detekuje prenosovou rychlost
    virtual bool FastToken()              = 0;
    virtual BYTE EERead (WORD Address, BYTE & Dato) = 0;
    virtual BYTE EEWrite (WORD Address, BYTE Dato)  = 0;
    
    virtual char * GetErrorString (char * ErrString);
    virtual bool ReadEEBlock (unsigned char * XBuffer, unsigned char Offset, unsigned int Size);

    
    virtual bool SelectStation (BYTE StAddr); // nastavi cislo stanice
    virtual void NetScan (void);              // detekuje stanice v siti
    /* STP R&W */
    virtual BYTE ReadSTPBit   (unsigned int Addr);
    virtual bool WriteSTPBit  (unsigned int Addr, unsigned char Bit);
    virtual WORD ReadSTPWord  (unsigned int Addr);
    virtual bool WriteSTPWord (unsigned int Addr, unsigned int Dato);

    /* Xdata R&W */
    virtual bool WriteXBit   (unsigned int Addr, unsigned char Bit, bool Set);
    virtual bool WriteXByte  (unsigned int Addr, unsigned char Dato);
    virtual BYTE ReadXByte   (unsigned int Addr);
    virtual bool ReadXWord   (unsigned int Addr, WORD * Dato);
    virtual bool WriteXWord  (unsigned int Addr, WORD Dato);
    virtual bool ReadXDWord  (unsigned int Addr, DWORD * Dato);
    virtual bool WriteXDWord (unsigned int Addr, DWORD Dato);
    virtual bool WriteXBlock (unsigned char * XBuffer, unsigned int Offset, unsigned int Size);
    virtual bool ReadXBlock  (unsigned char * XBuffer, unsigned int Offset, unsigned int Size);
    virtual bool ReadBlock   (unsigned char * XBuffer, WORD Offset, WORD Size);

    /* Prace s zpravamy */
    virtual bool ReliableSend (CmType Cmd);
    
    virtual void SendToken()         = 0;
    virtual bool SendCmd (CmType Cm) = 0;         // posle prikaz Cm
    virtual bool MakeCmd (CmType Cm) = 0;         // posle prikaz Cm

    /* Dalsi pomocne fce*/
    virtual bool ReadEEByte  (WORD Address, BYTE & Dato);
    virtual BYTE WriteEEByte (WORD Address, BYTE   Dato);

    virtual char * GetDataMemType (BYTE Type, char * Info); // vraci retezec identifikujici typ datove pameti
    virtual char * GetProgMemType (BYTE Type, char * Info); // vraci retezec identifikujici typ programove pameti

    virtual BYTE DownloadBlock (WORD Address, WORD Size, unsigned char * Data)    = 0; // Provede download bloku spolu s verifikaci
    virtual bool ReadCodeBlock (unsigned char * XBuffer, WORD Address, WORD Size) = 0;

    virtual void  UserWD  (int64_t * wd)                  ; // inicializace watchdog-u
    virtual bool  UserHaf (int64_t   wd, int _timeout_ms) ; // test watchdog-u
    virtual void InitWaitFunc();                            // inicializuje timer
    virtual void WaitShortTime (int64_t _timeout_mks);
    
    // added to Linux Viewer
    virtual bool SetStupidState (StupidStates st);
    virtual bool GetNetVersion (char * buf);
    virtual bool GetStpVersion (char * buf);
    virtual bool GetPioName    (char * buf);
    virtual bool GetStatus     (BYTE & sta);
    virtual bool GetDownCount  (WORD & cnt);
    virtual bool GetLastDown   (char * buf, int len);
    virtual bool GetLockState  (char * buf);
    
    bool isDisabled (void)    {return COM_Disable;  };
    bool isSuccsess (void)    {return Success;      };
  /// Qt specific
  public:
    void SetParent (ViewDialog * w) {parentWidget = w;};
    ViewDialog * GetParent (void) {return parentWidget;};
  private:
    ViewDialog * parentWidget;
};

#endif // !defined(AFX_PESNET_H__INCLUDED_)
