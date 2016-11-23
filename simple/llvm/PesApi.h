/* inclusion guard */
#ifndef __PESAPI_H__
#define __PESAPI_H__
#include "memorydef.h"

extern RamDef_t  Variables;

typedef void (*VHandleV)  (void);
/* Rozhrani */
extern VHandleV  PSimple;       // jeden pruchod Simple() z peslib
extern int  LoadLibPes (const char * name);
extern void ExitLibPes (void);

#endif /* __PESAPI_H__ */
