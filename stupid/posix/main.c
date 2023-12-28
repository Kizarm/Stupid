#include <stdio.h>
#include <string.h>
#include "Compiler.h"

  #define SRED   "\x1B[31;1m"
  #define SGREEN "\x1B[32;1m"
  #define SBLUE  "\x1B[34;1m"
  #define SDEFC  "\x1B[0m"

void Logging (const char *buff) {
  //fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  fprintf(stderr, SGREEN"%s"SDEFC, buff);
}

int main (int argc,char *argv[]) {
  int result;
  
  if (argc < 2) {
    fprintf(stdout, "Pouziti:\n\t%s filename.stp [|i|m|c|f|w]\n", argv[0]);
    fprintf(stdout, " i target native code (*.dll,*.so)\n");
    fprintf(stdout, " m target ARM Cortex-M0\n");
    fprintf(stdout, " c target ARM Cortex-M3\n");
    fprintf(stdout, " f target ARM Cortex-M4F (native floating half size)\n");
    fprintf(stdout, " w target WASM (+ files for WEB emulation)\n");
    fprintf(stdout, " no parameter - old x52 version\n");
    return 0;
  }
  
  LogFce  = Logging;
  CompilerFlags cf;
  cf.Common = 0U;
  cf.F.O = cf.F.L = 1U;     // nastav potrebne
  
  if (argc > 2) {
  if      (!strcmp (argv[2],"i")) cf.F.TGT = MachineTypeLinux64;
  else if (!strcmp (argv[2],"m")) cf.F.TGT = MachineTypeCortexM0;
  else if (!strcmp (argv[2],"c")) cf.F.TGT = MachineTypeCortexM3;
  else if (!strcmp (argv[2],"f")) cf.F.TGT = MachineTypeCortexM4F;
  else if (!strcmp (argv[2],"w")) cf.F.TGT = MachineTypeWasm;
  else                            cf.F.TGT = LLVMTypesMax;
  } else {
                                  cf.F.TGT = LLVMTypesMax;
  }
  
  //cf.F.TGT = LLVMTypesMax;
  //result = Simple("Test.stp" , cf.Common);
  result = Simple(argv[1] , cf.Common);
  return result;
}
