#include "llcapi.h"

int main(/*int argc, char **argv*/ void) {
  return CompileLLtoASFile("Test.mac.ll","tm.s", MachineTypeCortexM4F);
}


