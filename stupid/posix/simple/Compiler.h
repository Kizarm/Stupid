#ifndef _SIMLE_H_DEFINED
#define _SIMLE_H_DEFINED

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * @file
 * @brief API překladače jazyka SIMPLE.
 * 
 * Základ byl převzat z původních zdrojáků Hypel, ale jsou tam dva velké problémy :
 * -1. Jazyk nemá bezkontextovou gramatiku, původní parser je dost složitý, poplatný době.
 * -2. Celá původní aritmetika je vlastně dělána ve velkém indiánu.
 * 
 * Endianita je fakticky hodně divná a protože mě tohle na začátku vůbec nenapadlo,
 * nedá se to už předělat, protože když se do toho hrábne na jedné straně, podělá
 * se něco jiného úplně jinde. Vlastně vadí jen digitální vstupy a výstupy, nesedí
 * pak bitové adresy pro původní zdrojáky. Pro DX0, DX1, DY0, DY1 udělám patch,
 * který bude v simulaci odpovídat původním zdrojákům. Ale není to systémové řešení.
 * 
 * Vlastní překladač nejde dotáhnout do konce, protože původní koncepce počítala
 * s ukládáním na zásobník po bitech. To nejde dostatečně rozumně udělat pro obecný
 * procesor. Takže chybí funkce s bitovými parametry, ale to se stejně moc nepoužívá.
 * */
  
enum LLVMTypeMachine {
  MachineTypeLinux64,
  MachineTypeCortexM0,
  MachineTypeCortexM3,
  MachineTypeCortexM4F,
  MachineTypeWasm,
  LLVMTypesMax
};

union _CompilerFlags {
  struct _F {
    enum LLVMTypeMachine TGT : 8;
    unsigned RAM : 16;        //! /RAM
    unsigned L : 1;           //! /L
    unsigned M : 1;           //! /M
    unsigned S : 1;           //! /S
    unsigned V : 1;           //! /V
    unsigned O : 1;           //! /O
  } F;
  unsigned Common;
};
typedef union _CompilerFlags CompilerFlags;

#define COMPILER_API

/**
 * @brief Hlavní API funkce překladače.
 *
 * Kód je kompatibilní s ANSI C, nepodporuje parametry příkazové řádky, tedy umožní jen
 * překlad jednoho souboru do formátu *.dnl
 * 
 * @param arg Zatím jen jednoduché jméno souboru např. arg="cosi.stp", case sensitive.
 * @param flags místo /Cosi z příkazového řádku - nahrazuje jej CompilerFlags::Common
 * @return int 0 při úspěchu
 */
extern COMPILER_API int Simple (const char * arg, const unsigned flags);
/**
* @brief API pro logování např. do souboru.
*
* Příklad použití:
* @code
    #include <stdio.h>
    #include "Compiler.h"
    FILE * logfile;

    void Logging (const char * buff) {
      fprintf(logfile, "%s", buff);
    }

    int main (int argc,char * argv[]) {
      int result;
      if (argc < 2) return -1;
      logfile = fopen ("Simple.log", "w");
      LogFce  = Logging;
      result  = Simple(argv[1], 0);
      fclose (logfile);
      return result;
    }
* @endcode
*
*/
extern COMPILER_API void (*LogFce) (const char * buff);
extern COMPILER_API char LastOpenedFile[80];
//-extern COMPILER_API bool config_error; /// dodelat do pure C ?

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _SIMLE_H_DEFINED
