/* inclusion guard */
#ifndef __SIMPLEDEF_H__
#define __SIMPLEDEF_H__

#include "../simple/llvm/memorydef.h"

extern WORD WgetBit (unsigned bitadr);
extern void WsetBit (unsigned bitadr, WORD bitvalue);
extern void WcplBit (unsigned bitadr);

extern WORD TestPole (const WORD a, const WORD b);

extern void SetRamBasePtr (RamDef_t * ptr);
extern void Simple (void);

#endif // __SIMPLEDEF_H__
//enum typ;
