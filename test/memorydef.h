#ifndef MEMORY_DEF_H_
#define MEMORY_DEF_H_

#define RAMLEN 0x800

typedef unsigned char  BYTE;
typedef unsigned short WORD;

union PR2PW_u {
  float * PR;
  WORD  * PW;
};
typedef union PR2PW_u PR2PW;

union RamDef_u {
  BYTE Bytes [RAMLEN >> 0];
  WORD Words [RAMLEN >> 1];
};
typedef union RamDef_u RamDef_t;

struct PesApi_s {
  // API
  void (*SetRamBase) (RamDef_t * ptr);
  void (*MainPass)   (void);
  // callbacks
  void (*WordChangeDriver) (WORD * ptr, WORD newdata);  // lze pouzit pro sitova volani
  
};
typedef struct PesApi_s PesApi_t;



#endif // MEMORY_DEF_H_
