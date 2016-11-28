#ifndef LLVMPROCESSOR_H
#define LLVMPROCESSOR_H

#include "procesor.h"
#include "../Compiler.h"
#include "../llvmcomponent.h"

class LLVMProcessor : public BaseProcessor {
  public:
    LLVMProcessor(LLVMTypeMachine t = MachineTypeLinux64);
    virtual ~LLVMProcessor();
    /*---------------------------------------------------------------------------*/
    /*   Pragma                                                                  */
    /*---------------------------------------------------------------------------*/

    void c_PragmaLj();
    void c_PragmaSj();

    /*---------------------------------------------------------------------------*/
    /*   Vetveni                                                                 */
    /*---------------------------------------------------------------------------*/

    void c_if();
    void c_else();
    void c_endif();

    void c_caseof();
    void c_endcase();


    /*---------------------------------------------------------------------------*/
    /*   Procedury a funkce                                                      */
    /*---------------------------------------------------------------------------*/
    void c_procinit();
    void c_proced (LPOLOZKA * p, int pos);
    void c_procend();
    void c_func (LPOLOZKA * p, int pos);
    void c_preturn (LPOLOZKA * p);
    void c_freturn (LPOLOZKA * p);
    void c_pcall (char * u);             /*  volani procedury             */
    void c_fcall (char * u);             /*  volani funkce                */
    void c_skpb();                       /*  predani bitu jako skut.par.  */

    void c_allot();                      /*  vytvoreni prostoru na zas.   */
    int  c_retval();                     /*  offset vracene hodnoty na zas*/

    void c_procerror();         /*  MK  */

    /*---------------------------------------------------------------------------*/
    /*   Prace s adresami                                                        */
    /*---------------------------------------------------------------------------*/

    void c_ldbaim (unsigned long int a); /*  load bit address             */
    void c_ldaim (unsigned long int a);  /*  load immediate addres        */
    void c_ldbaa (unsigned long int a);  /*  load bit address ze stacku   */
    void c_ldaa (unsigned long int a);   /*  load address ze stacku       */
    void c_spba (unsigned long int a);   /*  bit adr operandu na stacku   */
    void c_spa (unsigned long int a);    /*  adr operandu na stacku       */
    void c_spar (unsigned long int a);    /*  adr operandu na stacku  real*/


    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ BIT                                                     */
    /*---------------------------------------------------------------------------*/

    void c_pushl();                       /*  uloz bit na zasobnik        */

    void c_setbd (unsigned long int n);   /*  nastav bit direct           */
    void c_setbi();                       /*  nastav bit indirect         */

    void c_resbd (unsigned long int n);   /*  nuluj  bit direct           */
    void c_resbi();                       /*  nuluj  bit indirect         */

    void c_invbd (unsigned long int n);   /*  neguj  bit direct           */
    void c_invbi();                       /*  neguj  bit indirect         */

    void c_ldbd (unsigned long int n);    /*  nacti  bit direct           */
    void c_ldbi();                        /*  nacti  bit indirect         */

    void c_stbd (unsigned long int n);    /*  uloz   bit direct           */
    void c_stbi();                        /*  uloz   bit indirect         */

    void c_orbd (unsigned long int n);    /*  OR     bit direct           */
    void c_andbd (unsigned long int n);   /*  AND    bit direct           */
    void c_norbd (unsigned long int n);   /*  OR    \bit direct           */
    void c_nandbd (unsigned long int n);  /*  AND   \bit direct           */

    void c_orb();                         /*  Logicky soucet              */
    void c_andb();                        /*  Logicky soucin              */
    void c_xorb();                        /*  Logicky xor                 */
    void c_notb();                        /*  Logicka negace              */

    void c_retb();                        /*  Vraceni BIT jako hod. fce   */


    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ WORD                                                    */
    /*---------------------------------------------------------------------------*/

    void c_ldwim();                      /*  load immediate operand       */
    void c_ldwd (unsigned long int n);   /*  load direct operand          */
    void c_ldwi();                       /*  load indirect operand        */

    void c_stwd (unsigned long int n);   /*  store direct operand         */
    void c_stwi();                       /*  store indirect operand       */

    void c_addwim();                     /*  pricteni immediate operandu  */
    void c_addwd (unsigned long int n);  /*  pricteni direct operandu     */
    void c_addw();                       /*  pricteni indirect operandu   */
    void c_addwp();

    void c_subwim();                     /*  odecteni immediate operandu  */
    void c_subwd (unsigned long int n);  /*  odecteni direct operandu     */
    void c_subw();                       /*  odecteni indirect operandu   */

    void c_mulwim();                     /*  nasobeni immediate oper.     */
    void c_mulwd (unsigned long int n);  /*  nasobeni direct oper.        */
    void c_mulw();                       /*  nasobeni indirect oper.      */

    void c_divwim();                     /*  deleni   immediate oper.     */
    void c_divwd (unsigned long int n);  /*  deleni   direct oper.        */
    void c_divw();                       /*  deleni   indirect oper.      */

    void c_newim();                      /*  immediate nerovno            */
    void c_eqwim();                      /*  immediate rovno              */
    void c_stimmd (unsigned long);  /*  immediate uloz do direct     */

    void c_ltw();                        /*  mensi                        */
    void c_lew();                        /*  mensi nebo rovno             */
    void c_gtw();                        /*  vetsi                        */
    void c_gew();                        /*  vetsi nebo rovno             */
    void c_eqw();                        /*  rovno                        */
    void c_new();                        /*  nerovno                      */

    void c_wtor();                       /*  konverze na REAL             */

    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ INT                                                     */
    /*---------------------------------------------------------------------------*/

    void c_muliim();                     /*  nasobeni immediate oper.     */
    void c_mulid (unsigned long int n);  /*  nasobeni direct oper.        */
    void c_muli();                       /*  nasobeni indirect oper.      */

    void c_diviim();                     /*  deleni   immediate oper.     */
    void c_divid (unsigned long int n);  /*  deleni   direct oper.        */
    void c_divi();                       /*  deleni   indirect oper.      */

    void c_chsi();                       /*  zmena znamenka               */
    void c_absi();                       /*  absolutni hodnota            */

    void c_swap();            /*  prohozeni wordu              */

    void c_lti();                        /*  mensi                        */
    void c_lei();                        /*  mensi nebo rovno             */
    void c_gti();                        /*  vetsi                        */
    void c_gei();                        /*  vetsi nebo rovno             */

    void c_itor();                       /*  konverze na REAL             */

    void c_disp (int);                   /*  zobrazeni                    */
    void c_dispb();           /*  zobrazeni                    */
    void c_prnt (int);                   /*  tisk                         */
    void c_prntb();                      /*  tisk                         */

    /*---------------------------------------------------------------------------*/
    /*   Operace pro typ REAL                                                    */
    /*---------------------------------------------------------------------------*/

    void c_ldrim();                      /*  load immediate operand       */
    void c_ldri();                       /*  load indirect operand        */

    void c_stri();            /*  store indirect operand       */

    void c_addr();                       /*  pricteni indirect operandu   */
    void c_subr();                       /*  odecteni indirect operandu   */
    void c_mulr();                       /*  nasobeni indirect oper.      */
    void c_divr();                       /*  deleni   indirect oper.      */

    void c_chsr();                       /*  zmena znamenka               */
    void c_absr();                       /*  absolutni hodnota            */
    void c_truncr();                     /*  cela cast                    */
    //void c_fracr();                      /*  necela cast                  */

    void c_ltr();                        /*  mensi                        */
    void c_ler();                        /*  mensi nebo rovno             */
    void c_gtr();                        /*  vetsi                        */
    void c_ger();                        /*  vetsi nebo rovno             */
    void c_eqr();                        /*  rovno                        */
    void c_ner();                        /*  nerovno                      */

    void c_rtoi();                       /*  konverze na INT              */
    void c_rtow();                       /*  konverze na WORD             */

    void c_andwim();                     /*  and immediate oper.     */
    void c_andwd (unsigned long int n);  /*  and direct oper.        */
    void c_andw();                       /*  and indirect oper.      */
    void c_orwim();                     /*  or immediate oper.     */
    void c_orwd (unsigned long int n);  /*  or direct oper.        */
    void c_orw();                       /*  or indirect oper.      */
    void c_xorwim();                     /*  xor immediate oper.     */
    void c_xorwd (unsigned long int n);  /*  xor direct oper.        */
    void c_xorw();                       /*  xor indirect oper.      */

    /*---------------------------------------------------------------------------*/
    /*   Zobrazeni retezce                                                       */
    /*---------------------------------------------------------------------------*/

    void c_testpole();                   /*  otestovani indexu na max     */
    void c_disps (char * u);             /*  zobrazeni textove konstanty  */
    void c_prnts (char * u);             /*  tisk      textove konstanty  */

    /*---------------------------------------------------------------------------*/
    /*   Prace s  i pameti                                                  */
    /*---------------------------------------------------------------------------*/

    void c_memwrite();                   /*  primy zapis do pameti        */
    void c_memread();                    /*  prime cteni z pameti         */

    /*---------------------------------------------------------------------------*/

    void c_unknown (char * u);           /*  kdyz nerozumi prikazu        */
  protected:
    void c_popl();                        /*  vyjmi bit ze zasobniku      */

    void PopLbl (void);
    /*
    void PreDefinitions (void);
    void PreDefinition  (const OperationCodeTexts & text);
    */
    void logOpWpop (const char * op);
    void mathOpIM  (const char * op);
    void mathOpRE  (const char * op);
    void mathOpWDS (const char * op);
    void mathOpWD  (const char * op, unsigned long adr);
    void logOpBD   (const char * op, bool inv, unsigned long adr);
    
    void relOpWD   (const char * op);
    void relOpWDIM (const char * op);
    void relOpRE   (const char * op);
    
    struct conv_s {
      const char * from;
      const char * to;
      int  align_from;
      int  align_to;
    };
    static const conv_s RetypeConversions[];
    void Convert   (const conv_s * ct, const char * util);
    
  private:
    const LLVMTypeMachine m_Type;
    const int maxstrs;
    
    unsigned long LblNum;
    unsigned ptrSize;
    char * iFmt;

    //FILE * procFile;
    FILE * tempFile;
    int ProcNum;
    int FuncNum;
    
    bool fromField;
    
    llfnc  * mainfunc;
    llfnci * funciner;
    llt      texts;
};

#endif // LLVMPROCESSOR_H
