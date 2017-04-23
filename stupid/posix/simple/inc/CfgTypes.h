/*###########################################################################*/
/*    TYPY PRO PRACI S KONFIGURACNIM SOUBOREM                                */
/*###########################################################################*/


//#ifndef CFG_TYPES_H
//#define CFG_TYPES_H

#include <list>

struct TAG_CONFIGURATION
{
	char	*name;
	int		data_begin;
	int		data_end;
	int		program_begin;
	int		program_end;
};

// externy 

void ReadConfigFile(char *name);
bool SetConfig(char *name);
void FreeConfig();



//#endif // #ifndef CFG_TYPES_H