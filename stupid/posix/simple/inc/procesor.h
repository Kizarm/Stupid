/*###########################################################################*/
/*###########################################################################*/
/*                                                                           */
/*   implementace                                                            */
/*                                                                           */
/*###########################################################################*/
/*###########################################################################*/
#if !defined(AFX_PROCESOR_H__INCLUDED_)
#define AFX_PROCESOR_H__INCLUDED_

#include <stdio.h>
#include "List.h"

#define RAMORG  (0x1000L<<3)


extern      int NetAdr;
extern      int MaxSpInModule;
extern unsigned long RamPtr;
extern long int AutoPtr;
extern long int RomBegin,RamBegin;
extern unsigned long RomEnd, RamEnd;

/**
 * Abstraktní báze pro možnost rozšíření na jinou architekturu.
 * */

class BaseProcessor {
  public:
    BaseProcessor() {};
    virtual ~BaseProcessor() {};
    /*---------------------------------------------------------------------------*/
    /*   Pragma                                                                  */
    /*---------------------------------------------------------------------------*/
    virtual void c_PragmaLj()=0;
    virtual void c_PragmaSj()=0;

    /*---------------------------------------------------------------------------*/
    /*   Vetveni                                                                 */
    /*---------------------------------------------------------------------------*/

    virtual void c_if()=0;
    virtual void c_else()=0;
    virtual void c_endif()=0;

    virtual void c_caseof()=0;
    virtual void c_endcase()=0;


    /*---------------------------------------------------------------------------*/
    /*   Procedury a funkce                                                      */
    /*---------------------------------------------------------------------------*/
    virtual void c_procinit()=0;
    virtual void c_procend()=0;
    virtual void c_proced (LPOLOZKA * p, int pos)=0;
    virtual void c_func (LPOLOZKA * p, int pos)=0;
    virtual void c_preturn (LPOLOZKA * p)=0;
    virtual void c_freturn (LPOLOZKA * p)=0;
    virtual int  c_retval()=0;                     /*  offset vracene hodnoty na zas*/

    virtual void c_pcall (char * u)=0;             /*  volani procedury             */
    virtual void c_fcall (char * u)=0;             /*  volani funkce                */
    virtual void c_skpb()=0;                       /*  predani bitu jako skut.par.  */

    virtual void c_allot()=0;                      /*  vytvoreni prostoru na zas.   */

    virtual void c_procerror()=0;         /*  MK  */

    /*---------------------------------------------------------------------------*/
    /*   Prace s adresami                                                        */
    /*---------------------------------------------------------------------------*/

    virtual void c_ldbaim (unsigned long int a)=0; /*  load bit address             */
    virtual void c_ldaim (unsigned long int a)=0;  /*  load immediate addres        */
    virtual void c_ldbaa (unsigned long int a)=0;  /*  load bit address ze stacku   */
    virtual void c_ldaa (unsigned long int a)=0;   /*  load address ze stacku       */
    virtual void c_spba (unsigned long int a)=0;   /*  bit adr operandu na stacku   */
    virtual void c_spa (unsigned long int a)=0;    /*  adr operandu na stacku       */
    virtual void c_spar(unsigned long int a)=0;    /*  adr operandu na stacku - real*/


    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ BIT                                                     */
    /*---------------------------------------------------------------------------*/

    virtual void c_pushl()=0;                       /*  uloz bit na zasobnik        */
    //--virtual void c_popl()=0;                        /*  vyjmi bit ze zasobniku      */

    virtual void c_setbd (unsigned long int n)=0;   /*  nastav bit direct           */
    virtual void c_setbi()=0;                       /*  nastav bit indirect         */

    virtual void c_resbd (unsigned long int n)=0;   /*  nuluj  bit direct           */
    virtual void c_resbi()=0;                       /*  nuluj  bit indirect         */

    virtual void c_invbd (unsigned long int n)=0;   /*  neguj  bit direct           */
    virtual void c_invbi()=0;                       /*  neguj  bit indirect         */

    virtual void c_ldbd (unsigned long int n)=0;    /*  nacti  bit direct           */
    virtual void c_ldbi()=0;                        /*  nacti  bit indirect         */

    virtual void c_stbd (unsigned long int n)=0;    /*  uloz   bit direct           */
    virtual void c_stbi()=0;                        /*  uloz   bit indirect         */

    virtual void c_orbd (unsigned long int n)=0;    /*  OR     bit direct           */
    virtual void c_andbd (unsigned long int n)=0;   /*  AND    bit direct           */
    virtual void c_norbd (unsigned long int n)=0;   /*  OR    \bit direct           */
    virtual void c_nandbd (unsigned long int n)=0;  /*  AND   \bit direct           */

    virtual void c_orb()=0;                         /*  Logicky soucet              */
    virtual void c_andb()=0;                        /*  Logicky soucin              */
    virtual void c_xorb()=0;                        /*  Logicky xor                 */
    virtual void c_notb()=0;                        /*  Logicka negace              */

    virtual void c_retb()=0;                        /*  Vraceni BIT jako hod. fce   */


    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ WORD                                                    */
    /*---------------------------------------------------------------------------*/

    virtual void c_ldwim()=0;                      /*  load immediate operand       */
    virtual void c_ldwd (unsigned long int n)=0;   /*  load direct operand          */
    virtual void c_ldwi()=0;                       /*  load indirect operand        */

    virtual void c_stwd (unsigned long int n)=0;   /*  store direct operand         */
    virtual void c_stwi()=0;                       /*  store indirect operand       */

    virtual void c_addwim()=0;                     /*  pricteni immediate operandu  */
    virtual void c_addwd (unsigned long int n)=0;  /*  pricteni direct operandu     */
    virtual void c_addw()=0;                       /*  pricteni indirect operandu   */
    virtual void c_addwp()=0;

    virtual void c_subwim()=0;                     /*  odecteni immediate operandu  */
    virtual void c_subwd (unsigned long int n)=0;  /*  odecteni direct operandu     */
    virtual void c_subw()=0;                       /*  odecteni indirect operandu   */

    virtual void c_mulwim()=0;                     /*  nasobeni immediate oper.     */
    virtual void c_mulwd (unsigned long int n)=0;  /*  nasobeni direct oper.        */
    virtual void c_mulw()=0;                       /*  nasobeni indirect oper.      */

    virtual void c_divwim()=0;                     /*  deleni   immediate oper.     */
    virtual void c_divwd (unsigned long int n)=0;  /*  deleni   direct oper.        */
    virtual void c_divw()=0;                       /*  deleni   indirect oper.      */

    virtual void c_newim()=0;                      /*  immediate nerovno            */
    virtual void c_eqwim()=0;                      /*  immediate rovno              */
    virtual void c_stimmd (unsigned long)=0;  /*  immediate uloz do direct     */

    virtual void c_ltw()=0;                        /*  mensi                        */
    virtual void c_lew()=0;                        /*  mensi nebo rovno             */
    virtual void c_gtw()=0;                        /*  vetsi                        */
    virtual void c_gew()=0;                        /*  vetsi nebo rovno             */
    virtual void c_eqw()=0;                        /*  rovno                        */
    virtual void c_new()=0;                        /*  nerovno                      */

    virtual void c_wtor()=0;                       /*  konverze na REAL             */

    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ INT                                                     */
    /*---------------------------------------------------------------------------*/

    virtual void c_muliim()=0;                     /*  nasobeni immediate oper.     */
    virtual void c_mulid (unsigned long int n)=0;  /*  nasobeni direct oper.        */
    virtual void c_muli()=0;                       /*  nasobeni indirect oper.      */

    virtual void c_diviim()=0;                     /*  deleni   immediate oper.     */
    virtual void c_divid (unsigned long int n)=0;  /*  deleni   direct oper.        */
    virtual void c_divi()=0;                       /*  deleni   indirect oper.      */

    virtual void c_chsi()=0;                       /*  zmena znamenka               */
    virtual void c_absi()=0;                       /*  absolutni hodnota            */

    virtual void c_swap()=0;            /*  prohozeni wordu              */

    virtual void c_lti()=0;                        /*  mensi                        */
    virtual void c_lei()=0;                        /*  mensi nebo rovno             */
    virtual void c_gti()=0;                        /*  vetsi                        */
    virtual void c_gei()=0;                        /*  vetsi nebo rovno             */

    virtual void c_itor()=0;                       /*  konverze na REAL             */

    virtual void c_disp (int)=0;                   /*  zobrazeni                    */
    virtual void c_dispb()=0;           /*  zobrazeni                    */
    virtual void c_prnt (int)=0;                   /*  tisk                         */
    virtual void c_prntb()=0;                      /*  tisk                         */

    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ REAL                                                    */
    /*---------------------------------------------------------------------------*/

    virtual void c_ldrim()=0;                      /*  load immediate operand       */
    virtual void c_ldri()=0;                       /*  load indirect operand        */

    virtual void c_stri()=0;            /*  store indirect operand       */

    virtual void c_addr()=0;                       /*  pricteni indirect operandu   */
    virtual void c_subr()=0;                       /*  odecteni indirect operandu   */
    virtual void c_mulr()=0;                       /*  nasobeni indirect oper.      */
    virtual void c_divr()=0;                       /*  deleni   indirect oper.      */

    virtual void c_chsr()=0;                       /*  zmena znamenka               */
    virtual void c_absr()=0;                       /*  absolutni hodnota            */
    virtual void c_truncr()=0;                     /*  cela cast                    */
    //--virtual void c_fracr()=0;                      /*  necela cast                  */

    virtual void c_ltr()=0;                        /*  mensi                        */
    virtual void c_ler()=0;                        /*  mensi nebo rovno             */
    virtual void c_gtr()=0;                        /*  vetsi                        */
    virtual void c_ger()=0;                        /*  vetsi nebo rovno             */
    virtual void c_eqr()=0;                        /*  rovno                        */
    virtual void c_ner()=0;                        /*  nerovno                      */

    virtual void c_rtoi()=0;                       /*  konverze na INT              */
    virtual void c_rtow()=0;                       /*  konverze na WORD             */

    virtual void c_andwim()=0;                     /*  and immediate oper.     */
    virtual void c_andwd (unsigned long int n)=0;  /*  and direct oper.        */
    virtual void c_andw()=0;                       /*  and indirect oper.      */
    virtual void c_orwim()=0;                     /*  or immediate oper.     */
    virtual void c_orwd (unsigned long int n)=0;  /*  or direct oper.        */
    virtual void c_orw()=0;                       /*  or indirect oper.      */
    virtual void c_xorwim()=0;                     /*  xor immediate oper.     */
    virtual void c_xorwd (unsigned long int n)=0;  /*  xor direct oper.        */
    virtual void c_xorw()=0;                       /*  xor indirect oper.      */

    /*---------------------------------------------------------------------------*/
    /*   Zobrazeni retezce                                                       */
    /*---------------------------------------------------------------------------*/

    virtual void c_testpole()=0;                   /*  otestovani indexu na max     */
    virtual void c_disps (char * u)=0;             /*  zobrazeni textove konstanty  */
    virtual void c_prnts (char * u)=0;             /*  tisk      textove konstanty  */

    /*---------------------------------------------------------------------------*/
    /*   Prace s virtuali pameti                                                  */
    /*---------------------------------------------------------------------------*/

    virtual void c_memwrite()=0;                   /*  primy zapis do pameti        */
    virtual void c_memread()=0;                    /*  prime cteni z pameti         */

    /*---------------------------------------------------------------------------*/

    virtual void c_unknown (char * u)=0;           /*  kdyz nerozumi prikazu        */

};
extern FILE  * mf;
extern BaseProcessor * BaseProcessorPtr;

#endif // AFX_PROCESOR_H__INCLUDED_
