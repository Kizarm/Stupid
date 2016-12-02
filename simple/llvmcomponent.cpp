#include <string.h>
#include "llvmcomponent.h"

using namespace std;

llfnc  * currentFunction;
unsigned wpSize;

struct ElementDesc {
  const char * align;
  const char * name;
};

static const ElementDesc Desc [] = {
  {" ","i1"}, {", align 2","i16"}, {", align 4","i32"}, {", align 4","float"},
  {", align 2","%union.RamDef_u"}
};
/** *************************************************************************************/
lls::lls(const int n) {
  const int max = 64;
  char  buf [max];
  snprintf (buf, max, "%%%d", n);
  data.assign(buf);
}
lls::lls(const ElementType t, const int ptrn) {
  data = Desc[t].name;
  if (ptrn) {
    for (int i=0; i<ptrn; i++) data += '*';
  }
}
/** *************************************************************************************/
void llt::display(const char * str) {
  lls decl;
  int l = strlen(str)+1;
  decl << "@T" << no <<".str = private unnamed_addr constant [" << l << " x i8] c\"" << str << "\\00\", align 1\n";
  def.push_back(decl);
  
  lls out;
  out << "  tail call void @Disps(i8* getelementptr inbounds ([" << l << " x i8], [";
  out << l << " x i8]* @T" << no <<".str, i32 0, i32 0)) #1\n";
  no += 1;
  *currentFunction << out;
}
void llt::close(llfnc & to){
  if (def.empty()) return;
  int i, n = def.size();
  for (i=0; i<n; i++) {
    to << def.at(i);
  }
}

/** *************************************************************************************/
lle::lle (ElementType t, const int o, const int n) :
  align(Desc[t].align), name (o), type (t,n) {
    width = 2;
}
// kopirovaci konstruktor pouzivany pro docasne objekty pro funkce typu "load"
lle::lle (const lle& from) {
  int num = currentFunction->newnumber ();
  width = from.width;
  align = from.align;
  lls s (num);
  name  = s;
  type  = from.type;
  addStar(false);
  //printf("Kopirovaci pro %s, type %s\n", (const char*)name, (const char*)type);
}
lle::~lle() {
}
lle lle::getelement(const lle& from) {
  printf ("TODO!!!\n");
  return * this;
}
lle  lle::getelement(int no) {
  lle tmp  (* this);
  //unsigned w = wpSize << 3;
  body << "  " << tmp.name << " = getelementptr inbounds " << tmp.type << ", ";
  body << type << " " << name << ", i32 " <<  no << "\n";
  //tmp.type = "i16*";  // trochu nelogicke, ale pouziva se jen pro stack
  tmp.addStar (true);
  * currentFunction << * this;
  return tmp;
}
void lle::store(lle& e) {
  body << "  store " << e.type << " " << e.name << ", " << type << " " << name <<  align << "\n";
  * currentFunction << * this;
}
void lle::store(unsigned int n) {
  body.clear();
  body << "  store i16 " << n <<                   ", " << type << " " << name <<  align << "\n";
  * currentFunction << * this;
}
void lle::stored(double n) {
  D2U dd;
  dd.d = n;
  char buf [64];
  snprintf (buf, 64, "0x%016lX", dd.u);
  body << "  store float " << buf <<                 ", " << type << " " << name <<  align << "\n";
  * currentFunction << * this;
}

lle lle::load(void) {
  lle tmp  (* this);
  body << "  " << tmp.name << " = load " << tmp.type << ", " << type << " " << name <<  align << "\n";
  * currentFunction << * this;
  return tmp;
}
lle lle::binaryOp(const char* op, unsigned int r) {
  lle tmp  (* this);
  body << "  " << tmp.name << " = " << op << " " << type << " " << name << ", " << r << "\n";
  * currentFunction << * this;
  return tmp;
}
lle lle::binaryOp(const char* op, lle& r) {
  lle tmp  (* this);
  body << "  " << tmp.name << " = " << op << " " << type << " " << name << ", " << r.name << "\n";
  * currentFunction << * this;
  return tmp;
}
lle lle::binaryRel(const char* op, unsigned int r) {
  lle tmp  (* this);
  body << "  " << tmp.name << " = icmp " << op << " " << type << " " << name << ", " << r << "\n";
  * currentFunction << * this;
  tmp.type = "i1";
  return tmp;
}
lle lle::binaryRel(const char* op, lle& r) {
  lle tmp  (* this);
  body << "  " << tmp.name << " = icmp " << op << " " << type << " " << name << ", " << r.name << "\n";
  * currentFunction << * this;
  tmp.type = "i1";
  return tmp;
}
lle lle::binaryRelRe(const char* op, lle& r){
  lle tmp  (* this);
  body << "  " << tmp.name << " = fcmp " << op << " " << type << " " << name << ", " << r.name << "\n";
  * currentFunction << * this;
  tmp.type = "i1";
  return tmp;
}

void lle::condBranch(unsigned int lab) {
  llabel l(lab);
  body << "  br i1 " << name << ", label %" << l.L() << ", label %" << l.D();
  body << "\n" << l.D() << ":\n";
  * currentFunction << * this;
}
void lle::wordChange(unsigned int adr) {
  body << "  tail call void @WordChangeWrap(i16* " << name << ", i16 " << adr    << ")\n";
  * currentFunction << * this;
}
void lle::wordChange(const lle& w) {
  body << "  tail call void @WordChangeWrap(i16* " << name << ", i16 " << w.name << ")\n";
  * currentFunction << * this;
}

lle lle::bitcast(const char * to) {
  lle tmp (*this);
  tmp.type = to;
  body << "  " << tmp.name << " = bitcast " << type << " " << name <<" to " << tmp.type << "\n";
  * currentFunction << * this;
  return tmp;
}
void lle::callw(const char* op, lle& r) {
  body << "  tail call void @" << op << "Word(i16* " << name << ", i16* " << r.name << ")\n";
  * currentFunction << * this;
}
lle lle::callTest(unsigned int p) {
  lle tmp (*this);
  body << "  " << tmp.name  << " = tail call i16 @TestPole (i16 " << name << ", i16 " << p << ")\n";
  * currentFunction << * this;
  return tmp;
}

/** *************************************************************************************/
void lle::setName(lls & newname) {
  name = newname;
}
void lle::setFtype(const int l) {
  const int max = 64;
  char buf [max];
  const char * old = type;
  snprintf (buf, max, "[%d x %s]", l, old);
  lls s (buf);
  type = s;
}
static inline int contains (const char * str, const char c) {
  int l = strlen (str) - 1;
  for (;;) {
    if (str[l] == c) break;
    l -= 1;
    if (!l) break;
  }
  return l;
}
void lle::addStar(bool a){
  const int max = 64;
  char buf [max];
  const char * old = type;
  if (a) snprintf (buf, max, "%s*", old);
  else {
    snprintf (buf, max, "%s", old);
    int n = contains(buf, '*');
    if (n) buf [n] = '\0';
  }
  lls s (buf);
  type = s;
}


/** *************************************************************************************/
lli::lli(ElementType t, const char* n, const int p): lle(t, 0, p) {
  lls s("%");
  s << n;
  setName(s);
  body << "  " << name << " = alloca " << type << align << "\n";
  addStar();
  sp = 0;
}
lli::lli(const lle& old, const char* n) {
  lls s("%");
  s << n;
  setName(s);
  type  = old.type;
  align = old.align;
  unsigned w = wpSize << 3;
  body << "  " << name << " = getelementptr inbounds " << type << ", ";
  body << type << "* " << old.name << ", i" << w << " 0, i" << w << " 0\n";
  addStar();
  sp = 0;
}
/*
void lli::shift(int n) {
  unsigned w = wpSize << 3;
  body << "  %wparm = getelementptr inbounds " << type << ", ";
  body << type << "* %wstore, i" << w << " 0, i" << w << " " << n << "\n";
  *currentFunction << body;
}
*/
lle lli::stackptrw(StackOperationW o){
  lle e0;
  switch (o) {
    case CurrentWord:    e0 = getelement(sp-1);      break;
    case PushWord:       e0 = getelement(sp); sp+=1; break;
    case PopWord: sp-=1; e0 = getelement(sp);        break;
  }
  return e0;
}
lle lli::stackptrr(StackOperationR o){
  lle e0;
  switch (o) {
    case CurrentReal:    e0 = getelement(sp-2);      break;
    case PushReal:       e0 = getelement(sp); sp+=2; break;
    case PopReal: sp-=2; e0 = getelement(sp);        break;
  }
  lle    e1 = e0.bitcast ("float*");
  return e1;
}
void lli::pusha(unsigned int adr) {
  lle e0 = getelement(sp);
  e0.store (adr);
  sp++;
}
void lli::test(void) {
  if (sp)
    printf ("!!! SP=%d\n", sp);
}
void lli::clear(int n){
  sp = n;
}

llf::llf(ElementType t, const char* n, const int l): lle (t, 0, 0) {
  lls s("%");
  s << n;
  setName  (s);
  setFtype (l);
  body << "  " << name << " = alloca " << type << align << "\n";
}
lle llf::getelement(int no) {
  lle tmp  (* this);
  unsigned w = wpSize << 3;
  body << "  " << tmp.name << " = getelementptr inbounds " << tmp.type << ", ";
  body << type << "* " << name << ", i" << w << " 0, i" << w << " " <<  no << "\n";
  tmp.type = "i16*";  // trochu nelogicke, ale pouziva se jen pro stack
  * currentFunction << * this;
  return tmp;
}

llg::llg(ElementType t, const char* n) : lle (t, 0, 1) {
  lls s("%");
  s << n;
  setName(s);
  body << "  " << name << " = load " << type << ", " << type << "* @RamBasePtr, align " << wpSize << "\n";
}
lle  llg::getelement(int no) {
  lle tmp  (* this);
  unsigned w = wpSize << 3;
  body << "  " << tmp.name << " = getelementptr inbounds " << tmp.type << ", ";
  body << type << " " << name << ", i" << w << " 0, i32 0, i" << w << " " <<  no << "\n";
  tmp.type = "i16*";  // trochu nelogicke
  * currentFunction << * this;
  return tmp;
}
lle llg::getelement(const lle& from) {
  lle tmp  (* this);
  unsigned w = wpSize << 3;
  body << "  " << tmp.name << " = getelementptr inbounds " << tmp.type << ", ";
  body << type << " " << name << ", i" << w << " 0, i32 0, " << from.type << " " << from.name << "\n";
  tmp.type = "i16*";  // trochu nelogicke
  * currentFunction << * this;
  return tmp;
}
lle llg::getelementr(const lle& from) {
  lle tmp  (* this);
  unsigned w = wpSize << 3;
  body << "  " << tmp.name << " = getelementptr inbounds " << tmp.type << ", ";
  body << type << " " << name << ", i" << w << " 0, i32 0, " << from.type << " " << from.name << "\n";
  tmp.type = "i16*";  // trochu nelogicke
  * currentFunction << * this;
  lle flt  = tmp.bitcast ("float*");
  return flt;
}

/** *************************************************************************************/
llfnc::llfnc(const char* n) :
  carry     (llvm_i1,    "carry"),
  RegHl     (llvm_i16,   "RegHL"),
  RegRe     (llvm_float, "RegRE"),
  wstore    (llvm_i16,   "wstore", 16),
  wstack    (wstore,     "wstack"),  
  bstore    (llvm_i1,    "bstore", 16),
  bstack    (bstore,     "bstack"),  
  Variables (llvm_user,  "Variables"),
  param     (llvm_i16,   "param"),
  name (n),number(1)  {

    intfunc = false;
  
}
void llfnc::declare(void) {
  lls s /*("@RamBasePtr = external global %union.RamDef_u*\n")*/;
  s << "\n; Function Attrs: nounwind\ndefine void @";
  s << name;
  s << "() #0 {\n";
  body.push_back(s);
  * this << carry << RegHl << RegRe << wstore << wstack  << bstore << bstack<< Variables;
  wstack.type = "i16*";
  bstack.type = "i1*";
}

llfnc::~llfnc() {
}
llfnc& llfnc::operator<<(lle & add) {
  lls & s = add.body;
  body.push_back (s);
  s.clear();
  return * this;
}
llfnc& llfnc::operator<<(const char* add) {
  lls s (add);
  body.push_back (s);
  return * this;
}
void llfnc::prefix(const char* str) {
  lls s (str);
  vector<lls>::iterator it = body.begin();
  body.insert (it,1,s);
}

void llfnc::write(FILE* to) {
  int i, n = body.size();
  for (i=0; i<n; i++) {
    lls s = body.at(i);
    fprintf (to, "%s", (const char*) s);
  }
  body.clear();
}
void llfnc::exit(void) {
  lls s ("\n  ret void\n}\n");
  body.push_back(s);
}

lle llfnc::callGetBit(unsigned int adr) {
  int num = newnumber ();
  lle tmp  (llvm_i16, num);
  num = newnumber();
  lle obi  (llvm_i1,  num);
  tmp.body << "  " << tmp.name << " = tail call zeroext i16 @WgetBit(i32 " << adr << ") #2\n";
  obi.body << "  " << obi.name << " = trunc " << tmp.type << " " << tmp.name << " to " << obi.type << "\n";
  *this << tmp;
  *this << obi;
  return obi;
}
lle llfnc::callGetBit (lle & adr) {
  lle ext  (adr);
  ext.type = "i32";
  ext.body <<  "  " << ext.name << " = zext " << adr.type << " " << adr.name << " to " << ext.type << "\n";
  lle tmp  (ext);
  tmp.type = "i16";
  int num = newnumber();
  lle obi  (llvm_i1,  num);
  tmp.body << "  " << tmp.name << " = tail call zeroext i16 @WgetBit(i32 " << ext.name << ") #2\n";
  obi.body << "  " << obi.name << " = trunc " << tmp.type << " " << tmp.name << " to " << obi.type << "\n";
  *this << ext;
  *this << tmp;
  *this << obi;
  return obi;
}

void llfnc::callVoid(const char * fnc, unsigned int p1) {
  lls str;
  str << "  tail call void @" << fnc << "(i32 " << p1 << ") #2\n";
  *this << str;
}
void llfnc::callVoid(const char* fnc, lle& p1) {
  lle tmp (p1);
  tmp.type = "i32";
  tmp.body << "  " << tmp.name << " = zext " << p1.type << " " << p1.name << " to " << tmp.type << "\n";
  *this << tmp;

  lls str;
  str << "  tail call void @" << fnc << "(i32 " << tmp.name << ") #2\n";
  *this << str;
}

void llfnc::callVoid(const char * fnc, unsigned int p1, unsigned int p2) {
  lls str;
  str << "  tail call void @" << fnc << "(i32 " << p1 << ", i16 zeroext " << p2 << ") #2\n";
  *this << str;
}
void llfnc::callVoid(const char* fnc, unsigned int p1, lle& p2) {
  lle tmp (p2);
  tmp.type = "i16";
  tmp.body << "  " << tmp.name << " = zext " << p2.type << " " << p2.name << " to " << tmp.type << "\n";
  *this << tmp;
  lls str;
  str << "  tail call void @" << fnc << "(i32 " << p1 << ", " << tmp.type << " zeroext " << tmp.name << ") #2\n";
  *this << str;
}
void llfnc::callVoid(const char* fnc, lle& p1, unsigned int p2) {
  lle tmp (p1);
  tmp.type = "i32";
  tmp.body << "  " << tmp.name << " = zext " << p1.type << " " << p1.name << " to " << tmp.type << "\n";
  *this << tmp;
  lls str;
  str << "  tail call void @" << fnc << "(i32 " << tmp.name << ", " << p1.type << " zeroext " << p2 << ") #2\n";
  *this << str;
}

void llfnc::branch(unsigned int lab) {
  llabel l(lab);
  lls str;
  str << "  br label %" << l.L() << "\n";
  *this << str;
}
void llfnc::label(unsigned int lab) {
  llabel l(lab);
  lls str;
  str << l.L() << ":\n";
  *this << str;
}
void llfnc::calli(const char* name) {
  lls str;
  str << "  tail call void @" << name << "(i16* %wstack)\n";
  *this << str;
}
void llfnc::callf(const char* name, lle & param) {
  lls str;
  str << "  tail call void @" << name << "(i16* " << param.name << ")\n";
  *this << str;
}

/** *************************************************************************************/
llfnci::llfnci(const char* name): llfnc(name) {

}
void llfnci::declare(void){
  lls s;
  s << "\n; Function Attrs: inline nounwind\ndefine void @";
  s << name;
  s << "(i16* %param) #0 {\n";
  body.push_back(s);
  * this << carry << RegHl << RegRe << wstore << wstack  << bstore << bstack<< Variables;
  wstack.type = "i16*";
  bstack.type = "i1*";
  param.body.clear();
}
void llfnci::close(vector< lls >& to) {
  lls s ("\n  ret void\n}\n");
  body.push_back(s);
  int i, n = body.size();
  for (i=0; i<n; i++) {
    lls str = body.at(i);
    to.push_back(str);
  }
}
