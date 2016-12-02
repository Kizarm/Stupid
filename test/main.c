#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "../simple/llvm/PesApi.h"
// Toto je jedina definice !
RamDef_t Variables;

void ApiNetDriver (WORD * ptr, WORD newdata) {
  WORD * base = Variables.Words;
  size_t index = (ptr - base);
  WORD olddata = * ptr;
//  if ((index<0x30) || (index>=0x40)) return; // sitove wordy
//  if (olddata == newdata)            return; // stara data jsou stejna
  printf ("new WORD at %04lX, %04X => %04X\n", index, olddata, newdata);
}
void ApiDispText (char * str) {
  printf ("DISPLAY : \"%s\"\n", str);
}

void printGlobals (void) {
  printf ("Carry[.6=C]:%02X\n",   Variables.Bytes[0x227]);
  printf ("060:%04X, 061:%04X\n", Variables.Words[0x60],  Variables.Words[0x61]);
  printf ("100:%04X, 102:%04X\n", Variables.Words[0x100], Variables.Words[0x102]);
//printf ("102:%d\n", Variables.Words[0x102]);
}
void printWords (void) {
  int i;
  const int n = 4;
  WORD * p = Variables.Words + 0x200 + 0x10;
  for (i=0; i<n; i++) {
    printf ("W%2d:\t%8d,\t%8d\n", i, p[i], p[i+n]);
  }
}
void printReals (void) {
  int i;
  const int n = 4;
  PR2PW p;
  p.PW = Variables.Words + 0x200;
  for (i=0; i<n; i++) {
    printf ("R%2d:\t%8.2f,\t%8.2f\n", i, p.PR[i], p.PR[i+n]);
  }
}
/** ******************************************************************************/
void test (void) {
  Variables.SYS.D32 = 0x1234;
  Variables.SYS.D33 = 0x1111;
  Variables.SYS.DX0 = 0xFFFF;
  Variables.SYS.DY0 = 0x5678;

  Variables.SYS.RESET=1;
  PSimple();
  printGlobals();
  printReals();
  printWords();
  PSimple();
  printReals();
  printWords();
}
int main (void) {
  if (LoadLibPes("libpes.so")) return -1;
  test();
  ExitLibPes();
  return 0;
}
