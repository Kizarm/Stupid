#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "PesApi.h"

static void * pesHandle = NULL;

typedef void (*VHandlePR) (RamDef_t*);

VHandleV  PSimple;
VHandlePR PSetRamBasePtr;

static PesApi_t * API;
// lokalni kopie
StationInfo GStationInfo;

int LoadLibPes (const char * name) {
  pesHandle = dlopen (name, RTLD_LAZY);
  if (!pesHandle) {
    fprintf (stderr,"Chyba: nejde otevrit knihovna \"%s\" - tip:\n"
    "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`\n", name);
    return -1;
  }
  dlerror();
  PAHandleV ApiFunc;
  ApiFunc = dlsym (pesHandle, "GetPesApiPtr");
  
  if (dlerror()) {
    fprintf (stderr, "Nelze najit [GetPesApiPtr]\n");
    dlclose (pesHandle);
    return -1;
  }
  API = ApiFunc ();
  PSimple        = API->MainPass;
  PSetRamBasePtr = API->SetRamBase;
  memcpy (&GStationInfo, API->Info, sizeof (StationInfo));
  /*
  printf ("Station adr = %d, RAMBEGIN=%04X, RAMEND=%04X\n",
    GStationInfo.NetAddr, GStationInfo.DataBegin, GStationInfo.DataEnd);
  printf ("%p, %p, %p, %p\n", API, PSimple, PSetRamBasePtr, API->Info);
  */
  if (PSetRamBasePtr) PSetRamBasePtr (& Variables);
  // set callbacks
  API->WordChangeDriver  = ApiNetDriver;
  API->DisplayTextDriver = ApiDispText;
  return 0;
}
void ExitLibPes (void) {
  dlclose (pesHandle);
}

