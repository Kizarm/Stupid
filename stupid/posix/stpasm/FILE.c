#include <stdio.h>
//#include <io.h>
//#include <dos.h>

extern char* MakeShortFileName(char *srcfile);

/*---------------------------------------------------------------------------*/
/*	 MakeShortFileName - prepise dlouhy nazev souboru do formatu 8.3			  */
/*---------------------------------------------------------------------------*/
/*  parametry : file - dlouhy nazev souboru									        */
/*																			                    */
/*  vraci     : dlouhy nazev souboru prevedeny do kratkeho formatu 8.3		  */
/*---------------------------------------------------------------------------*/
extern char* MakeShortFileName(char *srcfile)
{
/*	if( access( srcfile, 0)) // jestlize soubor tato funkce neotevre ==>
									 // nejspis to bude dlouhy nazev souboru
	{
		// zjisti nazev ve formatu 8.3
		asm	push	ds
		asm	push	es
		_CL = 1;
		_CH = 0x80;
		_SI = FP_SEG( srcfile);
		_DS = _SI;
		_SI = FP_OFF( srcfile);
		_DI = FP_SEG( srcfile);
		_ES = _DI;
		_DI = FP_OFF( srcfile);
		_AX = 0x7160;
		asm	int	0x21;
		asm	pop	es
		asm	pop	ds
	}
*/	return srcfile;
}
