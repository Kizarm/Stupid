#include "libwasm.h"


void* operator new (unsigned long size) {
  return calloc(1,size);
}
void operator delete (void* p) {
  free (p);
}

void* operator new[] (unsigned long size) {
  return calloc(1,size);
}

void operator delete[] (void* p) {
  free (p);
}

void operator delete (void* p, unsigned size) {
  (void) size;
  free (p);
}

