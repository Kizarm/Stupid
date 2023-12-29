#include "llcapi.h"
/* Nepoužito, API se moc mění, ale nějak funguje
 * Ve své podstatě nahradí jen llc, může vyhodit rovnou
 * objekt, není potřeba ani assembler. Ale je to dost vachrlaté.
 */
int main(/*int argc, char **argv*/ void) {
  return CompileLLtoASFile("Test.mac.ll","tm.o", MachineTypeWasm);
}


