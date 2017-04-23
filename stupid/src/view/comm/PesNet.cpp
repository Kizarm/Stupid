#include <stdio.h>
#include <string.h>

#include "PesNet.h"
//#include "PESDef.h"
#include "PesDefines.h"
#include "platform.h"
#include "viewwindow.h"

#define TRACE printf
//#define TRACE(...)

static const char * ErrText_Internal[MAX_INTERNERR] = {
  "nepodporovaný chybový kód"
  ,"chyba při inicializaci COM portu"
  ,"překročen timeout na COM portu"
  ,"Vypršel timeout pro TX - data"
  ,"Vypršel timeout pro RX - data"
  ,"chybný kontrolní součet v RX - data"
  ,"Není specifikována délka RX - bloku"
  ,"Stanice není aktivní"
  ,"DEBUG - MARKING FLAG"
  ,"různé baudové rychlosti v síti"
  ,"žádná stanice v síti"
};
/*
static const char * ErrText_Target[MAX_TARGETERR] = {
  "nepodporovaný chybový kód"
  ,"neznámý příkaz v CMD-FRAME"
  ,"chybný kontrolní souèet v posledním rámci"
  ,"neznámý typ rámce"
  ,"TRANSMITT SELF-CHECK ERROR"
  ,"Vstupní buffer je plný - frame je ztracen!"
  ,"IO BUFFER je prázdný!"
};
*/
CPesNet::CPesNet (/*MainWindow * p*/) {
  // parent        = p;
  BaudRate      = 57600;
  COM_Lock      = false;
  COM_Disable   = true;
  StationNum    = 0;
  AddrTarget    = 0;
}
//////////////////////////////////////////////////////////////////////
//  Nazev   : SelectStation
//  Popis   : Nastavi cislo stanice
//  Parametry : StAddr - cislo stanice
//////////////////////////////////////////////////////////////////////
bool CPesNet::SelectStation (BYTE StAddr) {
  Success = false;

  if (StAddr > 31) {
    COM_Error = ERR_ADDRESS;
    AddrTarget = 0x1F;
  } else {
    Success = true;
    AddrTarget = StAddr;
    TRACE ("#Info#\tNastavena stanice : %d\n", AddrTarget);
  }

  return Success;
}
void CPesNet::NetScan (void) {
  BYTE  i, temp;
  for (i = 0; i < 31; i++) {
    parentWidget->Progress(i);
    SelectStation (i);
    temp = 0;
    if (SendCmd (Cm_GetACK)) temp = IOData[0];
    else                     continue;
    if (SendCmd (Cm_GetACK)) {
      if (Success && IOData[0] == ACK_KEYWORD /*&& IOData[1] == ACK_KEYWORD*/ && temp == ACK_KEYWORD) {
        TRACE ("#Info#\tNalezena stanice : %d\n", i);
        StationList[StationNum] = i;
        StationNum++;
      }
    }
  }
  parentWidget->Progress(0);
}
//////////////////////////////////////////////////////////////////////
//  Nazev   : ReliableSend
//  Popis   : Posle sitovi prikaz a pokud se to nezdari, tak to
//                  zkusi znovu
//  Parametry : Cmd - prikaz k odeslani
//////////////////////////////////////////////////////////////////////
// Pozor u USB prevodniku pouzivat z rozvahou !!!
bool CPesNet::ReliableSend (CmType Cmd) {
  BYTE    Rep;
  BYTE  SaveIO[10];
  BYTE  i;


  for (i = 0; i < 10; i++) {
    SaveIO[i] = IOData[i];
  }

  Rep = NUM_REPEATS;

  do {
    if (Rep != NUM_REPEATS) TRACE ("\nReliableSend - repeat = %d\n", Rep);
    for (i = 0; i < 10; i++) {
      IOData[i] = SaveIO[i];
    }

    if (SendCmd (Cmd)) {
      return true;
    }

    SendCmd (Cm_ClrError);

    --Rep;
  } while (Rep);

  return false;
}
//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadSTPWord
//  Popis   : Cteni STP wordu
//  Parametry : Addr - wordova adresa STP wordu
//////////////////////////////////////////////////////////////////////
WORD CPesNet::ReadSTPWord (unsigned int Addr) {
  WORD  Dato;

  IOData[0] = Addr % 256;
  if (!ReliableSend (Cm_ReadSWord)) {
    Dato = 0;
    Success = false;
    return Dato;
  }

  Dato = (256 * IOData[0]) + IOData[1];

  Success = true;
  return Dato;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadXByte
//  Popis   : Cteni bitu z pameti
//  Parametry : Addr - bitova adresa
//////////////////////////////////////////////////////////////////////
BYTE CPesNet::ReadXByte (unsigned int Addr) {
  BYTE  Dato;

  IOData[0] = Addr / 256;
  IOData[1] = Addr % 256;
  if (!ReliableSend (Cm_ReadXByte)) {
    Dato = 0;
    Success = false;
    return Dato;
  }
  Dato = IOData[0];
  Success = true;
  return Dato;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadXByte
//  Popis   : Cteni STP bitu
//  Parametry : Addr - cislo bitu
//////////////////////////////////////////////////////////////////////
BYTE CPesNet::ReadSTPBit (unsigned int Addr) {
  if (Addr < 320) {
    IOData[0] = Addr / 256;
    IOData[1] = Addr % 256;

    ReliableSend (Cm_ReadSBit);

    return (IOData[0] == 0) ? 0 : 1;
  } else {
    /* puvodne
    // jinak pouzij ReadxByte
    // 936 - pocatek pameti(936.0 = x32, 936.1 = x33,...)
    BYTE b = _rotr (ReadXByte (936 + (Addr - 320) /8) , (Addr - 320) %8);

    b &= 0x01;
    */
    BYTE b = (ReadXByte (936 + (Addr - 320) /8) , (Addr - 320) %8);
    /// ???
    BYTE i, mask_msb = 0x80, mask_lsb = 0x01;
    BYTE rotate = (Addr - 320) & 0x7;
    for (i=0; i<rotate; i++) {
      if (b & mask_lsb) {
        b |=  mask_msb;
      }
      b >>= 1;
    }

    b &= 0x01;
    return b;
  }

}

//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteSTPWord
//  Popis   : Zapis STP wordu
//  Parametry : Addr - adresa STP wordu
//                  Dato - zapisovana hodnota
//////////////////////////////////////////////////////////////////////
bool CPesNet::WriteSTPWord (unsigned int Addr, unsigned int Dato) {
  IOData[0] = Addr % 256;
  IOData[1] = Dato / 256;
  IOData[2] = Dato % 256;

  ReliableSend (Cm_WriteSWord);

  return Success;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteXByte
//  Popis   : Zapis bytu do pameti
//  Parametry : Addr - adresa v pameti
//                  Dato - zapisovana hodnota
//////////////////////////////////////////////////////////////////////
bool CPesNet::WriteXByte (unsigned int Addr, unsigned char Dato) {
  IOData[0] = Addr / 256;
  IOData[1] = Addr % 256;
  IOData[2] = Dato;

  return ReliableSend (Cm_WriteXByte);
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteSTPBit
//  Popis   : Zapis STP bitu
//  Parametry : Addr - bitova adresa
//                  Bit - zapisovana hodnota(0,1)
//////////////////////////////////////////////////////////////////////
bool CPesNet::WriteSTPBit (unsigned int Addr, unsigned char Bit) {
  // otestuj jestli se jedna o zakladni bit y/x0 - y/x32
  if (Addr < 320) {
    IOData[0] = Addr / 256;
    IOData[1] = Addr % 256;
    if (Bit)
      Success = ReliableSend (Cm_SetSBit);
    else
      Success = ReliableSend (Cm_ClrSBit);
  } else {
    // jinak pouzij WriteXBit
    // 936 - pocatek pameti(936.0 = x32, 936.1 = x33,...)
    WriteXBit (936 + (Addr - 320) /8, // adresa v pameti
               (Addr - 320) %8,  // cislo bitu
               Bit ? 1 : 0      // hodnota
              );
  }

  return Success;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadXBlock
//  Popis   : Cteni bloku z pameti pres SetXPtr
//  Parametry : XBuffer - pole pro ulozeni vyctenych hodnot
//                  Offset - pocatecni adresa v pameti
//                  Size - pocet bytu pro cteni
//////////////////////////////////////////////////////////////////////
bool CPesNet::ReadXBlock (unsigned char * XBuffer, unsigned int Offset, unsigned int Size) {
  WORD    i;
  BYTE  ErrCnt=0;

  IOData[0] = Offset / 256;
  IOData[1] = Offset % 256;
  if (!ReliableSend (Cm_SetXPtr))
    return false;

  i = 0;

  do {

    if (!SendCmd (Cm_ReadXPB)) {
      if (ErrCnt > NUM_REPEATS)
        return false;
      SendCmd (Cm_ClrError);
      ErrCnt++;
      IOData[0] = (Offset + i) / 256;
      IOData[1] = (Offset + i) % 256;
      if (!ReliableSend (Cm_SetXPtr))
        return false;
    } else {
      XBuffer[i] = IOData[0];
      i++;
      ErrCnt = 0;
    }
  } while (i < Size);

  return true;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadBlock
//  Popis   : Cteni bloku z pameti po wordech
//  Parametry : XBuffer - pole pro ulozeni vyctenych hodnot
//                  Offset - pocatecni adresa v pameti
//                  Size - pocet bytu pro cteni
//////////////////////////////////////////////////////////////////////
bool CPesNet::ReadBlock (BYTE * XBuffer, WORD Offset, WORD Size) {
  WORD    i;
  //BYTE  ErrCnt=0;
  WORD  nAddr = Offset;


  WORD  nCnt = Size / 2;

  for (i = 0; i < nCnt; i+=2) {
    IOData[0] = nAddr / 256;
    IOData[1] = nAddr % 256;
    if (!ReliableSend (Cm_ReadXWord)) {
      return false;
    }

    XBuffer[i] = IOData[0];
    XBuffer[i + 1] = IOData[1];

    nAddr += 2;
  }

  for (; i < Size; i++) {
    IOData[0] = nAddr / 256;
    IOData[1] = nAddr % 256;
    if (!ReliableSend (Cm_ReadXByte)) {
      return false;
    }

    XBuffer[i] = IOData[0];

    ++nAddr;
  }

  return true;
}


//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadEEBlock
//  Popis   : Cteni bloku z EEPROM
//  Parametry : XBuffer - pole pro ulozeni vyctenych hodnot
//                  Offset - pocatecni adresa v pameti
//                  Size - pocet bytu pro cteni
//////////////////////////////////////////////////////////////////////
bool CPesNet::ReadEEBlock (unsigned char * XBuffer, unsigned char Offset, unsigned int Size) {
  WORD  i;

  for (i = 0; i < Size; i++) {
    if (!ReadEEByte (Offset+i, XBuffer[i])) {
      return false;
    }
  }

  return true;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteXBlock
//  Popis   : Zapis bloku do pameti pres SetXPtr
//  Parametry : XBuffer - pole zapisovanych hodnot
//                  Offset - pocatecni adresa v pameti
//                  Size - pocet bytu pro zapis
//////////////////////////////////////////////////////////////////////
bool CPesNet::WriteXBlock (unsigned char * XBuffer, unsigned int Offset, unsigned int Size) {
  WORD  i;
  BYTE  ErrCnt = 0;

  IOData[0] = Offset / 256;
  IOData[1] = Offset % 256;
  if (!ReliableSend (Cm_SetXPtr))
    return false;

  i = 0;

  do {
    IOData[0] = XBuffer[i];
    if (!SendCmd (Cm_WriteXPB)) {
      if (ErrCnt > NUM_REPEATS)
        return  false;
      SendCmd (Cm_ClrError);
      ErrCnt++;
      IOData[0] = (Offset + i) / 256;
      IOData[1] = (Offset + i) % 256;
      if (!ReliableSend (Cm_SetXPtr))
        return  false;
    } else {
      i++;
      ErrCnt = 0;
    }
  } while (i < Size);

  return true;
}


char * CPesNet::GetErrorString (char * ErrString) {
  if (COM_Error >= 0x80) {
    COM_Error -= 0x80;

    if (COM_Error > MAX_TARGETERR)
      COM_Error = 0;
    sprintf (ErrString,"%s", ErrText_Internal[COM_Error]);
  } else {
    if (COM_Error > MAX_INTERNERR)
      COM_Error = 0;
    sprintf (ErrString,"%s", ErrText_Internal[COM_Error]);
  }

  return ErrString;
}
//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteEEByte
//  Popis   : Zapis bytu do EEPROM(opakovane)
//  Parametry : Address - adesa v EEPROM
//                  Dato - zapisovana hodnota
//////////////////////////////////////////////////////////////////////
BYTE CPesNet::WriteEEByte (WORD Address, BYTE Dato) {
  BYTE  Rep,error,pom;

  Rep = NUM_REPEATS;
  do {
    error = EEWrite (Address,Dato);
    if (error == 0x5A) {
      if (Address != 0xFF && Address != 0xFC && Address != 0xFB) {
        if (ReadEEByte (Address,pom) && pom == Dato)
          return 0x5A;
      } else {
        return 0x5A;
      }
    }
    Rep--;
  } while (Rep != 0);

  return error;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadEEByte
//  Popis   : Cteni bytu z EEPROM(opakovane)
//  Parametry : Address - adesa v EEPROM
//                  Dato - vyctena hodnota
//////////////////////////////////////////////////////////////////////
bool CPesNet::ReadEEByte (WORD Address, BYTE & Dato) {
  BYTE Rep, error;


  Rep = NUM_REPEATS;
  do {
    error = EERead (Address, Dato) ;
    if (error == 0x5A)
      return true;
    TRACE ("\nEERead error %d\n", error);
    Rep--;
  } while (Rep != 0);

  return false;
}
//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteXBit
//  Popis   : Zapis bitu do pameti
//  Parametry : Addr - bytova adresa bitu
//                  Bit - bitova adresa
//                  Set - hodnota(0,1)
//////////////////////////////////////////////////////////////////////
bool CPesNet::WriteXBit (unsigned int Addr, unsigned char Bit, bool Set) {
  IOData[0] = Addr / 256;
  IOData[1] = Addr % 256;
  if (Set)
    IOData[2] = (char) 128;
  else
    IOData[2] = 0;

  IOData[2] |= Bit;

  return ReliableSend (Cm_WriteXBit);
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadXWord
//  Popis   : Cteni wordu z pameti
//  Parametry : Address - adesa v pameti
//                  Dato - vyctena hodnota
//////////////////////////////////////////////////////////////////////
bool CPesNet::ReadXWord (unsigned int Addr, WORD * Dato) {

  IOData[0] = Addr / 256;
  IOData[1] = Addr % 256;
  if (!ReliableSend (Cm_ReadXWord)) {
    return false;
  }
  * ( ( (BYTE *) Dato) + 1) = IOData[0];
  * ( ( (BYTE *) Dato)) = IOData[1];
  return true;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteXWord
//  Popis   : Zapis wordu do pameti
//  Parametry : Address - adesa v pameti
//                  Dato - zapisovana hodnota
//////////////////////////////////////////////////////////////////////
bool CPesNet::WriteXWord (unsigned int Addr, WORD Dato) {
  IOData[0] = Addr / 256;
  IOData[1] = Addr % 256;
  IOData[2] = * ( ( (BYTE *) &Dato) + 1);
  IOData[3] = * ( ( (BYTE *) &Dato));

  return ReliableSend (Cm_WriteXWord);
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : ReadXDWord
//  Popis   : Cteni double wordu z pameti(pres SetXPtr)
//  Parametry : Address - adesa v pameti
//                  Dato - vyctena hodnota
//////////////////////////////////////////////////////////////////////
bool CPesNet::ReadXDWord (unsigned int Addr, DWORD * Dato) {
  IOData[0] = Addr / 256;
  IOData[1] = Addr % 256;
  if (!ReliableSend (Cm_SetXPtr))
    return false;

  if (!SendCmd (Cm_ReadXPDW)) {
    SendCmd (Cm_ClrError);
    return false;
  }

  * ( ( (BYTE *) Dato)) = IOData[0];
  * ( ( (BYTE *) Dato) + 1) = IOData[1];
  * ( ( (BYTE *) Dato) + 2) = IOData[2];
  * ( ( (BYTE *) Dato) + 3) = IOData[3];

  return true;
}

//////////////////////////////////////////////////////////////////////
//  Nazev   : WriteXDWord
//  Popis   : Zapis double wordu do pameti(pres SetXPtr)
//  Parametry : Address - adesa v pameti
//                  Dato - zapisovana hodnota
//////////////////////////////////////////////////////////////////////
bool CPesNet::WriteXDWord (unsigned int Addr, DWORD Dato) {
  IOData[0] = Addr / 256;
  IOData[1] = Addr % 256;
  if (!ReliableSend (Cm_SetXPtr))
    return false;

  IOData[0] = * ( ( (BYTE *) &Dato));
  IOData[1] = * ( ( (BYTE *) &Dato) + 1);
  IOData[2] = * ( ( (BYTE *) &Dato) + 2);
  IOData[3] = * ( ( (BYTE *) &Dato) + 3);
  if (!SendCmd (Cm_WriteXPDW)) {
    return false;
  }

  return true;
}

// Vraci retezec identifikujici typ datove pameti
char * CPesNet::GetDataMemType (BYTE  Type, char * Info) {
  switch (Type) {
    case  1 :
      sprintf (Info, "8kB NVRAM");
      break;
    case  2 :
      sprintf (Info, "32kB NVRAM");
      break;
    case  3 :
      sprintf (Info, "8kB Time Keeper");
      break;
    case  4 :
      sprintf (Info, "32kB Time Keeper");
      break;
    case  6 :
      sprintf (Info, "32kB SRAM");
      break;
    case  7 :
      sprintf (Info, "2kB XRAM");
      break;
    case  8 :
      sprintf (Info, "512kB SRAM");
      break;
    case  9 :
      sprintf (Info, "512kB NVRAM");
      break;
    case  10  :
      sprintf (Info, "512kB Time Keeper");
      break;
    default   :
      sprintf (Info, "UNKNOW");
  }

  return Info;
}

// Vraci retezec identifikujici typ programove pameti
char * CPesNet::GetProgMemType (BYTE  Type, char * Info) {
  switch (Type) {
    case  0 :
      sprintf (Info, "8kB NVRAM(C/D)");
      break;
    case  1 :
      sprintf (Info, "32kB NVRAM(C/D)");
      break;
    case  2 :
      sprintf (Info, "8kB EEPROM");
      break;
    case  3 :
      sprintf (Info, "32kB EEPROM");
      break;
    case  4 :
      sprintf (Info, "8kB fast EEPROM");
      break;
    case  5 :
      sprintf (Info, "32kB fast EEPROM");
      break;
    case  6 :
      sprintf (Info, "62kB FLASH");
      break;
    default   :
      sprintf (Info, "UNKNOW");
  }

  return Info;
}
/// kokotiny
bool CPesNet::UserHaf (int64_t, int) {
  return true;
}
void CPesNet::UserWD (int64_t *) {
}

void CPesNet::InitWaitFunc() {
}
void CPesNet::WaitShortTime (int64_t _timeout_mks) {
  PlatformSleep (_timeout_mks);
}

/// Linux Viewer
bool CPesNet::SetStupidState (StupidStates st) {
  Success = false;
  const CmType stupid = { st, 0, 0 };
  Success = SendCmd (stupid);
  return Success;
}
bool CPesNet::GetNetVersion (char * buf) {
  Success = false;
  if (!ReliableSend (Cm_GetNETVer)) return Success;
  Success = true;
  memcpy (buf, IOData, 6);
  buf [6] = '\0';
  return Success;
}
bool CPesNet::GetPioName (char * buf) {
  BYTE tdat, i;
  Success = false;
  if (!ReadEEByte (EE_PIOTyp, tdat)) return Success;
  for (int i=0; i<10; i++) IOData [i] = 0;
  IOData[0] = tdat;
  if (!ReliableSend (Cm_GetPIOName)) return Success;
  if (IOData[0]) {
    for (i=0; i<6; i++) buf [i] = IOData [i];
  } else {
    for (i=0; i<6; i++) buf [i] = '0';
  }
  Success = true;
  buf [i] = '\0';
  return Success;
}

bool CPesNet::GetStpVersion (char * buf) {
  Success = false;
  if (!ReliableSend (Cm_GetSTPVer)) return Success;
  Success = true;
  memcpy (buf, IOData, 6);
  buf [6] = '\0';
  return Success;
}

bool CPesNet::GetStatus (BYTE & sta) {
  Success = SendCmd (Cm_GetStatus);
  if (!Success) return Success;
  sta = IOData[0];
  return Success;
}
bool CPesNet::GetDownCount (WORD & cnt) {
  Success = false;
  BYTE tb [2];
  //TRACE ("GetDownCount\n");
  if (!ReadEEBlock (tb , EE_DownloadCntLo, 2)) return Success;
  cnt = ( (WORD) tb[1] << 8) + (WORD) tb[0];
  Success = true;
  TRACE ("GetDownCount %d\n", cnt);
  return Success;
}
bool CPesNet::GetLastDown (char * buf, int len) {
  BYTE tmp [6];
  Success = ReadEEBlock (tmp, EE_LastDownloadBegin, 6);
  if (!Success) return Success;
  for (int i=0; i<6; i++) {
    if ( (tmp[i]) > 99 && (i != 3)) {   // nesmysl
      Success = false;
      break;
    }
  }
  if (!Success) {
    strcpy (buf, "unknown");
    Success = true;
    return Success;
  }
  snprintf (buf, len, "%d:%02d,%d.%d.%d", tmp[4],tmp[5], tmp[0], tmp[1], ( (WORD) tmp[2] << 8) + (WORD) tmp[3]);
  Success = true;
  TRACE ("GetLastDown %s\n", buf);
  return Success;
}
bool CPesNet::GetLockState (char * buf) {
  BYTE tmp;
  Success = ReadEEByte (EE_Locked, tmp);
  if (!Success) return Success;
  switch (tmp) {
    case 0x55:
      strcpy (buf, "UNLOCK");
      break;
    case 0xAA:
      strcpy (buf, "LOCK");
      break;
    default:
      strcpy (buf, "UNKNOWN");
      break;
  }
  return Success;
}
