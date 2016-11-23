#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inc/Znak.h"
//#include "inc/list.h"
#include "inc/zasob.h"
#include "inc/lexem.h"
#include "inc/Var.h"
#include "inc/firmware.h"
#include "inc/fce.h"
#include "inc/chyby.h"
#include "inc/procesor.h"
#include "LLVMStack.h"

LLVMStack::LLVMStack() {
  sp = 0;
}

LLVMStack::~LLVMStack() {
}
void LLVMStack::ZeroTest (void) {
  if (sp) fprintf (stderr, "!!! Interni chyba LLVM stack=%d (ma byt 0)\n", sp);
}

void LLVMStack::Declare (void) {
  //fprintf (mf, "%%union.Frame = type { float }\n");
}
void LLVMStack::Define (const int depth, const char * wide) {
  fprintf (mf, "  %%stock = alloca [%d x i16], align 16\n", depth);
  fprintf (mf, "  %%stack = getelementptr inbounds [%d x i16], [%d x i16]* %%stock, %s 0, %s 0\n",
    depth, depth, wide, wide );
}
void LLVMStack::PushAddrInBytes (unsigned int addr, long unsigned int & per) {
  GetElementPtr (per);
  BitCastTo (per, "i16");
  fprintf (mf, "  store i16 %d, i16* %%%ld, align 2\n\n", addr, per-1);
  sp += 1;
}
void LLVMStack::PushWordAtCurrent (long unsigned int & per) {
  GetElementPtr (per);
  BitCastTo (per, "i16");
  fprintf (mf, "  store i16 %%%ld, i16* %%%ld, align 2\n\n", per-3, per-1);
  sp += 1;
}
void LLVMStack::PopWord (long unsigned int & per) {
  sp -= 1;
  GetElementPtr (per);
  BitCastTo (per, "i16");
  fprintf (mf, "  %%%ld = load i16, i16* %%%ld, align 2\n", per, per-1);
  per += 1;
}
void LLVMStack::PopReal (long unsigned int & per) {
  sp -= 2;
  GetElementPtr (per);
  BitCastTo(per, "float");
  fprintf (mf, "  %%%ld = load float, float* %%%ld, align 2\n", per, per-1);
  per += 1;
}

void LLVMStack::PopWordAtCurrent (long unsigned int & per) {
  sp -= 1;
  GetElementPtr (per);
  BitCastTo (per, "i16");
  fprintf (mf, "  %%%ld = load i16, i16* %%%ld, align 2\n", per, per-1);  
  per += 1;
  fprintf (mf, "  tail call void @WordChangeWrap(i16* %%%ld, i16 %%%ld)\n", per-4, per-1);
  fprintf (mf, "  store i16 %%%ld, i16* %%%ld, align 2\n\n", per-1, per-4);
}
void LLVMStack::CurrentWordPtr (long unsigned int & per) {
  GetElementPtr (per, 1);
  BitCastTo (per, "i16");
}
void LLVMStack::CurrentRealPtr (long unsigned int & per) {
  GetElementPtr (per, 2);
  BitCastTo(per, "float");
}

void LLVMStack::PopWordPtr (long unsigned int & per) {
  CurrentWordPtr (per);
  sp -= 1;
  CurrentWordPtr (per);
}
void LLVMStack::PopRealPtr (long unsigned int & per) {
  CurrentRealPtr (per);
  sp -= 2;
  CurrentRealPtr (per);
}

void LLVMStack::DecrementStack (int minus) {
  sp -= minus;
}

void LLVMStack::PushImmWord (short unsigned int imm, long unsigned int & per) {
  GetElementPtr (per);
  BitCastTo (per, "i16");
  fprintf (mf, "  store i16 %d, i16* %%%ld, align 2\n\n", imm, per-1);
  sp += 1;
}
void LLVMStack::PushImmReal (uint64_t realImage, long unsigned int & per) {
  GetElementPtr (per);
  BitCastTo(per, "float");
  fprintf (mf, "  store float 0x%016lX, float* %%%ld, align 2\n\n", realImage, per-1);
  sp += 2;
}
void LLVMStack::PushReal (long unsigned int & per) {
  GetElementPtr (per);
  BitCastTo(per, "float");
  fprintf (mf, "  store float %%%ld, float* %%%ld, align 2\n\n", per-3, per-1);
  sp += 2;
}
/** *******************************************************************************************/
void LLVMStack::BitCastTo (long unsigned int & per, const char * to) {
  fprintf (mf, "  %%%ld = bitcast i16* %%%ld to %s*\n", per, per-1, to);
  per += 1;
}
void LLVMStack::GetElementPtr (long unsigned int & per, int ofs) {
  fprintf (mf, "  %%%ld = getelementptr inbounds i16, i16* %%stack, i32 %d\n", per, sp-ofs);
  per += 1;
}
