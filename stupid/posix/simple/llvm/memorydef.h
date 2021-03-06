#ifndef MEMORY_DEF_H_
#define MEMORY_DEF_H_
#include "System.h"

#define RAMLEN 0x800

typedef unsigned char  BYTE;
typedef unsigned short WORD;

struct _StationInfo {
  unsigned int NetAddr;
  unsigned short DataBegin;
  unsigned short DataEnd;
};
typedef struct _StationInfo StationInfo;

union PR2PW_u {
  float * PR;
  WORD  * PW;
};
typedef union PR2PW_u PR2PW;

union RamDef_u {
  struct SystemVariables SYS;
  BYTE Bytes [RAMLEN >> 0];
  WORD Words [RAMLEN >> 1];
};
typedef union RamDef_u RamDef_t;

struct PesApi_s {
  // API
  void (*SetRamBase) (RamDef_t * ptr);
  void (*MainPass)   (void);
  const StationInfo * Info;
  // callbacks
  void (*WordChangeDriver)  (WORD * ptr, WORD newdata);  // lze pouzit pro sitova volani
  void (*DisplayTextDriver) (char * str);
  
};
typedef struct PesApi_s PesApi_t;



#endif // MEMORY_DEF_H_
