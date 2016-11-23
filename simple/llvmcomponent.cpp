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
  {" align 2","i1"}, {" align 2","i16"}, {" align 4","i32"}, {" align 4","float"},
  {" align 2","%union.RamDef_u"}
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

lle::lle (ElementType t, const int o, const int n) :
  align(Desc[t].align), name (o), type (t,n) {
}
// kopirovaci konstruktor pouzivany pro docasne objekty pro funkce typu "load"
lle::lle (const lle& from) {
  int num = currentFunction->newnumber ();
  align = from.align;
  lls s (num);
  name  = s;
  type  = from.type;
  addStar(false);
  printf("Kopirovaci pro %s, type %s\n", (const char*)name, (const char*)type);
}
lle::~lle() {

}
lle lle::getelement(const lle& from) {
  return * this;
}
lle  lle::getelement(int no) {
  lle tmp  (* this);
  unsigned w = wpSize << 3;
  body += "  " + tmp.name + " = getelementptr inbounds " + tmp.type + ", ";
  body += type + " " + name + ", i" + w + " 0, i32 0, i" + w + " " +  no + "\n";
  tmp.type = "i16*";  // trochu nelogicke
  * currentFunction << * this;
  return tmp;
}
void lle::store(lle& e) {
  body += "  store " + e.type + " " + e.name + ", " + type + " " + name + "," + align + "\n";
  * currentFunction << * this;
}
void lle::store(unsigned int n) {
  body.clear();
  lls s ("  store i16 ");
  body = s + n + ", " + type + " " + name + "," + align + "\n";
  * currentFunction << * this;
}

lle lle::load(void) {
  lle tmp  (* this);
  body += "  " + tmp.name + " = load " + tmp.type + ", " + type + " " + name + "," + align + "\n";
  * currentFunction << * this;
  return tmp;
}
lle lle::binaryOp(const char* op, unsigned int r) {
//  int num = currentFunction->newnumber ();
  lle tmp  (* this);
  body += "  " + tmp.name + " = " + op + " " + type + " " + name + ", " + r + "\n";
  * currentFunction << * this;
  return tmp;
}
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
  s += n;
  setName(s);
  body += "  " + name + " = alloca " + type + "," + align + "\n";
  addStar();
}
lli::lli(const lle& old, const char* n) {
  lls s("%");
  s += n;
  setName(s);
  type = old.type;
  unsigned w = wpSize << 3;
  body += "  " + name + " = getelementptr inbounds " + type + ", ";
  body += type + "* " + old.name + ", i" + w + " 0, i" + w + " 0\n";
  addStar();
}

llf::llf(ElementType t, const char* n, const int l): lle (t, 0, 0) {
  lls s("%");
  s += n;
  setName  (s);
  setFtype (l);
  body += "  " + name + " = alloca " + type + "," + align + "\n";
}
llg::llg(ElementType t, const char* n) : lle (t, 0, 1) {
  lls s("%");
  s += n;
  setName(s);
  body += "  " + name + " = load " + type + ", " + type + "* @RamBasePtr," + align + "\n";
}

/** *************************************************************************************/
llfnc::llfnc(const char* n) :
  carry     (llvm_i16,   "carry"),
  RegHl     (llvm_i16,   "RegHl"),
  RegRe     (llvm_float, "RegRe"),
  stock     (llvm_i16,   "stock", 16),
  stack     (stock,      "stack"),  
  Variables (llvm_user,  "Variables"),
  number(1), name (n)  {

  labnum = 0;
  
  lls s ("@RamBasePtr = external global %union.RamDef_u*\n");
  s += "\n; Function Attrs: nounwind\ndefine void @";
  s += name;
  s += "() #0 {\n";
  body.push_back(s);
  * this << carry << RegHl << RegRe << stock << stack << Variables;
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
  tmp.body += "  " + tmp.name + " = tail call zeroext i16 @WgetBit(i32 " + adr + ") #2\n";
  *this << tmp;
  return tmp;
}
