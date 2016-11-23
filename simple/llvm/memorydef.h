#ifndef MEMORY_DEF_H_
#define MEMORY_DEF_H_

#define RAMLEN 0x800
/*
struct PredefTypes_s {
};
*/
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

#endif // MEMORY_DEF_H_
