/*###########################################################################*/
/*    FUNKCE PRO PRACI S KONFIGURACNIM SOUBOREM                              */
/*###########################################################################*/


//-#include "StdAfx.h"
#include <string.h>
#include <stdlib.h>
#include <list>
#include "inc/CfgTypes.h"

#include "inc/Znak.h"
#include "inc/List.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/fce.h"
#include "inc/error.h"
#include "inc/procesor.h"
#include "inc/firmware.h"

using namespace std;

list <TAG_CONFIGURATION>  Configurations;
TAG_CONFIGURATION config;

void Parameters();


void ReadConfigFile (char * name) {
  char  cfname[255];

  FileInit (name);

  je();
  {
    Mezery();
    _sled();
    do {
      KlSlovo ("[");
      if (zrtz())  {
        Retezec (cfname);
        if (pkk())  {
          config.name = strdup (cfname);
          KlSlovo ("]");
          if (pkk())  {
            Parameters();
          }
        }
      }
    } while (sled_());
  }
  vse();

  (*FSez[ZnakFn].cl) ();
}

void Parameters() {
  config.data_begin = -1;
  config.data_end = -1;
  config.program_begin = -1;
  config.program_end = -1;

  je();
  _sled();
  do {
    KlSlovo ("DATABEGIN");
    if (pkk()) {
      KlSlovo ("=");
      if (zrtz()) {
        Cislo();
        if (pkk()) {
          config.data_begin = SemPop() << 3;
        }
      }
    }
    if (neb()) {
      KlSlovo ("DATAEND");
      if (pkk()) {
        KlSlovo ("=");
        if (zrtz()) {
          Cislo();
          if (pkk()) {
            config.data_end = SemPop();
          }
        }
      }
      if (neb()) {
        KlSlovo ("PROGRAMBEGIN");
        if (pkk()) {
          KlSlovo ("=");
          if (zrtz()) {
            Cislo();
            if (pkk()) {
              config.program_begin = SemPop();
            }
          }
        }
        if (neb()) {
          KlSlovo ("PROGRAMEND");
          if (pkk()) {
            KlSlovo ("=");
            if (zrtz()) {
              Cislo();
              if (pkk()) {
                config.program_end = SemPop();
              }
            }
          }
        }
      }
    }
  } while (sled_());
  vse();

  Configurations.push_back (config);
}

bool SetConfig (char * name) {
  list<TAG_CONFIGURATION>::iterator i;

  for (i = Configurations.begin(); i != Configurations.end(); i++) {
    if (!strcmp ( (*i).name, name))
      break;
  }

  if (i == Configurations.end())
    return false;

  if ( (*i).data_begin >= 0) {
    RamBegin = (*i).data_begin;
    RamPtr = RamBegin;
  }
  if ( (*i).data_end >= 0) RamEnd= (*i).data_end;
  if ( (*i).program_begin >= 0)  RomBegin = (*i).program_begin;
  if ( (*i).program_end >= 0)  RomEnd = (*i).program_end;

  return true;
}

void FreeConfig() {
  while (!Configurations.empty()) {
    free (Configurations.back().name);
    Configurations.pop_back();
  }
}
