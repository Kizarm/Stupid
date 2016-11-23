/*---------------------------------############------------------------------*/
/*                                 ## ROMRAM ##                              */
/*---------------------------------############------------------------------*/
#if !defined(AFX_ROMRAM_H__INCLUDED_)
#define AFX_ROMRAM_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef unsigned long int ROMADR  ;
typedef unsigned long int RAMADR  ;
typedef unsigned      int UWORD   ;
typedef unsigned      int UBYTE   ;

extern ROMADR rom_z     ;        /* zacatek pameti ROM                       */
extern ROMADR rom_kod   ;        /* zacatek oblasti prekladu                 */
extern ROMADR rom_instr ;        /* zacatek oblasti instrukci virt. proc     */
extern ROMADR rom_del   ;        /* delka pameti ROM                         */
extern ROMADR rom_p     ;        /* ukazovatko v ROM                         */

extern RAMADR ram_z     ;        /* zacatek pameti RAM                       */
extern RAMADR ram_k     ;        /* konec pameti RAM                         */
extern RAMADR ram_p     ;        /* ukazovatko v RAM                         */
extern RAMADR ram_dzs   ;        /* delka zasobniku pro jeden proces         */

extern void   RamDej(RAMADR Del);               /* vyzadani RAM pro simulaci */
extern void   RamFree();                        /* vraceni  RAM po  simulaci */
extern void   RamAlot(RAMADR Del);              /* alokace v RAM             */
extern void   RamUw(UWORD Word, RAMADR Adr);    /* ulozeni slova do RAM      */
extern UWORD  RamVw(RAMADR Adr);                /* vybrani slova z  RAM      */
extern void   RamUb(UBYTE Byte, RAMADR Adr);    /* ulozeni bytu do  RAM      */
extern UBYTE  RamVb(RAMADR Adr) ;               /* vybrani bytu z adresy RAM */

extern void   RomDej(ROMADR Del);               /* vyzadani ROM pro simulaci */
extern void   RomFree();                        /* vraceni  ROM po  simulaci */
extern void   RomAlot(ROMADR Del);              /* alokace v ROM             */
extern void   RomUw(UWORD Word, ROMADR Adr);    /* ulozeni slova do ROM      */
extern UWORD  RomVw(ROMADR Adr);                /* vybrani slova z  ROM      */
extern void   RomUb(UBYTE Byte, ROMADR Adr);    /* ulozeni bytu do  ROM      */
extern UBYTE  RomVb(ROMADR Adr) ;               /* vybrani bytu z adresy ROM */
extern void   RomCw(UWORD w);
extern void   RomCb(UBYTE b);

#endif //AFX_ROMRAM_H__INCLUDED_