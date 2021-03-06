#ifndef _PESDEFINES_H
#define _PESDEFINES_H
#include "PesNet.h"
#include "wintypes.h"

// chyby
const unsigned char        ERR_NONE       = 0x00;

const unsigned char        ERR_INITCOM    = 0x01;
const unsigned char        ERR_TIMEOUT    = 0x02;
const unsigned char        ERR_TIMEOUTTX  = 0x03;
const unsigned char        ERR_TIMEOUTRX  = 0x04;
const unsigned char        ERR_RXSUM      = 0x05;
const unsigned char        ERR_DATASIZE   = 0x06;
const unsigned char        ERR_ADDRESS    = 0x07;
const unsigned char        ERR_DEBUGFLAG  = 0x08;
const unsigned char        ERR_MOREBAUD   = 0x09;
const unsigned char        ERR_NETEMPTY   = 0x0A;
const unsigned char        ERR_SETTIMEOUTS= 0x0B;
const unsigned char        ERR_SETSTATE   = 0x0C;
const unsigned char        ERR_SETCOMM    = 0x0D;
const unsigned char        ERR_CLEARRXTX  = 0x0E;
const unsigned char        ERR_TXDATA     = 0x0F;
const unsigned char        ERR_RXDATA     = 0x10;

// Error BYTE
const unsigned char        ERR_BADERROR   = 0x80;
const unsigned char        ERR_BADCMD     = 0x81;
const unsigned char        ERR_TARGETSUM  = 0x82;
const unsigned char        ERR_BADFRAME   = 0x83;
const unsigned char        ERR_SELFRX     = 0x84;
const unsigned char        ERR_IOFULL     = 0x85;
const unsigned char        ERR_NODATA     = 0x86;

// status BYTE
#define ST_ERROR      0x01
#define ST_INBUSY     0x02
#define ST_OUTREADY   0x04
#define ST_TOGGLEOK   0x08
#define ST_ERRORRAM   0x10
#define ST_STUPIDSTEP 0x20
#define ST_STUPIDRUN  0x40
#define ST_STUPIDIDLE 0x80

#define MAX_INTERNERR   11
#define MAX_TARGETERR   7

#define STD_TIMEOUT     50

#define LINE_RXTX       0x3F
#define LINE_TXHEAD     0x37
#define LINE_DLAB       0xDD
#define LINE_BREAK      0x7D

#define DOUBLESTOP      0x04
#define PAR_ENABLE      0x08
#define PAR_EVEN        0x10
#define PAR_NULL        0x20
#define BREAK           0x40
#define DLAB            0x80


#define DEFAULT_MCTRL   0x01

#define DTR             0x01
#define RTS             0x02
#define IRQ_ENABLE      0x08


#define STAT_RXREADY    0x01
#define STAT_OVERRUN    0x02
#define STAT_ERRPARITY  0x04
#define STAT_ERRFRAMING 0x08
#define STAT_TXREADY    0x20
#define STAT_TXEMPTY    0x40

// hlavicka
#define HD_CMDRQ        0xA0
#define HD_CMDF         0x80
#define HD_TOKEN        0xC0
#define HD_REQUEST      0xE0


// baudove rychlosti
const char              BdNums = 3;
const unsigned long     BdList[BdNums] = {57600, 19200, 9600};

#define NUM_REPEATS     10
#define STPBUFFER_SIZE  5120
#define STUPID_BEGIN    0x0400

// odezva na Cm_GetACK
#define ACK_KEYWORD     0x5A

// zamek datove EEPROM pro F series
#define EELOCK_KEY    0x4290
#define SFW_EELOCK    188

const unsigned char  EE_SerialLo     =   0;
const unsigned char  EE_SerialHi     =   1;
const unsigned char  EE_UserLo       =   2;
const unsigned char  EE_UserHi       =   3;
const unsigned char  EE_SizeLo       =   4;
const unsigned char  EE_SizeHi       =   5;
const unsigned char  EE_CheckLo      =   6;
const unsigned char  EE_CheckHi      =   7;
const unsigned char  EE_EETop        =   8;
const unsigned char  EE_Baud         =   9;
const unsigned char  EE_StationAddr  =  10;
const unsigned char  EE_RunMode      =  11;
const unsigned char  EE_PIOTyp       =  12;

const unsigned char  EE_CalibLo      =  13;
const unsigned char  EE_CalibHi      =  14;
const unsigned char  EE_CodeMemCfg   =  15;
const unsigned char  EE_StackPage    =  16;
const unsigned char  EE_DataMemCfg   =  26;
const unsigned char  EE_Locked       =  27;

const unsigned char  EE_DownloadCntLo       =  28;
const unsigned char  EE_DownloadCntHi       =  29;
const unsigned char  EE_DNLNameBegin        =  30;
const unsigned char  EE_DNLNameEnd          =  37;
const unsigned char  EE_LastDownloadBegin   =  38;
const unsigned char  EE_LastDownloadEnd     =  43;
const unsigned char  EE_PasswordBegin       =  44;
const unsigned char  EE_PasswordEnd         =  51;
const unsigned char  EE_PasswordCheck       =  52;
const unsigned char  EE_PasswordEntredLo    =  53;
const unsigned char  EE_PasswordEntredHi    =  54;
const unsigned char  EE_ExStationAddr       =  55;

#define PASSWORD_ENTRED 12957

const WORD       TopEE         =  0xff;



// DIRECT COMMANDS
//-----------------                        CMD  Tx   Rx
/* Read&Write X data*/
const CmType          Cm_WriteXBit   = { 0x19,  3, 0};
const CmType          Cm_WriteXByte  = { 0x05,  3, 0};
const CmType          Cm_ReadXByte   = { 0x06,  2, 1};
const CmType          Cm_WriteXWord  = { 0x1B,  4, 0};
const CmType          Cm_ReadXWord   = { 0x1C,  2, 2};
const CmType          Cm_ReadXPDWC   = { 0x1D,  0, 4};

/* Blokovy Read&Write X data */
const CmType          Cm_SetXPtr     = { 0x08,  2, 0};
const CmType          Cm_WriteXPB    = { 0x09,  1, 0};
const CmType          Cm_ReadXPB     = { 0x0C,  0, 1};
const CmType          Cm_WriteXPW    = { 0x0A,  2, 0};
const CmType          Cm_ReadXPW     = { 0x0D,  0, 2};
const CmType          Cm_WriteXPDW   = { 0x0B,  4, 0};
const CmType          Cm_ReadXPDW    = { 0x0E,  0, 4};

/* Read&Write Stupid data */
const CmType          Cm_WriteSWord  = { 0x0F,  3, 0};
const CmType          Cm_ReadSWord   = { 0x10,  1, 2};
const CmType          Cm_SetSBit     = { 0x11,  2, 0};
const CmType          Cm_ClrSBit     = { 0x12,  2, 0};
const CmType          Cm_ReadSBit    = { 0x13,  2, 1};

/* status */
const CmType          Cm_ClrError    = { 0x14,  0, 0};
const CmType          Cm_GetError    = { 0x15,  0, 1};

/* informacni prikazi */
const CmType          Cm_GetACK      = { 0x01,  0, 1};
const CmType          Cm_GetStatus   = { 0x02,  0, 1};
const CmType          Cm_GetNETVer   = { 0x03,  0, 6};
const CmType          Cm_GetSTPVer   = { 0x04,  0, 6};
const CmType          Cm_GetIOBuf    = { 0x16,  0, 0xFF};
const CmType          Cm_GetPIOName  = { 0x17,  1, 6};
const CmType          Cm_GetMemCfg   = { 0x18,  0, 1};

/* ukonceni prikazu */
const CmType          Cm_Abort       = { 0x07,  0, 0};


/* INDIRECT COMMANDS */
const CmType          Cm_IO_WriteEEByte  = { 0x80,  3, 0};
const CmType          Cm_IO_ReadEEByte   = { 0x81,  2, 1};

const CmType          Cm_IO_StupidReset  = { 0x82,  0, 0};
const CmType          Cm_IO_StupidStep   = { 0x83,  0, 0};
const CmType          Cm_IO_StupidStop   = { 0x84,  0, 0};
const CmType          Cm_IO_StupidRun    = { 0x85,  0, 0};
const CmType          Cm_IO_Reboot       = { 0x86,  0, 0};
const CmType          Cm_IO_SetTime      = { 0x87,  3, 0};
const CmType          Cm_IO_SetDate      = { 0x88,  4, 0};

/*
const CString ErrText_Internal[MAX_INTERNERR] = {
  "nepodporovan  chybov  k d"
  ,"chyba p i inicializaci COM portu"
  ,"p ekro en timeout na COM portu"
  ,"Vypr el timeout pro TX - data"
  ,"Vypr el timeout pro RX - data"
  ,"chybn  kontroln  sou et v RX - data"
  ,"Nen  specifikov na d lka RX - bloku"
  ,"Stanice nen  aktivn "
  ,"DEBUG - MARKING FLAG"
  ,"r zn  baudov  rychlosti v s ti"
  ,"  dn  stanice v s ti"
};

const CString ErrText_Target[MAX_TARGETERR] = {
  "nepodporovan  chybov  k d"
  ,"nezn m  p  kaz v CMD-FRAME"
  ,"chybn  kontroln  sou et v posledn m r mci"
  ,"nezn m  typ r mce"
  ,"TRANSMITT SELF-CHECK ERROR"
  ,"Vstupn  buffer je pln  - frame je ztracen!"
  ,"IO BUFFER je pr zdn !"
};
*/
#endif //_PESDEFINES_H
