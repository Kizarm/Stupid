#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "inc/Znak.h"
//#include "inc/list.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/LLVMProcessor.h"
#include "inc/firmware.h"
#include "inc/fce.h"
#include "inc/chyby.h"

using namespace std;

static vector<lls> bodyFunctions;
//#define trace(...)
static void trace (const char * fmt, ...) {
  const int buflen = 0x400;
  char buf [buflen];
  va_list arglist;
  va_start (arglist, fmt);
  vsnprintf (buf, buflen, fmt, arglist);
  va_end (arglist);
  
//  printf  ("%s", buf);
  *currentFunction << ";" << buf;
}
static lle lastw;
static lle lastb;
static lle lindex;

extern char   InpF[80];
extern char   MacF[80];
extern char   SymF[80];
extern char   ObjF [80];
extern char   HexF[80];
extern char   DnlF[80];
extern char   SupF[80];
extern int    ListF,MCodeF,SrcF,VerbF;
extern FILE  * of;

//static  const char * ramdef = "%union.RamDef_u";

struct LLVMTexts {
  const char * prefix;
  const char * suffix;
};
static const LLVMTexts pst[LLVMTypesMax] = {
  {
    "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"\ntarget triple = \"x86_64-pc-linux-gnu\"\n",
    
    "attributes #0 = { nounwind uwtable \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"false\" \"no-infs-fp-math\"=\"false\""
    "\"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" \"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n"
    "attributes #1 = { \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"false\" \"no-infs-fp-math\"=\"false\" "
    "\"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" \"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n"
    "attributes #2 = { nounwind }"
  },
  {
    "target datalayout = \"e-m:e-p:32:32-i1:8:32-i8:8:32-i16:16:32-f64:32:64-v64:32:64-v128:32:128-a:0:32-n32-S32\"\n"
    "target triple = \"thumbv7m--\"\n",
    
    "attributes #0 = { nounwind \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"true\""
    " \"no-frame-pointer-elim-non-leaf\" \"no-infs-fp-math\"=\"false\" \"no-nans-fp-math\"=\"false\" "
    "\"stack-protector-buffer-size\"=\"8\" \"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"true\" }\n"
    "attributes #1 = { \"less-precise-fpmad\"=\"false\" \"no-frame-pointer-elim\"=\"true\" \"no-frame-pointer-elim-non-leaf\" "
    "\"no-infs-fp-math\"=\"false\" \"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" "
    "\"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"true\" }\n"
  },
  {
    "target datalayout = \"e-m:e-p:32:32-i64:64-v128:64:128-a:0:32-n32-S64\"\n"
    "target triple = \"thumbv7em--\"\n",
    
    "attributes #0 = { norecurse nounwind \"disable-tail-calls\"=\"false\" \"less-precise-fpmad\"=\"false\""
    " \"no-frame-pointer-elim\"=\"true\" \"no-frame-pointer-elim-non-leaf\" \"no-infs-fp-math\"=\"false\""
    " \"no-nans-fp-math\"=\"false\" \"stack-protector-buffer-size\"=\"8\" \"target-cpu\"=\"cortex-m4\""
    " \"target-features\"=\"+d16,+dsp,+fp-only-sp,+hwdiv,+strict-align,+vfp4,-crypto,-fp-armv8,-neon\" "
    "\"unsafe-fp-math\"=\"false\" \"use-soft-float\"=\"false\" }\n"
    "attributes #1 = { argmemonly nounwind }\n"
  }
};

LLVMProcessor::LLVMProcessor (LLVMTypeMachine t) : BaseProcessor(),
  m_Type (t), maxstrs(256), texts() {
  iFmt = NULL;
  ptrSize = 4;
  wpSize  = 4;
  switch (m_Type) {
    case MachineTypeLinux64:
      ptrSize = 8;
      wpSize  = 8;
      break;
    case MachineTypeCortexM3:
    case MachineTypeCortexM4F:
      break;
    default:
      break;
  }
  char buf [80];
  snprintf(buf, 80, "i%d", ptrSize * 8);
  iFmt = strdup (buf);
  fromField = false;
  
  mainfunc = new llfnc ("Simple");
  bodyFunctions.clear();
}
LLVMProcessor::~LLVMProcessor() {
  if (iFmt) free (iFmt);
  delete mainfunc;
}
/**
 * Pravidlo:
 * za kazde "  %%%ld = cosi..." se musi zvysit citac promennych %NN o 1.
 * */
void LLVMProcessor::c_PragmaLj() {
  //-fprintf (stdout, "Funkce:%s - unused\n", __FUNCTION__);
}
void LLVMProcessor::c_PragmaSj() {
  //-fprintf (stdout, "Funkce:%s - unused\n", __FUNCTION__);
}
void LLVMProcessor::PopLbl (void) {
  unsigned long tmp = SemPop();
  currentFunction->label (tmp);
}

void LLVMProcessor::c_if() {
  // provede skok na adresu ++LblNum, pokud je carry
  LblNum++;
  //-fprintf (stdout, "Funkce:!%s\t->L%04lX\n", __FUNCTION__, LblNum);
  trace ("\t#IF ->L%04lX\n", LblNum);
  SemPush (LblNum);
  // a pak jeste nejak obnovi carry
  
  lle e1 = currentFunction->carry.load ();
  lle e2 = e1.binaryOp ("icmp eq", 0);
  e2.condBranch (LblNum);
  
}
void LLVMProcessor::c_else() {
  LblNum++;
  //-fprintf (stdout, "Funkce:%s\t->L%04lX\n", __FUNCTION__, LblNum);
  trace ("\t#ELSE ->L%04lX\n", LblNum);
  currentFunction->branch(LblNum);
  PopLbl();
  SemPush (LblNum);
}
void LLVMProcessor::c_endif() {
  // placne tam label, schovanou v c_if
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned long tmp = (unsigned long) SemPop();
  trace ("\t#ENDIF ->L%04lX\n", tmp);
  
  currentFunction->branch(tmp);
  currentFunction->label (tmp);
}
extern const char * pesll;
void LLVMProcessor::c_procinit() {
  //-fprintf (stdout, "\nFunkce:%s\n", __FUNCTION__);

  mf=of;
  LblNum = 0;
  
  ProcNum = 0;
  FuncNum = 0;

  if (!RomBegin) {
    RomBegin = ROMORG;
  }
  if (!RamBegin) {
    RamBegin = RAMORG;
  }
  RamPtr=RamBegin;

  AddInfo ("\nImplementace pro LLVM, Kizarm - testing\n");

  currentFunction = mainfunc;
  const int bufmax = 0x400;
  char buffer [bufmax];
  int count = 0;
  count += snprintf (buffer + count, bufmax - count, "%s\n", pst[m_Type].prefix);
  *currentFunction << buffer;
  *currentFunction << pesll;
  
  currentFunction->declare();
  
}
void LLVMProcessor::c_procend() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  long int a,b;
  
  currentFunction->exit();
  if (!bodyFunctions.empty()) {
    int i, n = bodyFunctions.size();
    for (i=0; i<n; i++) {
      lls str = bodyFunctions.at (i);
      *currentFunction << str;
    }
  }
  texts.close(*currentFunction);
  *currentFunction << pst[m_Type].suffix;
  currentFunction->write (mf);
  fclose (mf);
  
  char * copy = new char [1024];
  strncpy (copy, MacF, 1024);
  strcat (copy, ".ll");
  rename (MacF, copy);
  delete [] copy;

  a=RamPtr>>3;
  b=RamBegin>>3;
  if (RamPtr&7) {
    a++;
  }

  AddInfo ("\n");
  AddInfo ("\nAdresa stanice    ... %d",NetAdr);
  AddInfo ("\nProgram           ... %04LXH",RomBegin);
  AddInfo ("\nPromenne          ... %04LXH",b);
  AddInfo (" - %04LXH\n",a);

}

/** *************************************************************************/

void LLVMProcessor::c_pushl() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace   ("\tPUSH CARRY; TOC++\n");
  lle e0 = currentFunction->bstack.stackptrw(PushWord);
  lle e1 = currentFunction->carry.load();
  e0.store(e1);
}
void LLVMProcessor::c_popl() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace   ("\tPOP CARRY [->last %%var, nemeni %%carry]; --TOC\n");
  lle e0 = currentFunction->bstack.stackptrw(PopWord);
  lastb  = e0.load();
}
/*---------------------------------------------------------------------------*/
/*   Bitove operace                                                          */
/*---------------------------------------------------------------------------*/

void LLVMProcessor::c_setbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  trace  ("\tBIT[%04lX]=%04lX.%lX SET DIRECT\n", n, n>>3, n&7);
  
  currentFunction->callVoid ("WsetBit", n, 1);
}
void LLVMProcessor::c_resbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  trace  ("\tBIT[%04lX]=%04lX.%lX RESET DIRECT\n", n, n>>3, n&7);
  
  currentFunction->callVoid ("WsetBit", n, 0);
}
void LLVMProcessor::c_invbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  trace  ("\tBIT[%04lX]=%04lX.%lX CPL DIRECT\n", n, n>>3, n&7);
  
  currentFunction->callVoid ("WcplBit", n);
}
void LLVMProcessor::c_ldbd (unsigned long int n) {
  // do carry da bit na adrese n
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  trace  ("\tBIT %04lX.%lX -> CARRY\n", n>>3, n&7);
  PredefAdr (n);
  
  lle e1 = currentFunction->callGetBit(n);
  currentFunction->carry.store(e1);
  
}
void LLVMProcessor::c_stbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  trace  ("\tCARRY -> BIT %04lX.%lX\n", n>>3, n&7);
  PredefAdr (n);
  
  lle e0 = currentFunction->carry.load();
  currentFunction->callVoid ("WsetBit", n, e0);
}
void LLVMProcessor::logOpBD (const char * op, bool inv, long unsigned int n) {
  PredefAdr (n);
  if (inv) trace  ("\tBIT %04lX.%lX DIRECT not %s CARRY -> CARRY\n", n>>3, n&7, op);
  else     trace  ("\tBIT %04lX.%lX DIRECT     %s CARRY -> CARRY\n", n>>3, n&7, op);
  PredefAdr (n);
  lle e1 = currentFunction->callGetBit(n);
  lle e2 = currentFunction->carry.load();
  lle e3;
  if (inv) {
    lle e0 = e1.binaryOp ("xor", 1);
        e3 = e0.binaryOp (op, e2);
  } else {
        e3 = e1.binaryOp (op, e2);
  }
  currentFunction->carry.store (e3);
}

void LLVMProcessor::c_orbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("or", false, n);
}
void LLVMProcessor::c_andbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("and", false, n);
}
void LLVMProcessor::c_norbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("or", true, n);
}
void LLVMProcessor::c_nandbd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  logOpBD ("and", true, n);
}
void LLVMProcessor::logOpWpop (const char * op) {
  c_popl();
  trace   ("\tCARRY %s POP[CARRY] -> CARRY\n", op);
  
  lle e0 = currentFunction->carry.load();
  lle e3 = e0.binaryOp (op, lastb);
  currentFunction->carry.store(e3);

}
void LLVMProcessor::c_orb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  logOpWpop ("or");
}
void LLVMProcessor::c_andb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  logOpWpop ("and");
}
void LLVMProcessor::c_xorb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  logOpWpop ("xor");
}
void LLVMProcessor::c_notb() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace  ("\tBIT CARRY NOT\n");
  
  lle e0 = currentFunction->carry.load();
  lle e1 = e0.binaryOp ("xor", 1);
  currentFunction->carry.store(e1);
}

union R2U {
  struct {
    u_int16_t h;
    u_int16_t l;
  };
  u_int32_t hl;
  float x;
};

void LLVMProcessor::c_ldaim (unsigned long int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX ->>>>>>>>>> je v bytech\n", __FUNCTION__, a);
  trace        ("\tNacteni adresy %04lX do TOS; TOS++\n", a >> 4);
  PredefAdr (a);
  
  currentFunction->wstack.pusha(a >> 3);
}
void LLVMProcessor::c_stri() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  if (fromField) {
    trace        ("\tSTORE REAL TO FIELD -> @TOS; --TOS\n");
    lle e0 = currentFunction->wstack.stackptrr(PopReal);
    lle e1 = lastw.bitcast ("float*");
    lle e3 = e0.load();
    e1.store(e3);
  } else {
    trace        ("\tSTORE REAL @TOS-1 -> @TOS; TOS-=2\n");
    lle e0 = currentFunction->wstack.stackptrw(PopWord);
    lle e1 = currentFunction->wstack.stackptrr(PopReal);
    lle e4 = e1.load();			// ulozeny real
    lle e2 = e0.load();
    lle e5 = e2.binaryOp("ashr", 1);	// adresa je v bytech
    lle e3 = currentFunction->Variables.getelementr(e5);
    e3.store(e4);
  }
  currentFunction->wstack.test();
  fromField = false;
}
void LLVMProcessor::c_ldri() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  /*  load indirect operand  */
  if (fromField) {
    trace       ("\tLOAD REAL FROM FIELD -> @TOS++;\n");
    lle e0 = currentFunction->wstack.stackptrr(PushReal);
    lle e1 = lastw.bitcast ("float*");
    lle e3 = e1.load();
    e0.store(e3);
  } else {
    trace       ("\tLOAD REAL @TOS -> @TOS\n");
    lle e0 = currentFunction->wstack.stackptrw(PopWord);
    lle e1 = e0.load();
    lle e2 = e1.binaryOp("ashr", 1);	// adresa je v bytech
    lle e3 = currentFunction->Variables.getelementr(e2);
    lle e4 = e3.load();
    
    lle e5 = currentFunction->wstack.stackptrr(PushReal);
    e5.store(e4);
  }
  fromField = false;
}
void LLVMProcessor::c_ldrim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  R2U t; D2U d;         // Konverze je nepochopitelna, ale funkcni
  t.h = SemPop();       // !!! neprenositelna na be
  t.l = SemPop();
  d.d = t.x;    // musi byt double a z toho teprve hex string
  trace  ("\tLOAD IMM %g, 0x%016lX -> @TOS; TOS++\n", t.x, d.u);
  lle e0 = currentFunction->wstack.stackptrr(PushReal);
  e0.stored (d.d);
  
}
void LLVMProcessor::mathOpRE (const char * op) {
  trace   ("\t! REAL @TOS %s @TOS-1 -> @TOS; --TOS\n", op);
  lle e0 = currentFunction->wstack.stackptrr(PopReal);
  lle e1 = currentFunction->wstack.stackptrr(PopReal);
  lle e2 = e0.load();
  lle e3 = e1.load();
  lle e4 = e3.binaryOp (op, e2);
  
  lle e5 = currentFunction->wstack.stackptrr(PushReal);
  e5.store(e4);
}
void LLVMProcessor::c_addr() {
  //-fprintf  (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fadd");
}
void LLVMProcessor::c_subr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fsub");
}
void LLVMProcessor::c_mulr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fmul");
}
void LLVMProcessor::c_divr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpRE ("fdiv");
}

/** *********************************************************************************************************/
void LLVMProcessor::c_chsr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tREAL @TOS -> -@TOS\n");
  lle e0 = currentFunction->wstack.stackptrr(CurrentReal);
  lle e1 = e0.load();
  lle e2 (e1);
  e2.body << "  " << e2.name << " = fsub float 0.0, " << e1.name << "\n";
  * currentFunction << e2.body;
  e0.store(e2);
}
void LLVMProcessor::c_absr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tREAL @TOS -> abs @TOS\n");
  lle e0 = currentFunction->wstack.stackptrr(CurrentReal);
  lle e1 = e0.load();
  lle e2 (e1);
  e2.body << "  " << e2.name << " = tail call float @fabsf(float " << e1.name << ")\n";
  * currentFunction << e2.body;
  e0.store(e2);
}
void LLVMProcessor::c_truncr() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tREAL @TOS -> abs @TOS\n");
  lle e0 = currentFunction->wstack.stackptrr(CurrentReal);
  lle e1 = e0.load();
  lle e2 (e1);
  e2.body << "  " << e2.name << " = tail call float @truncf(float " << e1.name << ")\n";
  * currentFunction << e2.body;
  e0.store(e2);
}

/** *********************************************************************************************************/
void LLVMProcessor::c_testpole() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned long a = SemPop();  // adresa konce pole v bytech
  trace       ("\tTEST POLE %04lX; --TOS\n", a);
  fromField = true;
  
  lle e0 = currentFunction->wstack.stackptrw(PopWord);
  lle e1 = e0.load();
  lindex = e1.callTest((short) a);	// v bytech
  /// zde se vraci ptr na prvek pole, se kterym se dal bude pracovat
  lle e2  = lindex.binaryOp("ashr", 1);
  lastw   = currentFunction->Variables.getelement(e2);
}
/** *********************************************************************************************************/
void LLVMProcessor::c_ldwi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace        ("\tLOAD  INDIRECT WORD; ++TOS\n");
  fromField = false;
  
  lle e0 = lastw.load();
  lle e1 = currentFunction->wstack.stackptrw(PushWord);
  e1.store(e0);
}
void LLVMProcessor::c_stwi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tSTORE INDIRECT WORD; --TOS ;\n");
  fromField = false;
  
  lle e0 = currentFunction->wstack.stackptrw(PopWord);
  lle e1 = e0.load();
  // vymena na pomocnem stacku zpusobuje blbosti
//if (!currentFunction->intfunc) lastw.wordChange(e1);
  lastw.store     (e1);

  currentFunction->wstack.test();
}
void LLVMProcessor::c_ldwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned long a = SemPop();
  trace       ("\tLOAD IMM WORD 0x%04lX -> @TOS; TOS++\n", a);
  lle e0 = currentFunction->wstack.stackptrw(PushWord);
  e0.store (a);
}

void LLVMProcessor::c_ldwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  trace       ("\tLOAD WORD @W%04lX -> @TOS; TOS++\n", n >> 4);
  lle e1 = currentFunction->Variables.getelement (n >> 4);
  lle e2 = e1.load();
  lastw  = currentFunction->wstack.stackptrw (PushWord);
  lastw.store(e2);
}
void LLVMProcessor::c_stwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  trace       ("\tSTORE @TOS -> WORD @W%04lX; --TOS\n", n >> 4);
  PredefAdr (n);
  lle e1 = currentFunction->Variables.getelement (n >> 4);
  lle e2 = currentFunction->wstack.stackptrw(PopWord);
  lle e3 = e2.load();
  e1.wordChange(e3);
  e1.store     (e3);
  
  currentFunction->wstack.test();
}
void LLVMProcessor::mathOpWDS (const char * op) {
  trace         ("\t! WORD @TOS-1 %s @TOS -> @TOS; --TOS;\n", op);
  lle e0 = currentFunction->wstack.stackptrw(PopWord);
  lle e1 = currentFunction->wstack.stackptrw(PopWord);
  lle e2 = e0.load();
  lle e3 = e1.load();
  lle e4 = e2.binaryOp (op, e3);
  lle e5 = currentFunction->wstack.stackptrw(PushWord);
  e5.store(e4);
}

void LLVMProcessor::mathOpIM (const char * op) {
  unsigned a = SemPop();
  trace         ("\t! @TOS %s IMM WORD %04X -> @TOS\n", op, a);
  currentFunction->RegHl.store(a);
  lle e0 = currentFunction->wstack.stackptrw(CurrentWord);
  lle e1 = e0.load();
  lle e2 = currentFunction->RegHl.load();
  lle e3 = e1.binaryOp (op, e2);
  e0.store(e3);
}

void LLVMProcessor::mathOpWD (const char * op, long unsigned int n) {
  PredefAdr (n);
  trace         ("\t! WORD @W%04lX %s @TOS -> @TOS\n", n >> 4, op);
  lle e0 = currentFunction->wstack.stackptrw(CurrentWord);
  lle e1 = currentFunction->Variables.getelement(n >> 4);
  lle e2 = e0.load();
  lle e3 = e1.load();
  lle e4 = e2.binaryOp (op, e3);
  e0.store(e4);
}
void LLVMProcessor::c_addwp() {
  //-fprintf (stdout, "Funkce:%s - unused\n", __FUNCTION__);
  c_addw();     // to je blbost, lze vratit
}

void LLVMProcessor::c_addwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("add", n);
}
void LLVMProcessor::c_subwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("sub", n);
}
void LLVMProcessor::c_mulwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("mul", n);
}
void LLVMProcessor::c_divwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("udiv", n);
}

void LLVMProcessor::c_addwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("add");
}
void LLVMProcessor::c_subwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("sub");
}
void LLVMProcessor::c_mulwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("mul");
}
void LLVMProcessor::c_divwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("udiv");
}

void LLVMProcessor::c_addw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("add");
}

void LLVMProcessor::c_subw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("sub");
}

void LLVMProcessor::c_mulw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("mul");
}

void LLVMProcessor::c_divw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("udiv");
}

void LLVMProcessor::c_stimmd (unsigned long n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  PredefAdr (n);
  unsigned a = SemPop();        // immediate op
  trace  ("\tWORD %04X -> @W%04lX\n", a, n >> 4);
  // + sitovy WORD vola jeste sluzbu site
  lle e1 = currentFunction->Variables.getelement(n>>4);
  e1.wordChange ((short) a);
  e1.store ((short) a);
}
/** *********************************************************************************************************/
void LLVMProcessor::relOpWD (const char * op) {
  trace   ("\t@TOS %s @TOS-1 -> CARRY; TOS-=2\n", op);
  lle e0 = currentFunction->wstack.stackptrw(PopWord);
  lle e1 = currentFunction->wstack.stackptrw(PopWord);
  lle e2 = e0.load();
  lle e3 = e1.load();
  lle e4 = e3.binaryRel (op, e2);
  currentFunction->carry.store(e4);
}
void LLVMProcessor::relOpWDIM (const char * op) {
  unsigned long a = SemPop();
  trace       ("\t@TOS %s IMM %04lX -> CARRY; --TOS\n", op, a);
  lle e0 = currentFunction->wstack.stackptrw(PopWord);
  lle e2 = e0.load();
  lle e3 = e2.binaryRel (op, a);
  currentFunction->carry.store(e3);
}
void LLVMProcessor::relOpRE(const char* op) {
  trace   ("\t@TOS %s @TOS-1 -> CARRY; TOS-=2\n", op);
  lle e0 = currentFunction->wstack.stackptrr(PopReal);
  lle e1 = currentFunction->wstack.stackptrr(PopReal);
  lle e2 = e0.load();
  lle e3 = e1.load();
  lle e4 = e3.binaryRelRe (op, e2);
  currentFunction->carry.store(e4);
}

void LLVMProcessor::c_ltw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ult");
}
void LLVMProcessor::c_lew() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ule");
}
void LLVMProcessor::c_gtw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ugt");
}
void LLVMProcessor::c_gew() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("uge");
}
void LLVMProcessor::c_eqw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("eq");
}
void LLVMProcessor::c_new() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("ne");
}
void LLVMProcessor::c_lti() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("slt");
}
void LLVMProcessor::c_lei() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("sle");
}
void LLVMProcessor::c_gti() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("sgt");
}
void LLVMProcessor::c_gei() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWD ("sge");
}

void LLVMProcessor::c_ltr() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpRE("ult");
}
void LLVMProcessor::c_ler() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpRE("ule");
}
void LLVMProcessor::c_gtr() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpRE("ugt");
}
void LLVMProcessor::c_ger() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpRE("uge");
}
void LLVMProcessor::c_eqr() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpRE("eq");
}
void LLVMProcessor::c_ner() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpRE("ne");
}

void LLVMProcessor::c_muliim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("mul");
}
void LLVMProcessor::c_mulid (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD ("mul", n);
}
void LLVMProcessor::c_muli() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("mul");
}

void LLVMProcessor::c_diviim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM ("sdiv");
}
void LLVMProcessor::c_divid (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, n);
  mathOpWD("sdiv", n);
}
void LLVMProcessor::c_divi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS ("sdiv");
}
/** *********************************************************************************************************/
void LLVMProcessor::c_newim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWDIM ("ne");
}
void LLVMProcessor::c_eqwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  relOpWDIM ("eq");
}

/** *************************************************************************/
void LLVMProcessor::c_andwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM("and");
}
void LLVMProcessor::c_xorwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM("xor");
}
void LLVMProcessor::c_orwim() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpIM("or");
}
void LLVMProcessor::c_andwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWD ("and", n);
}
void LLVMProcessor::c_orwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWD ("or", n);
}
void LLVMProcessor::c_xorwd (unsigned long int n) {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWD ("xor", n);
}
void LLVMProcessor::c_andw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS("and");
}
void LLVMProcessor::c_orw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS("or");
}
void LLVMProcessor::c_xorw() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  mathOpWDS("xor");
}
/** *******************************************************************************/
const LLVMProcessor::conv_s LLVMProcessor::RetypeConversions[] = {
  {"float","i16",4,2},
  {"i16","float",2,4}
};

void LLVMProcessor::Convert (const LLVMProcessor::conv_s * ct, const char * util) {
  lle efp, etp;
  if (ct->align_from == 4) {
    efp = currentFunction->wstack.stackptrr(PopReal);
    etp = currentFunction->wstack.stackptrw(PushWord);
  }
  else{
    efp = currentFunction->wstack.stackptrw(PopWord);
    etp = currentFunction->wstack.stackptrr(PushReal);
  }
  lle ef = efp.load();
  lle ed (ef);
  ef.body << "  " << ed.name << " = " << util << " " << ct->from << " " << ef.name << " to " << ct->to << "\n";
  *currentFunction << ef.body;
  ed.type = ct->to;
  etp.store (ed);  
}

void LLVMProcessor::c_rtoi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace        ("\tREAL -> INT  @TOS\n");
  Convert (RetypeConversions + 0,"fptosi");
}
void LLVMProcessor::c_rtow() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace        ("\tREAL -> WORD @TOS\n");
  Convert (RetypeConversions + 0,"fptoui");
}
void LLVMProcessor::c_itor() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tINT  -> REAL @TOS\n");
  Convert (RetypeConversions + 1,"sitofp");
}
void LLVMProcessor::c_wtor() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tWORD -> REAL @TOS\n");
  Convert (RetypeConversions + 1,"uitofp");
}

/** *************************************************************************/

void LLVMProcessor::c_caseof() {
  unsigned n = SemPop();
  //-fprintf (stdout, "Funkce:%s, %d\n", __FUNCTION__, n);
  trace       ("\t#CASEOF (%d) -> L%04lX\n", n, LblNum+1);
  
  lle e1 = currentFunction->wstack.stackptrw(CurrentWord);
  lle e2 = e1.load();
  lle e3 = e2.binaryOp ("icmp ne", n);
  LblNum += 1;
  e3.condBranch (LblNum);
  SemPush (LblNum);
}
void LLVMProcessor::c_endcase() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\t#ENDCASE; TOS--\n");
  lle e1 = currentFunction->wstack.stackptrw(PopWord);  
}

/** *************************************************************************/
void LLVMProcessor::c_proced (LPOLOZKA * p, int pos) {
  //-fprintf (stdout, "Funkce:%s -> \"%s\" fpl=%d\n", __FUNCTION__, p->iden, p->fpl);
  trace      ("\tPROCEDURE %s\n", p->iden);
  ProcNum += 1;
  p->atr   = ProcNum;
  funciner = new llfnci (p->iden);
  funciner->declare();
  currentFunction = funciner;
}
void LLVMProcessor::c_preturn (LPOLOZKA * p) {
  //-fprintf (stdout, "Funkce:%s -> \"%s\"\n", __FUNCTION__, p->iden);
  trace       ("\tRETURN PROCEDURE %s\n", p->iden);
  LblNum = 0;
  funciner->close(bodyFunctions);
  currentFunction = mainfunc;
  delete funciner;
}
void LLVMProcessor::c_pcall (char * u) {
  //-fprintf (stdout, "Funkce:%s -> [%s]\n", __FUNCTION__, u);
  trace       ("\tCALL PROCEDURE %s; TOS:=0\n", u);
  currentFunction->calli(u);
  currentFunction->wstack.clear();
}
void LLVMProcessor::c_spar (long unsigned int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
// ono to a ukazuje vlastne za parametr - musime zde tedy znat jeho velikost
  unsigned long parno = (a>>4)-2;
  fromField = true;
  trace       ("\tOPERAND REAL %ld z PARAM -> PTR\n", parno);
  lastw = currentFunction->param.getelement(parno);  
}

void LLVMProcessor::c_spa (unsigned long int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
  unsigned long parno = (a>>4)-1;
  //fromField = true;
  trace       ("\tOPERAND WORD %ld z PARAM -> PTR\n", parno);
  lastw = currentFunction->param.getelement(parno);  
}
void LLVMProcessor::c_ldaa (unsigned long int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
  unsigned long parno = (a>>4)-1;
  fromField = true;
  trace       ("\tOPERAND @WORD %ld z PARAM -> PTR\n", parno);
  lle e0 = currentFunction->param.getelement(parno);
  lle e1 = e0.load();
  lle e2 = e1.binaryOp("ashr", 1);
  lastw  = currentFunction->Variables.getelement(e2);
}

void LLVMProcessor::c_func (LPOLOZKA * p, int pos) {
  //-fprintf (stdout, "Funkce:%s, \"%s\"\n", __FUNCTION__, p->iden);
  trace      ("\tFUNCTION \"%s\"\n", p->iden);
  FuncNum += 1;
  p->atr   = FuncNum;
  funciner = new llfnci (p->iden);
  funciner->intfunc = true;
  funciner->declare();
  currentFunction = funciner;
}
void LLVMProcessor::c_freturn (LPOLOZKA * p) {
  //-fprintf (stdout, "Funkce:%s - \"%s\":(%d)\n", __FUNCTION__, p->iden, p->rvs);
  trace       ("\tRETURN FUNCTION \"%s\"\n", p->iden);
  LblNum = 0;
  funciner->close(bodyFunctions);
  currentFunction = mainfunc;
  delete funciner;
}
static unsigned allocatedWords;
static lle wshift;

void LLVMProcessor::c_allot() {
  int a = SemPop();
  //-fprintf (stdout, "Funkce:%s (%d)\n", __FUNCTION__, a);
  trace       ("\tALLOC ON TOP %d WORDS\n", a>>4);
  allocatedWords = a >> 4;
  wshift = currentFunction->wstore.getelement(allocatedWords);
  for (unsigned i=0; i<allocatedWords; i+=1)
    currentFunction->wstack.pusha(0);		// volne misto na stacku
}
void LLVMProcessor::c_fcall (char * u) {
  //-fprintf (stdout, "Funkce:%s (%d) \"%s\"\n", __FUNCTION__, (int) sez_uk->atr, u);
  trace       ("\tCALL FUNCTION %s; TOS:=1\n", u);
  currentFunction->callf (u, wshift);
  currentFunction->wstack.clear(allocatedWords);
  //fromField = true;
}

int  LLVMProcessor::c_retval() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  // do nothing
  return 0;
}
/** *************************************************************************/
static unsigned beginBadr;

void LLVMProcessor::c_ldbaim (unsigned long int a) {
  //-fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
  trace        ("\tNacteni adresy bitu %04lX [%03X.%X]; 0 do TOS; TOS++\n", a, a>>4, a&0xF);
  PredefAdr (a);
  beginBadr = a;
  
  currentFunction->wstack.pusha(0);
}

void LLVMProcessor::c_setbi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned n = beginBadr;
  trace  ("\tBIT [%04lX]+ofset SET INDIRECT\n", n);
  lle e0 = lindex.binaryOp("add", n);
  
  currentFunction->callVoid ("WsetBit", e0, 1);
}
void LLVMProcessor::c_resbi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned n = beginBadr;
  trace  ("\tBIT [%04lX]+ofset RESET INDIRECT\n", n);
  lle e0 = lindex.binaryOp("add", n);
  
  currentFunction->callVoid ("WsetBit", e0, 0);
}

void LLVMProcessor::c_invbi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned n = beginBadr;
  trace  ("\tBIT [%04lX]+ofset INV INDIRECT\n", n);
  lle e0 = lindex.binaryOp("add", n);
  
  currentFunction->callVoid ("WcplBit", e0);
}
void LLVMProcessor::c_ldbi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  unsigned n = beginBadr;
  trace  ("\tBIT [%04lX]+ofset -> CARRY\n", n);
  lle e0 = lindex.binaryOp("add", n);
  
  lle e1 = currentFunction->callGetBit (e0);
  currentFunction->carry.store(e1);
}
void LLVMProcessor::c_chsi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tWORD @TOS -> -@TOS\n");
  lle e0 = currentFunction->wstack.stackptrw(CurrentWord);
  lle e1 = e0.load();
  lle e2 (e1);
  e2.body << "  " << e2.name << " = sub i16 0, " << e1.name << "\n";
  * currentFunction << e2.body;
  e0.store(e2);
}
void LLVMProcessor::c_absi() {
  //-fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
  trace       ("\tWORD @TOS -> abs @TOS\n");
  lle e0 = currentFunction->wstack.stackptrw(CurrentWord);
  lle e1 = e0.load();
  lle e2 (e1);
  e2.body << "  " << e2.name << " = tail call i16 @absi(i16 " << e1.name << ")\n";
  * currentFunction << e2.body;
  e0.store(e2);
}

void LLVMProcessor::c_disps (char * u) {
  //-fprintf (stdout, "Funkce:%s : \"%s\"\n", __FUNCTION__, u);
  texts.display(u);
}
/** *********************************************************************************************************/
void LLVMProcessor::c_skpb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
/** *********************************************************************************************************/
void LLVMProcessor::c_retb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_stbi() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_ldbaa (unsigned long int a) {
  fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
}
void LLVMProcessor::c_spba (unsigned long int a) {
  fprintf (stdout, "Funkce:%s\t%08lX\n", __FUNCTION__, a);
}

void LLVMProcessor::c_swap() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_disp (int) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_dispb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_prnt (int) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_prntb() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

/** *******************************************************************************/
void LLVMProcessor::c_prnts (char * u) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

void LLVMProcessor::c_memwrite() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_memread() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_unknown (char * u) {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}
void LLVMProcessor::c_procerror() {
  fprintf (stdout, "Funkce:%s\n", __FUNCTION__);
}

