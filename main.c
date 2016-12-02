#include <stdio.h>
#include <string.h>
#include "Compiler.h"

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
  if (argc < 2) return -1;
  
  LogFce  = Logging;
  CompilerFlags cf;
  cf.Common = 0U;
  cf.F.O = cf.F.L = 1U;     // nastav potrebne
  
  if (argc > 2) {
  if      (!strcmp (argv[2],"i")) cf.F.TGT = MachineTypeLinux64;
  else if (!strcmp (argv[2],"c")) cf.F.TGT = MachineTypeCortexM3;
  else if (!strcmp (argv[2],"f")) cf.F.TGT = MachineTypeCortexM4F;
  else                            cf.F.TGT = LLVMTypesMax;
  } else {
                                  cf.F.TGT = LLVMTypesMax;
  }
  result = Simple(argv[1] , cf.Common);
  return result;
}
