#include "simpledef.h"
#include <stdio.h>
#include <stdint.h>
// pokud je inicializovana, je v sekci .data
static PesApi_t MainApi = {
  .SetRamBase = SetRamBasePtr,
  .MainPass   = Simple
};

RamDef_t * RamBasePtr;

//__attribute__ ((section (".start_proc")))
PesApi_t * GetPesApiPtr (void) {
  MainApi.SetRamBase = SetRamBasePtr;   // v MCU lepe inicializovat zde
  MainApi.MainPass   = Simple;          // pokud to bude nekde extra.
  //
  MainApi.WordChangeDriver = 0;
  return & MainApi;
}
void SetRamBasePtr (RamDef_t * ptr) {
  RamBasePtr = ptr;
}
WORD WgetBit (unsigned bitadr) {
  WORD result = 0u;
  WORD val = RamBasePtr->Words [bitadr >> 4];
  WORD ref = (1u << (bitadr & 0xF));
  if (ref & val) result = 1;
  return result;
}
void WsetBit (unsigned bitadr, WORD bitvalue) {
  WORD ref = (1u << (bitadr & 0xF));
  if (bitvalue) {
    RamBasePtr->Words [bitadr >> 4] |=  ref;
  } else {
    RamBasePtr->Words [bitadr >> 4] &= ~ref;
  }
}
void WcplBit (unsigned bitadr) {
  WORD ref = (1u << (bitadr & 0xF));
  RamBasePtr->Words [bitadr >> 4] ^= ref;
}

WORD TestPole (const WORD a, const WORD b) {
#ifndef __arm__
//  printf ("\ta=%04X, b=%04X\n", a, b);
#endif
  if (a > b) return b;
  return a;
}
/** ************************************************************************/
void WordChangeWrap (WORD * ptr, WORD newdata) {        // volano ze Simple()
  if (MainApi.WordChangeDriver) MainApi.WordChangeDriver(ptr, newdata);
}