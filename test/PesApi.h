/* inclusion guard */
#ifndef __PESAPI_H__
#define __PESAPI_H__
#include "memorydef.h"

extern RamDef_t  Variables;
typedef void       (*VHandleV)  (void);
typedef PesApi_t * (*PAHandleV) (void);

extern VHandleV  PSimple;

extern int  LoadLibPes (const char * name);
extern void ExitLibPes (void);

extern PesApi_t * GetPesApiPtr (void);
// callbacks :
extern void ApiNetDriver (WORD * ptr, WORD newdata);
#endif /* __PESAPI_H__ */
