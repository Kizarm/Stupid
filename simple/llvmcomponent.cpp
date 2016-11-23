/*
 * Copyright 2016 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#include "llvmcomponent.h"

using namespace std;

llfnc  * currentFunction;
unsigned wpSize;

struct ElementDesc {
  const int    align;
  const char * name;
};

static const ElementDesc Desc [] = {
  {2,"i1"}, {2,"i16"}, {4,"i32"}, {4,"float"}, {8,"union.RamDef_u"}
};

lle::lle(ElementType t, const char* n, int p, int number) :
 nume (number), ptrn (p), type (t), name (n) {
  ordr = 0;
  ainc = false;
  setFullName();
  if (name.empty()) {
    ainc = true;
  } else {
    const int maxbuf = 0x400;
    char buf [maxbuf];
    if (nume) snprintf (buf, maxbuf, "[%d x %s]", nume, Desc[type].name);
    else      snprintf (buf, maxbuf, "%s"             , Desc[type].name);
    styp = buf;
    snprintf    (buf, maxbuf, "  %%%s = alloca %s, align %d\n",
		  full.c_str(), styp.c_str(), Desc[type].align);
    body.assign (buf);
  }
}
lle::lle(ElementType t, const int order) :
 nume (0), ptrn (0), type (t), name ("") {
  ordr = order;
  ainc = false;
  const int maxbuf = 0x40;
  char buf [maxbuf];
  snprintf (buf, maxbuf, "%d", ordr);
  name = buf;
  setFullName();
}

lle::lle(const lle& from) {

}
lle::lle(const lle& from, const char* n) {
  name = n;
  if (from.ptrn > 0) ptrn = from.ptrn - 1;
  setFullName();
  ordr = from.ordr;
  type = from.type;
  ainc = from.ainc;
  styp = from.styp;
  const int maxbuf = 0x400;
  char buf [maxbuf];
  snprintf (buf, maxbuf,"  %%%s = getelementptr inbounds %s, %s* %%%s, i%d 0, i%d %d\n",
	    full.c_str(), styp.c_str(), styp.c_str(), from.full.c_str(), wpSize << 3, wpSize << 3, 0);
  body.assign (buf);
}

lle::~lle() {

}
string & lle::getBody(void) {
  return body;
}
lle& lle::getelement(const lle& from) {
  return * this;
}
lle& lle::getelement(int no) {
  return * this;
}
lle& lle::getglobal(int no) {
  body.clear();
  const int maxbuf = 0x400;
  char buf [maxbuf];
  snprintf (buf, maxbuf,"  %%%s = load %%%s*, %%%s** @RamBasePtr, align %d\n",
	    name.c_str(), Desc[type].name, Desc[type].name, Desc[type].align);
  body.assign (buf);
  return * this;
}
void lle::setFullName(void) {
  if (ptrn > 0) {
    full = name;
    for (int i=0; i<ptrn; i++) full += '*';
  } else full = name;
}
void lle::store(lle& e) {
  const int maxbuf = 0x400;
  char buf [maxbuf];
  snprintf (buf, maxbuf, "  store %s %%%s, %s* %%%s, align %d\n",
	    Desc[type].name, e.full.c_str(), Desc[type].name, full.c_str(), Desc[type].align);
  body.assign(buf);
  * currentFunction << *this;
}
lle lle::load(void) {
  int num = currentFunction->newnumber ();
  lle tmp  (llvm_i16, num);
  const int maxbuf = 0x400;
  char buf [maxbuf];
  snprintf (buf, maxbuf, "  %%%s = load %s, %s* %%%s, align %d\n",
            tmp.full.c_str(), Desc[type].name, Desc[type].name, full.c_str(), Desc[type].align);
  body.assign(buf);
  * currentFunction << *this;
  return tmp;
}
lle lle::binaryOp(const char* op, unsigned int r) {
  int num = currentFunction->newnumber ();
  lle tmp  (llvm_i16, num);
  const int maxbuf = 0x400;
  char buf [maxbuf];
  snprintf (buf, maxbuf, "  %%%s = %s %s %%%s, %d\n", tmp.full.c_str(), op, Desc[type].name, full.c_str(), r);
  body.assign(buf);
  * currentFunction << *this;
  return tmp;
}

/** *************************************************************************************/
llfnc::llfnc(const char* n) :
  carry     (llvm_i16,   "carry"),
  RegHl     (llvm_i16,   "RegHl"),
  RegRe     (llvm_float, "RegRe"),
  stock     (llvm_i16,   "stock", 0, 16),
  stack     (stock,      "stack"),  
  Variables (llvm_user,  "Variables", 1),
  number(1), name (n), maxbuf(0x400)  {

  labnum = 0;
  buf = new char [maxbuf];
  // currentFunction = this;
  snprintf (buf, maxbuf,   "@RamBasePtr = external global %%union.RamDef_u*\n"
                           "\n; Function Attrs: nounwind\ndefine void @%s() #0 {\n", name.c_str());
  string s (buf);
  body.clear();
  body.push_back(s);
  Variables.getglobal(0);
  * this << carry << RegHl << RegRe << stock << stack << Variables;
}
llfnc::~llfnc() {
  delete [] buf;
}
llfnc& llfnc::operator<<(lle & add) {
  string & s = add.getBody();
  body.push_back (s);
  s.clear();
  return * this;
}
llfnc& llfnc::operator<<(const char* add) {
  string s (add);
  body.push_back (s);
  return * this;
}
void llfnc::prefix(const char* str) {
  string s (str);
  vector<string>::iterator it = body.begin();
  body.insert (it,1,s);
}

void llfnc::write(FILE* to) {
  int i, n = body.size();
  for (i=0; i<n; i++) {
    fprintf (to, "%s", body.at(i).c_str());
  }
  body.clear();
}
void llfnc::exit(void) {
  string s ("\n  ret void\n}\n");
  body.push_back(s);
}

lle llfnc::callGetBit(unsigned int adr) {
  lle tmp  (llvm_i16, number);
  snprintf (buf, maxbuf, "  %%%d = tail call zeroext i16 @WgetBit(i32 %d) #2\n", number, adr);
  string  s(buf);
  body.push_back(s);
  increment();
  return tmp;
}
