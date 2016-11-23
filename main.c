#include <stdio.h>
#include <string.h>
#include "Compiler.h"
/*
int find_bounding_arrea (const char * src, const char * dst, const int n) {
  const char * end_arrea = dst + n;
  if ((src > dst) && (src < end_arrea)) {
    printf ("Oblasti %p a %p se prekryvaji\n", src, dst);
    return 1;
  }
  return 0;
}
void BreakPoint (void) {
  volatile int x = 0;
  (void) x;
}
extern size_t strlen (const char * str); 
char * strcpy (char * dst, const char * src) {
  const size_t n = strlen (src);
  if (find_bounding_arrea (src, dst, n)) {
    BreakPoint();
  }
  int i;
  for (i=0; i<n; i++) dst[i] = src[i];
  dst[n] = '\0';
  return dst;
}
*/
  #define SRED   "\x1B[31;1m"
  #define SGREEN "\x1B[32;1m"
  #define SBLUE  "\x1B[34;1m"
  #define SDEFC  "\x1B[0m"

void Logging (const char *buff) {
  //fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  fprintf(stderr, SBLUE"%s"SDEFC, buff);
}

int main (int argc,char *argv[]) {
  int result;
  //if (argc < 2) return -1;
  
  LogFce  = Logging;
  CompilerFlags cf;
  cf.Common = 0U;
  cf.F.O = cf.F.L = 1U;     // nastav potrebne
  //result  = Simple (argv[1], cf.Common);
  
  if      (!strcmp (argv[1],"i")) cf.F.TGT = MachineTypeLinux64;
  else if (!strcmp (argv[1],"c")) cf.F.TGT = MachineTypeCortexM3;
  else if (!strcmp (argv[1],"f")) cf.F.TGT = MachineTypeCortexM4F;
  else                            cf.F.TGT = LLVMTypesMax;
  //-printf ("CF=0x%08X\n", cf.Common);
  
  cf.F.TGT = MachineTypeLinux64;
  if (argc < 3) {
    result  = Simple("Test.stp", cf.Common);
  } else {
    result  = Simple(argv[2]   , cf.Common);
  }
  return result;
}
