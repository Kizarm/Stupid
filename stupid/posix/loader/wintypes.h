#ifndef _WINTYPES_H
#define _WINTYPES_H

#include <stdint.h>
#ifdef UNIX
typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
#else
#include <windows.h>
#endif

#endif // _WINTYPES_H
