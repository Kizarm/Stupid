#include <stdio.h>
#include <string.h>
#include "cudppes.h"

CUDPPes::CUDPPes() : CPesNet() {
  printf ("%s\n", __FUNCTION__);
}

CUDPPes::~CUDPPes() {
  printf ("%s\n", __FUNCTION__);

}

void CUDPPes::COM_Close() {
  printf ("%s\n", __FUNCTION__);
  if (COM_Disable) return;
  COM_Disable = true;
}

void CUDPPes::COM_Init (int nVerify) {
  printf ("%s: %d\n", __FUNCTION__, nVerify);
  COM_Disable = false;
  Success     = true;
}
bool CUDPPes::MakeCmd (CmType Cm) {
  printf ("%s: %02X%02X%02X\n", __FUNCTION__, Cm.Cm, Cm.Rx, Cm.Tx);

  return true;
}

bool CUDPPes::SendCmd (CmType Cm) {
  printf ("%s: %02X%02X%02X\n", __FUNCTION__, Cm.Cm, Cm.Rx, Cm.Tx);

  return true;
}

BYTE CUDPPes::EEWrite (WORD Address, BYTE Dato) {
  printf ("%s: %04X-%02X\n", __FUNCTION__, Address, Dato);

  return 0;
}
// Zatim se s tim moc patlat nebudu.
struct EEContext_s {
  BYTE UNUSED1 [12];
  BYTE PIO;           // 12
  BYTE UNUSED2 [ 2];
  BYTE CodeMem;       // 15
  BYTE UNUSED3 [10];
  BYTE DataMem;       // 26
  BYTE LockSta;       // 27
  WORD DnCnt;         // 28
  BYTE StpName [8];   // 30
  struct {
    BYTE dd,mm;
    WORD year;  // BE, swap bytes
    BYTE hh,min;
  } lastDnl;          // 38
};
static const EEContext_s EE_Context = {
  .UNUSED1 = {0,},
  .PIO = 1,
  .UNUSED2 = {0,},
  .CodeMem = 2,
  .UNUSED3 = {0,},
  .DataMem = 2,
  .LockSta = 0x55,
  .DnCnt   = 1234,
  .StpName = {'T','e','s','t','V','i','e','w'},
  .lastDnl = {1,1,0xE107,11,59},
};
union EEBB {
  const EEContext_s * ee;
  const BYTE        * bb;
};
BYTE CUDPPes::EERead (WORD Address, BYTE & Dato) {
  EEBB eebb;
  eebb.ee = & EE_Context;
  if (Address < sizeof (EEContext_s)) Dato = eebb.bb [Address];
  else                                Dato = 0x55;
  // printf ("%s: %04d-%02X\n", __FUNCTION__, Address, Dato);
  return 0x5A;
}

/** ***************  Zde nepouzito  ***********************/
void CUDPPes::SendToken() {
  printf ("%s\n", __FUNCTION__);

}

bool CUDPPes::FastToken() {
  printf ("%s\n", __FUNCTION__);

  return true;
}

DWORD CUDPPes::BaudScan() {
  printf ("%s\n", __FUNCTION__);

  return 57600;
}

void CUDPPes::SetBaudRate (DWORD Baud) {
  printf ("%s: %d\n", __FUNCTION__, Baud);

}

bool CUDPPes::NetBreak() {
  printf ("%s\n", __FUNCTION__);

  return true;
}
// code from dll
bool CUDPPes::ReadCodeBlock (unsigned char* XBuffer, WORD Address, WORD Size) {
  printf ("%s: %p,%08X,%d\n", __FUNCTION__, XBuffer, Address, Size);

  return true;
}

BYTE CUDPPes::DownloadBlock (WORD Address, WORD Size, unsigned char* Data) {
  printf ("%s: %08X,%d,%p\n", __FUNCTION__, Address, Size, Data);

  return 0;
}
/** ************************************************************/
bool CUDPPes::GetStpVersion (char* buf) {
  strncpy (buf, "V5.67", 6);
  Success = true;
  return Success;
}
bool CUDPPes::GetNetVersion (char* buf) {
  strncpy (buf, "UDP00", 6);
  Success = true;
  return Success;
}
bool CUDPPes::GetPioName (char* buf) {
  strncpy (buf, "LINUX", 6);
  Success = true;
  return Success;
}

