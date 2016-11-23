//////////////////////////////////////////////////////////////////////
//
// Soubor : USBPES.H
// Popis  : Deklarace tridy pro komunikaci v siti PesNet pre USB prevodnik
// Vytvoril : Martin Kucera / 02/03/2006
// Historie :
//
//
// Copyright (c) 2006 by Martin Kucera
// All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBPES_H__INCLUDED_)
#define AFX_USBPES_H__INCLUDED_

#include "wintypes.h"
#include "PesNet.h"

struct usb_dev_handle;
class  MainWindow;

// Definice ridicich prikazu pro USB prevodnik
#define USB_REQ_PESNET_GET_BAUDRATE     0xAC
#define USB_REQ_PESNET_READ_CODE_BLOCK  0xAD
#define USB_REQ_PESNET_DOWNLOAD_BLOCK   0xAE
#define USB_REQ_PESNET_COMMAND          0xAF
#define USB_REQ_PESNET_RESET            0xA0
#define USB_REQ_PESNET_BREAK            0xAB
#define USB_REQ_PESNET_SET_BAUDRATE     0xAA

class /*AFX_EXT_CLASS*/ CUSBPes: public CPesNet {
  public:
    void USBReset();
    void COM_Init (int nVerify);    // inicializuje komunikacni port
    void COM_Close();               // uzavira komunikacni port
    bool NetBreak();                // schodi sit
    void SetBaudRate (DWORD Baud);  // nastavi komunikacni rychlost
    DWORD BaudScan();               // detekuje prenosovou rychlost
    void NetScan (void);            // detekuje stanice v siti
    bool FastToken();
    BYTE EERead (WORD Address, BYTE & Dato);
    BYTE EEWrite (WORD Address, BYTE Dato);

    /* Prace s zpravamy */
    void SendToken();
    bool SendCmd (CmType Cm);         // posle prikaz Cm
    bool MakeCmd (CmType Cm);         // posle prikaz Cm

    /* Dalsi pomocne fce*/
    void SetOneCharTime();

    void  UserWD  (int64_t * wd);       // inicializace watchdog-u
    bool  UserHaf (int64_t   wd, int _timeout_ms); // test watchdog-u

    BYTE DownloadBlock (WORD Address, WORD Size, unsigned char * Data); // Provede download bloku spolu s verifikaci
    BYTE DownloadBlock64Max (WORD Address, WORD Size, unsigned char * Data); // Provede download bloku spolu s verifikaci
    BYTE DownloadBlockDirect (WORD Address, WORD Size, unsigned char * Data); // Provede download bloku - pri primem pristupu na sit
    bool ReadCodeBlock (unsigned char * XBuffer, WORD Address, WORD Size);
    bool ReadCodeBlockDirect (unsigned char * XBuffer, WORD Address, WORD Size);


    CUSBPes (/*MainWindow * p*/);   // konstruktor
    virtual ~CUSBPes();         // destruktor

    bool m_bDirectMode;
    // added to Linux
  public:
    usb_dev_handle * udev;
    //bool WriteByte (BYTE * nBytes, BYTE nPocet); // posle nPocet bytu na seriovy port
    //bool ReadByte  (BYTE * nBytes, BYTE nPocet); // precte n_Pocet bytu ze serioveho port

    bool m_bScan;     // udava jestli byla spustena fce NetScan
    BYTE ReadCodeBlock64 (unsigned char * XBuffer, WORD Address, WORD Size);

};

#endif // !defined(AFX_USBPES_H__INCLUDED_)
