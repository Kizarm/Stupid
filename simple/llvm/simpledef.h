/* inclusion guard */
#ifndef __SIMPLEDEF_H__
#define __SIMPLEDEF_H__

#include "memorydef.h"

extern WORD carry, RegHL;
extern WORD WgetBit (unsigned bitadr);
extern void WsetBit (unsigned bitadr, WORD bitvalue);
extern void WcplBit (unsigned bitadr);

extern void PushCarry (WORD c);
extern WORD  PopCarry (void);

extern void PushReal (float c);
extern float PopReal (void);

extern void PushWord (WORD c);
extern WORD  PopWord (void);
extern int  TestStackPointer (void);

#endif // __SIMPLEDEF_H__