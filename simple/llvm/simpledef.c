#include "simpledef.h"
#include <stdint.h>

//RamDef_t Variables;
WORD carry, RegHL;
RamDef_t * RamBasePtr;

void SetRamBasePtr (RamDef_t * ptr) {
  RamBasePtr = ptr;
}
WORD WgetBit (unsigned bitadr) {
  WORD result = 0u;
  BYTE val = RamBasePtr->Bytes [bitadr >> 3];
  BYTE ref = (1u << (bitadr & 0x7));
  if (ref & val) result = 1;
  return result;
}
void WsetBit (unsigned bitadr, WORD bitvalue) {
  BYTE ref = (1u << (bitadr & 0x7));
  if (bitvalue) {
    RamBasePtr->Bytes [bitadr >> 3] |=  ref;
  } else {
    RamBasePtr->Bytes [bitadr >> 3] &= ~ref;
  }
}
void WcplBit (unsigned bitadr) {
  BYTE ref = (1u << (bitadr & 0x7));
  RamBasePtr->Bytes [bitadr >> 3] ^= ref;
}

static unsigned StackCarry = 0;
static unsigned StackPosic = 0;
// depth = sizeof unsigned * 8, coz bude stacit.
// kontrolovat preteceni zatim nebudeme
void PushCarry (WORD c) {
  unsigned x = c & 1;
  unsigned y = 1u << StackPosic;
  if (x)  StackCarry |=  y;
  else    StackCarry &= ~y;
  StackPosic += 1;
}
WORD  PopCarry (void) {
  StackPosic -= 1;
  if (StackCarry & (1u << StackPosic)) return 1;
  else                                 return 0;
}
union REAL2WORD_u {
  float    re;
  uint32_t ui;
  struct {
    uint16_t hi;
    uint16_t lo;
  };
};
#define MAXSTACKPOS 0x100
typedef union REAL2WORD_u REAL2WORD;
static REAL2WORD RPN_Stack [MAXSTACKPOS];
static int StackPointer = 0;

void PushReal (float c) {
  RPN_Stack[StackPointer].re = c;
  StackPointer += 1;
}
float PopReal (void) {
  StackPointer -= 1;
  return RPN_Stack[StackPointer].re;
}

void PushWord (WORD c) {
  RPN_Stack[StackPointer].lo = c;
  StackPointer += 1;
}
WORD  PopWord (void) {
  StackPointer -= 1;
  return RPN_Stack[StackPointer].lo;
}
int  TestStackPointer (void) {
  return StackPointer;
}

