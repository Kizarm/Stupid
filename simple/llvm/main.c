#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "PesApi.h"
// Toto je jedina definice !
RamDef_t Variables;
/* Nepouziji funkce z peslib.so, napisu je znova.
static WORD MgetBit (unsigned bitadr) {
  WORD result = 0u;
  BYTE val = Variables.Bytes [bitadr >> 3];
  BYTE ref = (1u << (bitadr & 0x7));
  if (ref & val) result = 1;
  return result;
}
*/
static void MsetBit (unsigned bitadr, WORD bitvalue) {
  BYTE ref = (1u << (bitadr & 0x7));
  if (bitvalue) {
    Variables.Bytes [bitadr >> 3] |=  ref;
  } else {
    Variables.Bytes [bitadr >> 3] &= ~ref;
  }
}

void test (void) {
  Variables.Words[0x060] = 0x1234;
  Variables.Words[0x061] = 0x1111;
  Variables.Words[0x100] = 0xABCD;
  Variables.Words[0x102] = 0x5678;
  printf ("Carry[.6=C]:%02X\n",   Variables.Bytes[0x227]);
  printf ("060:%04X, 061:%04X\n", Variables.Words[0x60],  Variables.Words[0x61]);
  printf ("100:%04X, 102:%04X\n", Variables.Words[0x100], Variables.Words[0x102]);
  MsetBit(0x113E, 1);
  printf ("Carry[.6=C]:%02X\n",   Variables.Bytes[0x227]);
  PSimple();
  printf ("Carry[.6=C]:%02X\n",   Variables.Bytes[0x227]);
  printf ("060:%04X, 061:%04X\n", Variables.Words[0x60],  Variables.Words[0x61]);
  printf ("100:%04X, 102:%04X\n", Variables.Words[0x100], Variables.Words[0x102]);
  //MsetBit(0x113E, 0); // zbytecne, nulovano v Simple()
  printf ("Carry[.6=C]:%02X\n",   Variables.Bytes[0x227]);
  PSimple();
  printf ("Carry[.6=C]:%02X\n",   Variables.Bytes[0x227]);
  printf ("060:%04X, 061:%04X\n", Variables.Words[0x60],  Variables.Words[0x61]);
  printf ("100:%04X, 102:%04X\n", Variables.Words[0x100], Variables.Words[0x102]);
  
  PR2PW p;
  p.PW = Variables.Words + 0x200;
  printf ("Real=%g\n", * p.PR);
  p.PW = Variables.Words + 0x202;
  printf ("Real=%g\n", * p.PR);
}


int main (void) {
  if (LoadLibPes("libpes.so")) return -1;
  test();
  ExitLibPes();
  return 0;
}
