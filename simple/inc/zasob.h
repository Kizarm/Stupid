/*-----------------------------##############-------------------------------*/
/*  externaly z lexikonu       ## ZASOBNIK ##                               */
/*-----------------------------##############-------------------------------*/

#if !defined(AFX_ZASOB_H__INCLUDED_)
#define AFX_ZASOB_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "List.h"

#define  SSL      (4000)

extern char *str_uk;               /* ukazatel do semantickeho retezce      */
extern void ZasInit()  ;           /* inicialisace lexikonu                 */

                                   /*  Operace na semantickem zasobniku     */
extern void    SemPush(ATRIBUT atr) ;
extern ATRIBUT SemPop();
extern ATRIBUT SemPick(int offset);
extern ATRIBUT *SemPtr(int offset);
extern void    SemDrop(int pocet);
extern int	   SemPocetAtr();
extern void    StrAdd (const char * s) ;
extern void    StrPush(const char *s) ;
extern void    StrDrop();
extern void    StrPop (char *s) ;
extern void    StrApp ();
extern float   RSemPop();
extern void    RSemPush( float f ) ;
                                  /*  Operace s priznaky                   */
extern int uspech  ;              /* promenna s priznakem                  */
extern void Pravda()    ;         /* predej priznak pravdy                 */
extern void Nepravda()  ;         /* predej priznak nepravdy               */
extern void Opacne()    ;         /* neguj hodnotu priznaku                */

                                  /*  funkce pro definici syntaxe          */
extern void je()        ;         /* na tyto funkce je vazan konverzni     */
extern int  zrtz()      ;         /* program EXPAND_C.PRG , ktery je sam   */
extern int  neb()       ;         /* zarazuje do zdrojoveho textu. Nejsou  */
extern void vse()       ;         /* tedy urceny k volnemu uzivani. Viz.   */
extern void _sled()     ;         /* popis konverzniho programu.           */
extern int  sled_()     ;         /*                                       */
extern int  pkk()       ;         /*                                       */

#define pravda()  Pravda() // MK
#define nepravda()  Nepravda() // MK
#define mezery()  Mezery() // MK
#define nic()  pravda()


#endif // AFX_ZASOB_H__INCLUDED_