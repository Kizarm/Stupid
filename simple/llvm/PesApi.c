#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "PesApi.h"

static void * pesHandle = NULL;

typedef void (*VHandlePR) (RamDef_t*);

VHandleV  PSimple;
VHandlePR PSetRamBasePtr;

int LoadLibPes (const char * name) {
  pesHandle = dlopen (name, RTLD_LAZY);
  if (!pesHandle) {
    fprintf (stderr,"Chyba: nejde otevrit knihovna \"%s\" - tip:\n"
    "export LD_LIBRARY_PATH=`pwd`\n", name);
    return -1;
  }
  dlerror();
  PSimple        = dlsym (pesHandle, "Simple");
  if (dlerror()) {
    fprintf (stderr, "Nelze najit [Simple()]\n");
    dlclose (pesHandle);
    return -1;
  }
  //-printf("PSimple:%p\n", PSimple);
  PSetRamBasePtr = dlsym (pesHandle, "SetRamBasePtr");
  if (dlerror()) {
    fprintf (stderr, "Nelze najit [SetRamBasePtr()]\n");
    dlclose (pesHandle);
    return -1;
  }
  //-printf("PSetRamBasePtr:%p\n", PSetRamBasePtr);
  PSetRamBasePtr(&Variables);
  printf ("Lib %s is open\n", name);
  return 0;
}
void ExitLibPes (void) {
  dlclose (pesHandle);
}

