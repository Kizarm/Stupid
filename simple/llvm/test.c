#include "simpledef.h"

#define RESET 0x113E

extern RamDef_t * RamBasePtr;

static void xadd (WORD* a, WORD* b) {
 *a += *b;
}

void Simple (void) {
  carry = WgetBit (RESET);
  if (carry) {
    WcplBit (RESET);
    RamBasePtr->Words[0x60] = 0xAAAA;
  } else {
    RamBasePtr->Words[0x102] = RamBasePtr->Words[0x060];
    RamBasePtr->Words[0x061] = RamBasePtr->Words[0x100];
  }
  WORD a = 5;
  if (a > 10) WsetBit (RESET, 1);
}
