//////////////////////////////////////////////////////////////////////
//
// Soubor : USPPes.CPP
//
// Popis  : Implementace tridy pro komunikaci v siti PesNet
//              pomoci USB485
//
// Vytvoril : Martin Kucera
//
//
// Historie : 24/04/2005  - vytvoreno
//
//////////////////////////////////////////////////////////////////////
//#define UNIX
#include <stdio.h>
#include "USBPes.h"
#include "PesDefines.h"
#include "PesNet.h"
#include "platform.h"
#ifdef UNIX
#include "usb.h"
#else
#include "lusb0_usb.h"
#endif // UNIX

#define VENDOR_ID         0xA051
#define PRODUCT_ID        0x0485

#define TRACE(...)
//#define TRACE printf

#define CLAIM_INTERFACE()                 \
  int ret = usb_claim_interface(udev, 0); \
  if (ret != 0)                           \
  {                                       \
    TRACE("%s", usb_strerror());          \
    Success = false;                      \
    return Success;                       \
  }                                       \
   
#define RELEASE_INTERFACE()               \
  usb_release_interface(udev, 0);         \
   
int  g_TransferTimeout  = 100; // in msecs

char g_nInstanceCounter = 0;
usb_dev_handle * g_udev;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CUSBPes::CUSBPes (/*MainWindow * p*/) : CPesNet (/*p*/) {
  // Inicializace promennych
  //
  m_bDirectMode = false;
}

CUSBPes::~CUSBPes() {
  //COM_Close();
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : COM_Init
//  Popis   : Inicializuje komunikacni port
//  Parametry : nic
//////////////////////////////////////////////////////////////////////
void CUSBPes::COM_Init (int  nVerify) {
  Success = false;

  TRACE ("COM_Init\n");
  if (nVerify != 456786)
    return ;

  // Vyhledani a otevreni USB zarizeni
  //
  if (g_udev != NULL) {
    udev = g_udev;
    Success = true;
    COM_Disable = false;
    ++g_nInstanceCounter;
    return;
  }

  struct  usb_bus * bus;
  struct  usb_device * dev;
  usb_init();

  usb_find_busses();
  usb_find_devices();

  bool  usb_finded = false;
  for (bus = usb_get_busses(); bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      udev = usb_open (dev);
      if (udev) {
        if ( (dev->descriptor.idVendor == VENDOR_ID)
             && (dev->descriptor.idProduct == PRODUCT_ID)) {
	  printf ("Nalezeno USB device\n");
#if 1 // Ubuntu 16.04 opravneni v udev rules !!!
          if (usb_set_configuration (udev, 2) != 0) {
	    printf ("Nelze nastavit CFG USB device\n");
            Success     = false;
            COM_Disable = true;
            return;
          }
#endif
          usb_finded = true;
          break;
        } else {
          usb_close (udev);
        }
      }
    }
    if (usb_finded)
      break;
  }

  if (usb_finded) {
    InitWaitFunc();
    TRACE ("COM_Init SUCESS\n");

    COM_Disable = false;
    Success     = true;
    ++g_nInstanceCounter;
  }
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : COM_Close
//  Popis   : Zavira komunikacni port
//  Parametry : nic
//////////////////////////////////////////////////////////////////////
void CUSBPes::COM_Close() {
  if (COM_Disable) return;

  --g_nInstanceCounter;
  if (g_nInstanceCounter == 0) {
    printf ("USB Port uzavren\n");
    usb_close (udev);
    TRACE ("CUSBPes::COM_Close()\n");
    udev = NULL;
    g_udev = NULL;
  }

  COM_Disable = true;
}


//////////////////////////////////////////////////////////////////////
//  Nazev   : NetBreak
//  Popis   : Zastavi sit
//  Parametry : nic
//////////////////////////////////////////////////////////////////////
bool CUSBPes::NetBreak() {
  TRACE ("\nCUSBPes::NetBreak() - disfunct\n");
  return true; // nefunguje ctrl
  if (m_bDirectMode) {
    int ret = usb_claim_interface (udev, 0);
    if (ret != 0) {
      TRACE ("NetBreak-claim%s", usb_strerror());
    }

    ret = usb_control_msg (udev, 0x41, USB_REQ_PESNET_BREAK, 0, 0, NULL, 0, g_TransferTimeout);
    if (ret != 0) {
      TRACE ("NetBreak-ctrl-%s", usb_strerror());
    }

    usb_release_interface (udev,0);
  }
  // pro USB prevodnik nedela nic, protoze sit se nezastavuje
  return true;
}
//////////////////////////////////////////////////////////////////////
//  Nazev   : SendCmd
//  Popis   : Posle sitovi prikaz
//  Parametry : Cm - prikaz k odeslani
//////////////////////////////////////////////////////////////////////
bool CUSBPes::SendCmd (CmType Cm) {
  // funkce UserHaf, UserWD neumim spravne prepsat v linuxu, to co potrebuji chodi i bez toho.
  BYTE  TmpIO[2];
  //bool  Success = false;
  int64_t  wd;

  if (Cm.Cm >= 0x80) {
    TmpIO[0] = IOData[0];
    TmpIO[1] = IOData[1];
    UserWD (&wd);
    do {
      MakeCmd (Cm_GetStatus);
      if (!UserHaf (wd, 500)) {
        TRACE ("\n#ERROR#\tcmd0 %d, status = %d\n", Cm.Cm, IOData[0]);
        COM_Error = ERR_IOFULL;
        return false;
      }
    } while ( (IOData[0] & ST_INBUSY) != 0);
    IOData[0] = TmpIO[0];
    IOData[1] = TmpIO[1];
  }

  if (Cm.Cm == 0x16) {
    TmpIO[0] = IOData[0];
    TmpIO[1] = IOData[1];
    UserWD (&wd);
    do {
      MakeCmd (Cm_GetStatus);
      if (!UserHaf (wd, 500)) {
        TRACE ("\n#ERROR#\tcmd1 %d, status = %d\n", Cm.Cm, IOData[0]);
        COM_Error = ERR_IOFULL;
        return false;
      }
    } while ( (IOData[0] & ST_OUTREADY) == 0);
    IOData[0] = TmpIO[0];
    IOData[1] = TmpIO[1];
  }
  return MakeCmd (Cm);
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : MakeCmd
//  Popis   : Posle sitovi prikaz
//  Parametry : Cm - prikaz k odeslani
//////////////////////////////////////////////////////////////////////
bool CUSBPes::MakeCmd (CmType Cm) {
  BYTE  FrameSum;
  BYTE  Dato[8];
  BYTE  i,j;
  //DWORD dwMask = 0;
  BYTE  Buffer[10], ResponseLen;

  Success = false;

  if (COM_Disable) {
    COM_Error = ERR_INITCOM;
    return Success;
  }

  FrameSum = 0;

  COM_Error = ERR_NONE;
  COM_Lock = false;

  // Sestavim ramec
  //
  j = 0;
  // ramec bude CMDF + adresa
  Dato[j] = HD_CMDF | AddrTarget;
  FrameSum = Dato[j];
  j++;
  // COMMAND
  Dato[j] = Cm.Cm;
  FrameSum += Dato[j];
  j++;
  // tady jsou pripadne parametry
  for (i = 0; i < Cm.Tx; i++) {
    Dato[j] = IOData[i];
    j++;
    FrameSum += IOData[i];
  }
  // kontrolni soucet
  Dato[j]= FrameSum;

  // nastaveni poctu bytu odezvy
  if (Cm.Cm >= 0x80)    // indirect command
    ResponseLen = 0;
  else if (Cm.Rx == 0)  // ramec s error
    ResponseLen = 2;
  else if (Cm.Rx == 0xFF) // GetIOBuffer
    ResponseLen = 2;
  else          // normalni ramec + CRC
    ResponseLen = Cm.Rx + 1;

  CLAIM_INTERFACE()

  // odeslani prikazu
  ret = usb_control_msg (udev,
                         0x41,     // endpoint
                         0xAF,     // typ zadosti
                         ResponseLen,  // delka odezvy
                         0,        // index
                         (char *) Dato, // zprava
                         Cm.Tx + 3,    // delka zpravy
                         100);     // timeout
  if (ret < 0) {
//    TRACE("%s", usb_strerror());
    Success = false;
    RELEASE_INTERFACE()
    return Success;
  }

  // Zpracovani odezvy
  //

  // globalni adresa je bez odezvy, krome GetACK!
  if (AddrTarget == 31 && Cm.Cm != 0x01) {
    RELEASE_INTERFACE()
    Success = true;
    return Success;
  }

  // indirect CMD bez odezvy - zatim !
  if (Cm.Cm >= 0x80) {
    // cekam na odezvu
    ret = usb_interrupt_read (udev, 0x82, (char *) IOData, 1, g_TransferTimeout);
    if (ret != 1 && IOData[0] != 0x5A) {
//      TRACE ("MakeCmd - %s\n", usb_strerror());
      Success  = false;
      RELEASE_INTERFACE()
      return Success;
    }
    RELEASE_INTERFACE()
    Success  = true;
    return Success;
  }

  // ramec s ERROR
  if (Cm.Rx == 0) {
    ret = usb_interrupt_read (udev, 0x81, (char *) Buffer, 2, g_TransferTimeout);
    if (ret != 2) {
//      TRACE("%s\n", usb_strerror());
      Success  = false;
      RELEASE_INTERFACE()
      return Success;
    }

    if (Buffer[0] != Buffer[1]) {
//      TRACE("%d,%d\n",IOData[0],IOData[1]);
      COM_Error = ERR_RXSUM;
      Success = false;
      RELEASE_INTERFACE()
      return Success;
    }

    if ( (Buffer[0]) && (COM_Error == ERR_NONE)) {
      if (COM_Error != 0x80) {
        RELEASE_INTERFACE()
        Success = false;
        return Success;
      }
    }
  }
  // nebo ramec s DATY + SUM
  else {
    if (Cm.Rx != 0xFF) {
      // odezva na ramec
      ret = usb_interrupt_read (udev, 0x81, (char *) IOData, Cm.Rx + 1, g_TransferTimeout);
      if (ret != Cm.Rx + 1) {
//        TRACE ("2.MakeCmd-%s\n", usb_strerror());
        COM_Error = ERR_TIMEOUTRX;
        Success  = false;
        RELEASE_INTERFACE()
        return Success;
      }

      FrameSum = 0;
      for (i = 0; i < Cm.Rx; i++) {
        FrameSum += IOData[i];
      }

      if (FrameSum != IOData[Cm.Rx]) {
        COM_Error = ERR_RXSUM;
        Success = false;
        RELEASE_INTERFACE()
        return Success;
      }
    } else {
      // odezva na Cm_GetIOBuffer
      ret = usb_interrupt_read (udev, 0x81, (char *) IOData, 2, g_TransferTimeout);
      if (ret != 2) {
        TRACE ("3.MakeCmd-%s\n", usb_strerror());
        COM_Error = ERR_TIMEOUTRX;
        Success  = false;
        RELEASE_INTERFACE()
        return Success;
      }

      if (IOData[0] != IOData[1]) {
        COM_Error = ERR_RXSUM;
        Success = false;
        RELEASE_INTERFACE()
        return Success;
      }
    }

  }

  Success = true;

  RELEASE_INTERFACE()
  return Success;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : SetBaudRate
//  Popis   : Nastavi komunikacni rychlost
//  Parametry : Baud - komunikacni rychlost
//////////////////////////////////////////////////////////////////////
void CUSBPes::SetBaudRate (DWORD Baud) {
  // u USB prevodniku se nenastavuje, protoze je automaticky detekovana
  TRACE ("#Info#\tNastavena prenosova rychlost : %dBd - nefunguje\n", Baud);
  BaudRate = Baud;
  Success = true;
  return;

  if (m_bDirectMode) {
    BYTE  Dato[8];
    char  buffer[8];
    int   ret;

    Success = true;
    // Prevod kodu pro prenosovou rychlost
    switch (Baud) {
      case  9600  :
        Dato[0] = 2;
        break;
      case  19200 :
        Dato[0] = 1;
        break;
      case  57600 :
      default     :
        Dato[0] = 0;
        break;
    }

    // odeslani ridiciho prikazu
    TRACE ("interface - claim (Line %d)\n",__LINE__);
    ret = usb_claim_interface (udev, 0);
    if (ret != 0) {
      TRACE ("%s", usb_strerror());
      Success = false;
      return;
    }

    ret = usb_control_msg (udev,
                           0x41,             // endpoint
                           USB_REQ_PESNET_SET_BAUDRATE,  // typ zadosti
                           0,                // delka odezvy
                           0,                // index
                           (char *) Dato,        // zprava
                           1,                // delka zpravy
                           50);              // timeout
    if (ret < 0) {
      TRACE ("SetBaudRate-ctrl-%s\n", usb_strerror());
      RELEASE_INTERFACE()
      Success = false;
      return;
    }
    // Zpracovani odezvy
    //
    ret = usb_interrupt_read (udev, 0x82, buffer, 1, g_TransferTimeout);
    if (ret != 1 || buffer[0] != Dato[0]) {
      TRACE ("etBaudRate-%s\n", usb_strerror());
      Success = false;
    }
    RELEASE_INTERFACE()
  }
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : BaudScan
//  Popis   : detekuje komunikacni rychlost
//  Parametry : nic
//  Vraci   : detekovanou prenosovou rychlost
//////////////////////////////////////////////////////////////////////
DWORD CUSBPes::BaudScan() {
  BYTE bcod = 0;
  // Nacteme z EEPROM zvolene stanice
  Success = ReadEEByte (EE_Baud, bcod);
  bcod = 0x100U - bcod;
  if (bcod) BaudRate = 57600 / bcod;
  //if (oldtgt != AddrTarget) SelectStation (oldtgt);
  return (DWORD) BaudRate;

  // Nasledujici kod nefunguje, ale mel by
  char  byte = 0;
  CLAIM_INTERFACE()
  ret = usb_control_msg (udev, 0x41, USB_REQ_PESNET_GET_BAUDRATE, 0, 0, NULL, 0, 100);
  if (ret < 0) {
    TRACE ("BaudScan - ctrl_msg - %s\n", usb_strerror());
    Success = false;
    RELEASE_INTERFACE()
    return 0;
  }
  // cekam na odezvu
  ret = usb_interrupt_read (udev, 0x82, &byte, 1, g_TransferTimeout);
  if (ret != 1) {
    TRACE ("BaudScan - intr_read - %s\n", usb_strerror());
    Success  = false;
    RELEASE_INTERFACE()
    return 0;
  }
  RELEASE_INTERFACE()

  switch (byte) {
    case  0 :
      BaudRate = 57600;
      break;
    case  1 :
      BaudRate = 19200;
      break;
    case  2 :
      BaudRate = 9600;
      break;
    default   :
      BaudRate = 0;
  }
  return (DWORD) BaudRate;
}


//////////////////////////////////////////////////////////////////////
//  Nazev   : NetScan
//  Popis   : prohleda vsechny adresy od 0 do 30, na polozku 0 do
//          StationList zapise pocet aktivnich stanic a na dalsi polozky postupne
//          zapise jejich adresy (ve vzestupnem poradi)
//  Parametry : nic
//////////////////////////////////////////////////////////////////////
void CUSBPes::NetScan (void) {

  BYTE st    = AddrTarget;  // ulozim cislo stanice
  StationNum = 0;

  switch (BaudRate) {
    case  57600 :
      g_TransferTimeout = 130;
      break;
    case  19200 :
      g_TransferTimeout = 160;
      break;
    case  9600  :
      g_TransferTimeout = 230;
      break;
    default     :
      g_TransferTimeout = 50;
  }
//  g_TransferTimeout = 200;//50;
  CPesNet::NetScan ();
  g_TransferTimeout = 500;//100;
  SelectStation (st); // obnovim puvodni cislo stanice

  COM_Error = ERR_NONE;
  Success = true;
  m_bScan = true;
}


void CUSBPes::SendToken() {
  // u USB prevodniku nema smysl
  Success = true;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : EEWrite
//  Popis   : Zapis bytu EEPROM
//  Parametry : Address - adesa v EEPROM
//                  Dato - hodnota pro zapis
//////////////////////////////////////////////////////////////////////
BYTE CUSBPes::EEWrite (WORD Address, BYTE Dato) {
  WORD  EEaddr;//, pom = 0;

  if (!WriteSTPWord (SFW_EELOCK, EELOCK_KEY))
    WriteSTPWord (SFW_EELOCK, EELOCK_KEY);

  if (Address == 0xFF) {
    TRACE ("Prepis bloku %d do EEPROM\n", Dato);
  }

  if (Address <= 0xFF) {
    EEaddr = 0xFF - Address;
  } else {
    EEaddr = Address;
  }

  IOData[0] = EEaddr / 256;
  IOData[1] = EEaddr % 256;
  IOData[2] = Dato;


  BYTE  FrameSum;
  char  Frame[8];
  //DWORD dwMask = 0;
  char  Buffer[10];

  // Sestavim ramec
  //
  Frame[0] = HD_CMDF | AddrTarget;
  Frame[1] = (char) 0x80;
  Frame[2] = EEaddr / 256;
  Frame[3] = EEaddr % 256;
  Frame[4] = Dato;
  FrameSum = IOData[0] + IOData[1] + IOData[2] + IOData[3] + IOData[4];
  // kontrolni soucet
  Frame[5]= FrameSum;

  CLAIM_INTERFACE()

  ret = usb_control_msg (udev, 0x41, 0xAF, 0, 0, Frame, 6, g_TransferTimeout);
  if (ret < 0) {
    TRACE ("%s\n", usb_strerror());
    RELEASE_INTERFACE()
    return 60;
  }

  WaitShortTime (100);
  // Zpracovani odezvy
  //
  ret = usb_interrupt_read (udev, 0x82, Buffer, 1, g_TransferTimeout);
  if (ret != 1) {
    TRACE ("%s\n", usb_strerror());
    RELEASE_INTERFACE()
    return 70;
  }

  RELEASE_INTERFACE()
  return Buffer[0];
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : EERead
//  Popis   : Cteni bytu EEPROM
//  Parametry : Address - adesa v EEPROM
//                  Dato - vyctena hodnota
//////////////////////////////////////////////////////////////////////
BYTE CUSBPes::EERead (WORD Address, BYTE & Dato) {
  WORD  EEaddr;//, pom = 0;

  if (Address <= 0xff)
    EEaddr = 0xff - Address;
  else
    EEaddr = Address;

  if (m_bDirectMode) {
    IOData[0] = EEaddr / 256;
    IOData[1] = EEaddr % 256;

    if (!SendCmd (Cm_IO_ReadEEByte))
      return 10;

    if (!SendCmd (Cm_GetIOBuf))
      return 20;

    Dato = IOData[0];

    return 0x5A;
  }

  BYTE  FrameSum;
  char  Frame[8];
  char  Buffer[10];

  // Sestavim ramec
  //
  Frame[0] = HD_CMDF | AddrTarget;
  Frame[1] = (char) 0x81;
  Frame[2] = EEaddr / 256;
  Frame[3] = EEaddr % 256;
  FrameSum = IOData[0] + IOData[1] + IOData[2] + IOData[3];
  // kontrolni soucet
  Frame[4]= FrameSum;

  // Ted to vsechno poslu
  //
  CLAIM_INTERFACE()

  ret = usb_control_msg (udev, 0x41, 0xAF, 2, 0, Frame, 5, g_TransferTimeout);
  if (ret < 0) {
    TRACE ("EERead - ctrl - %s\n", usb_strerror());
    RELEASE_INTERFACE()
    return 50;
  }

  WaitShortTime (100);
  // Zpracovani odezvy
  //
  ret = usb_interrupt_read (udev, 0x82, Buffer, 1, g_TransferTimeout);
  if (ret != 1) {
    TRACE ("EERead - 0x82 - %s\n", usb_strerror());
    RELEASE_INTERFACE()
    return 70;
  }
#if 0
  //if (Buffer[0] != 0x5A) TRACE ("\nEERead error %d", Buffer[0]);
#endif // 0
  ret = usb_interrupt_read (udev, 0x81, Buffer, 2, g_TransferTimeout);
  if (ret != 2) {
    TRACE ("EERead - 0x81 - %s\n", usb_strerror());
    RELEASE_INTERFACE()
    return 80;
  }
  if (Buffer[0] != Buffer[1]) {
    RELEASE_INTERFACE()
    return 110;
  }

  RELEASE_INTERFACE()
  Dato = Buffer[0];
  return 0x5A;
}
// USB prevodniku se netyka
bool CUSBPes::FastToken() {
  return true;
}
// Provede download bloku
// vraci 16 pri spravnem downloadu, jinak vraci cislo chyby
BYTE CUSBPes::DownloadBlock (WORD Address, WORD Size, unsigned char * Data) {
  TRACE ("Zapis bloku velikosti %dB na adresu %x\n", Size, Address);

  if (m_bDirectMode) {
    return DownloadBlockDirect (Address, Size, Data);
  }

  int i,blok, rep;

  i = 0;
  rep = 3;
  while (i < Size) {
    if (i + 64 < Size) {
      blok = 64;
    } else {
      blok = Size - i;
    }

    if (DownloadBlock64Max (Address + i, blok, Data + i) != 16) {
      if (rep--) {
        continue;
      } else {
        return 60;
      }
    }

    i += blok;
  }

  return 16;
}
// Provede download bloku
// vraci 16 pri spravnem downloadu, jinak vraci cislo chyby
// - pouze pro pri primem pristupu na sit!!!
BYTE CUSBPes::DownloadBlockDirect (WORD Address, WORD Size, unsigned char * Data) {
  WORD ErrCnt = 0, i, j;

  IOData[0] = Address / 256;
  IOData[1] = Address % 256;
  if (!ReliableSend (Cm_SetXPtr)) {
    TRACE ("DownloadBlock::SetXPtr ... chyba c.%d\n", COM_Error);
    return 10;
  }

  i = 0;
  while (i < Size) {
    // napln IOData 4B kodu
    for (j = 0; j < 4; j++) {
      IOData[j] = Data[i+j];
    }

    SendCmd (Cm_WriteXPDW); // posli je
    // jestli se to nepovedlo nastav ukazatel na posledn? poslany byte
    if (COM_Error != 0) {
      if (ErrCnt > 10) {
        TRACE ("DownloadBlock::WriteXPB ... prekroceno opakovani, chyba c.%d\n", COM_Error);
        return 20;
      }
      SendCmd (Cm_ClrError);
      ErrCnt++;
      IOData[0] = (Address + i) / 256;
      IOData[1] = (Address + i) % 256;
      if (!ReliableSend (Cm_SetXPtr)) {
        TRACE ("DownloadBlock::SetXPtr ... chyba c.%d\n", COM_Error);
        return 30;
      }
      continue;
    }

    i += 4;
  }

  return 16;
}

// Provede download bloku
// vraci 0x5A pri spravnem downloadu, jinak vraci cislo chyby
BYTE CUSBPes::DownloadBlock64Max (WORD Address, WORD Size, unsigned char * Data) {
  TRACE ("Zapis bloku velikosti %dB na adresu %x ... ", Size, Address);

  char  buffer[4];

  Success = false;

  if (COM_Disable) {
    COM_Error = ERR_INITCOM;
    TRACE ("chyba\n");
    return 10;
  }

  // sestaveni datoveho ramce pro OEP0
  buffer[0] = Address % 256;  // dolni byte adresy
  buffer[1] = Address / 256;  // horni byte adresy
  buffer[2] = Size % 256;     // velikost bloku, ktery se posle pres IEP1 (max. 64B)
  buffer[3] = AddrTarget;     // cislo stanice

  CLAIM_INTERFACE()

  usb_interrupt_read (udev, 0x82, buffer, 1, 20); //200

  // poslu prikaz zapisu bufferu
  ret = usb_control_msg (udev, 0x41, 0xAE, 0, 0, buffer, 4, g_TransferTimeout);

  if (ret < 0) {
    TRACE ("%s\n", usb_strerror());
    Success = false;
    RELEASE_INTERFACE()
    return 20;
  }

  // poslu buffer po OEP1
  ret = usb_interrupt_write (udev, 0x01, (char *) Data, Size, 100); //500

  if (ret != Size) {
    TRACE ("%s\n", usb_strerror());
    Success = false;
    RELEASE_INTERFACE()
    return 30;
  }

  // cekam na odezvu
  ret = usb_interrupt_read (udev, 0x82, buffer, 1, 2000); //2000
  if (ret != 1) {
    TRACE ("%s\n", usb_strerror());
    Success  = false;
    RELEASE_INTERFACE()
    return 40;
  }

  if (buffer[0] == 16) {
    TRACE ("OK\n");
  } else {
    TRACE ("chyba %d\n", buffer[0]);
  }
  RELEASE_INTERFACE()

  return buffer[0];
}

BYTE CUSBPes::ReadCodeBlock64 (unsigned char * XBuffer, WORD Address, WORD Size) {
  TRACE ("Cteni bloku velikosti %dB z adresy %x ... ", Size, Address);

  char  buffer[4];//, frame[8];

  Success = false;

  if (COM_Disable) {
    COM_Error = ERR_INITCOM;
    TRACE ("chyba\n");
    return 10;
  }
  /*
    // sestavym CONTROL REQUEST
    frame[0] = 0x40;  // INPUT CONTROL REQUEST
    frame[1] = 0x0d;  // cislo funkce pro "Read"
    frame[2] = 0x00;
    frame[4] = 0x00;
    frame[5] = 0x00;
    frame[6] = 0x04;  // pocet bytu vyslanych po OEP0
    frame[7] = 0x00;
  */
  // sestaveni datoveho ramce pro OEP0
  buffer[0] = Address % 256;  // dolni byte adresy
  buffer[1] = Address / 256;  // horni byte adresy
  buffer[2] = Size % 256;     // velikost bloku, ktery se posle pres IEP1 (max. 64B)
  buffer[3] = AddrTarget;     // cislo stanice

  CLAIM_INTERFACE()

  // poslu prikaz zapisu bufferu
  ret = usb_control_msg (udev, 0x41, 0xAD, 0, 0, buffer, 4, g_TransferTimeout);

  if (ret < 0) {
    TRACE ("%s\n", usb_strerror());
    Success = false;
    usb_release_interface (udev,0);
    return 20;
  }

  // vyctu buffer z OEP1
  ret = usb_interrupt_read (udev, 0x81, (char *) XBuffer, Size, 1000);
  if (ret != Size) {
    TRACE ("%s\n", usb_strerror());
    Success  = false;
    usb_release_interface (udev,0);
    return 30;
  }

  // cekam na odezvu
  ret = usb_interrupt_read (udev, 0x82, buffer, 1, 300);
  if (ret != 1) {
    TRACE ("%s\n", usb_strerror());
    Success  = false;
    usb_release_interface (udev,0);
    return 40;
  }

  if (buffer[0] == 16) {
    TRACE ("OK\n");
  } else {
    TRACE ("chyba %d\n", buffer[0]);
  }
  usb_release_interface (udev,0);

  return buffer[0];
}

bool CUSBPes::ReadCodeBlock (unsigned char * XBuffer, WORD Address, WORD Size) {
  if (m_bDirectMode) {
    return ReadCodeBlockDirect (XBuffer, Address, Size);
  }

  unsigned int size;

  for (unsigned int i = 0; i < Size;) {
    size = (Size - i) > 64 ? 64 : Size - i;
    if (ReadCodeBlock64 (&XBuffer[i], Address + i, size) != 16)
      if (ReadCodeBlock64 (&XBuffer[i], Address + i, size) != 16)
        return false;
    i += size;
  }

  return true;
}

bool CUSBPes::ReadCodeBlockDirect (unsigned char * XBuffer, WORD Address, WORD Size) {
  BYTE  ErrCnt = 0, j;
  WORD  i;

  IOData[0] = Address / 256;
  IOData[1] = Address % 256;
  if (!ReliableSend (Cm_SetXPtr)) {
    TRACE ("ReadCodeBlock::SetXPtr ... chyba c.%d\n", COM_Error);
    return false;
  }

  for (i = 0; i < Size; i+=4) {
    SendCmd (Cm_ReadXPDW); // posli je
    // jestli se to nepovedlo nastav ukazatel na posledn? poslany byte
    if (COM_Error != 0) {
      if (ErrCnt > 5) {
        TRACE ("ReadCodeBlock::Cm_ReadXPDW ... prekroceno opakovani, chyba c.%d\n", COM_Error);
        return false;
      }
      SendCmd (Cm_ClrError);
      ErrCnt++;
      IOData[0] = (Address + i) / 256;
      IOData[1] = (Address + i) % 256;
      if (!ReliableSend (Cm_SetXPtr)) {
        TRACE ("ReadCodeBlock::SetXPtr ... chyba c.%d\n", COM_Error);
        return false;
      }
    }
    for (j = 0; j < 4; j++) {
      if (i+j < Size)
        XBuffer[i+j] = IOData[j];
    }
  }

  return true;
}
void CUSBPes::UserWD (int64_t * wd) {
  PlatformPerformanceCounter (wd);
}

bool CUSBPes::UserHaf (int64_t wd, int _timeout_ms) {
  int64_t t;
  PlatformPerformanceCounter (&t);

  int64_t  cnt = _timeout_ms;

  if (t - wd <= cnt)
    return true;
  else
    return false;
}

/*
/// kokotiny
void CUSBPes::UserWD (int64_t *) {
}
*/
void CUSBPes::USBReset() {
  TRACE ("\nUSBReset - nechodi\n");
  return;

  int ret = usb_claim_interface (udev, 0);
  if (ret != 0) {
    TRACE ("USBReset claim - %s\n", usb_strerror());
  }

  ret = usb_control_msg (udev, 0x41, 0xA0, 0, 0, NULL, 0, g_TransferTimeout);
  if (ret != 0) {
    TRACE ("USBReset ctrl_msg - %s\n", usb_strerror());
  }

  usb_release_interface (udev,0);
}
