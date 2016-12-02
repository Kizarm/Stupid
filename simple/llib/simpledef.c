#include "simpledef.h"

/**
 * Nakonec to skončilo tak,
 * že tyhle pomocné funkce jsou kompilovány
 * do llvm pomocí clang, ožežou se z toho pomocí sed
 * nepotřebné věci a celé se to includuje pomocí peslib.s
 * přímo do zdrojáků cílového llvm textu. Konečný produkt
 * je tedy celý interface v assembleru.
 * 
 * Z toho jde udělat *.so knihovnu pro PC (zatím Linux),
 * nebo *.hex pro automat. Interface je však stejný.
 * */

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
  MainApi.WordChangeDriver  = 0;
  MainApi.DisplayTextDriver = 0;
  return & MainApi;
}
void SetRamBasePtr (RamDef_t * ptr) {
  RamBasePtr = ptr;
}
WORD WgetBit (unsigned bitadr) {
  WORD result = 0u;
  WORD val = RamBasePtr->Words [bitadr >> 4];
  WORD ref = (1u << (bitadr & 0xF));
  if (ref & val) { result = 1; }
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
  if (a > b) { return b; }
  return a;
}
short absi (short n) {
  if (n<0) { return -n; }
  else     { return +n; }
}
union fu {
  int   i;
  float f;
};
/**
 * Tohle je vzato z newlib, aby nebylo nutné linkovat -lm.
 * Je to sice hnusné, jde to většinou 1 instrukcí, ale nevím,
 * jak sem dostat builtin.
 * */
float truncf (float x) {
  union fu u;
  int signbit, w, exponent_less_127;

  u.f = x;
  w = u.i;
  /* Extract sign bit. */
  signbit = w & 0x80000000;
  /* Extract exponent field. */
  exponent_less_127 = ( (w & 0x7f800000) >> 23) - 127;
  if (exponent_less_127 < 23) {
    if (exponent_less_127 < 0)  {
      u.i = signbit; // -1 < x < 1, so result is +0 or -0.
    } else {
      u.i = signbit | (w & ~ (0x007fffff >> exponent_less_127));
    }
  } else  {
    if (exponent_less_127 == 255) { // x is NaN or infinite.
      return x + x;
    }
    /* All bits in the fraction field are relevant. */
  }
  x = u.f;
  return x;
}

float fabsf (float x) {
  if (x < 0.0f) { return -x; }
  else          { return +x; }
}
/** ************************************************************************/
void WordChangeWrap (WORD * ptr, WORD newdata) {        // volano ze Simple()
  if (MainApi.WordChangeDriver) { MainApi.WordChangeDriver (ptr, newdata); }
}
void Disps (char * str) {
  if (MainApi.DisplayTextDriver) { MainApi.DisplayTextDriver (str); }
}


