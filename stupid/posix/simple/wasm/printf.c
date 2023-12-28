#include "libwasm.h"
extern void IMPORT(PrintOut) (const char * ptr, const int len);  // external javascript function
int puts (const char * ptr) {
  const int len = strlen (ptr);
  char * out = malloc (len);
  strncpy(out, ptr, len);
  PrintOut (out, len);
  free (out);
  return len;
}
#define MAXSTRINGSIZE 0x1000
int printf (const char *fmt, ...) {
  char * p = malloc(MAXSTRINGSIZE);
  va_list ap;
  va_start(ap, fmt);
  int size = vsnprintf(p, MAXSTRINGSIZE, fmt, ap);
  va_end(ap);
  if (size < 0) return 0;
  p [size] = '\0';
  PrintOut(p, size);
  free(p);
  return size;
}
signed int snprintf (char *pStr, size_t length, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int size = vsnprintf(pStr, length, fmt, ap);
  va_end(ap);
  if (size < 0) return 0;
  pStr [size] = '\0';
  return size;
}
