/*
 */

#ifndef LLVMCOMPONENT_H
#define LLVMCOMPONENT_H
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>

class llfnc;

extern llfnc  * currentFunction;
extern unsigned wpSize;

union D2U {
  uint64_t u;
  double   d;
};

enum ElementType {
  llvm_i1, llvm_i16, llvm_i32, llvm_float,
  llvm_user
};
class lls {
  public:
    lls () {data.clear();};
    lls (const ElementType t, const int ptrn);
    lls (const int n);
    lls (const      char   * s) : data(s) {};
    lls (const std::string & s) : data(s) {};
    lls (const lls & old) {data = old.data;};
    
    lls & operator<< (const char * add) {
      data.append (add);
      return * this;
    }
    lls & operator<< (const int r) {
      const int max = 64;
      char  buf [max];
      snprintf (buf, max, "%d", r);
      data.append(buf);
      return * this;
    }
    operator const char*() const {
      return data.c_str();
    }
    void clear (void) {data.clear();}
  private:
    std::string data;
};

class llt {
  public:
    llt (){no=0;};
    void display (const char * str);
    void close   (llfnc & to);
    ~llt(){};
  public:
    int              no;
  private:
    std::vector<lls> def;
};

class llabel {
  public:
    llabel (unsigned n) {
      no = n;
    }
    lls D (void) {
      lls tmp;
      const int max = 64; char  buf [max];
      snprintf (buf, max, "D%04X", no);
      tmp << buf;
      return tmp;
    }
    lls L (void) {
      lls tmp;
      const int max = 64; char  buf [max];
      snprintf (buf, max, "L%04X", no);
      tmp << buf;
      return tmp;
    }
  private:
    unsigned no;
};

class lle {
  public:
    lle(){width=2;};
    lle (ElementType type, const int order, const int ptrn=0);
    lle (const lle & from);
    void setWidth(unsigned n=2) {
      width = n;
    }
    void setName (lls & newname);
    void setFtype(const int l);
    void addStar (bool a=true);

    virtual lle  getelement (int no);
    lle  getelement (const lle & from);
    lle  bitcast    (const char * to);
    
    void store (lle & e);    
    void store (unsigned n);
    void stored(double n);
    lle  load  (void);
    void callw      (const char * op, lle & r);
    lle  binaryOp   (const char * op, unsigned r);
    lle  binaryOp   (const char * op, lle & r);
    lle  binaryRel  (const char * op, unsigned r);
    lle  binaryRel  (const char * op, lle & r);
    lle  binaryRelRe(const char * op, lle & r);
    
    void condBranch (unsigned lab);
    void wordChange (unsigned adr);
    void wordChange (const lle & w);
    lle  callTest   (unsigned p);
    ~lle();
  protected:
  public:
    unsigned    width;
    lls         align;
    lls         name;
    lls         type;
    lls         body;
};
class llf : public lle {
  public:
    llf():lle(){};
    llf (ElementType type, const char * name, const int len);
    lle  getelement (int no);
    ~llf(){};
};
enum StackOperationW {
  CurrentWord,
  PushWord,
  PopWord
};
enum StackOperationR {
  CurrentReal,
  PushReal,
  PopReal
};

class lli : public lle {
  public:
    lli():lle(){sp=0;};
    lli (const lle & old, const char * name);
    lli (ElementType type, const char * name, const int ptrn=0);
    
    void pusha  (unsigned adr);
    
    lle stackptrw (StackOperationW);
    lle stackptrr (StackOperationR);
    void test  (void);
    void clear (int n=0);
    //void shift (int n=2);
    ~lli(){};
  private:
    int sp;
};
class llg : public lle {
  public:
    llg (ElementType type, const char * name);
    lle  getelement (int no);
    lle  getelement (const lle & from);
    lle  getelementr(const lle & from);
    ~llg(){};
};
/** *****************************************************************************/
class llfnc {
  public:
    llfnc (const char * name);
    void    prefix (const char * str);
    llfnc & operator<< (lle & add);
    llfnc & operator<< (const char *  add);
    void    write (FILE * to);
    void    exit  (void);
    void    branch (unsigned lab);
    void    label  (unsigned lab);
    
    virtual void    declare (void);
    lle     callGetBit (unsigned adr);
    lle     callGetBit (lle  &   adr);
    void    callVoid   (const char * fnc, unsigned p1);
    void    callVoid   (const char * fnc, lle  &   p1);
    void    callVoid   (const char * fnc, unsigned p1, unsigned p2);
    void    callVoid   (const char * fnc, unsigned p1, lle  &   p2);
    void    callVoid   (const char * fnc, lle  &   p1, unsigned p2);
    int  newnumber (void) {
      int result = number;
      increment();
      return result;
    }
    void calli (const char * name);
    void callf (const char * name, lle & param);
    virtual ~llfnc ();
    //void stack(StackOperationW PopWord);
  protected:
    void increment (void) {
      number += 1;
    }
  public:
    lli                      carry;
    lli                      RegHl;
    lli                      RegRe;
    
    llf                      wstore;
    lli                      wstack;
    llf                      bstore;
    lli                      bstack;
    llg                      Variables;
    lli                      param;
    lls                      name;
    std::vector<lls>         body;
    bool                     intfunc;
  private:
    int                      number;
};
class llfnci : public llfnc {
  public:
    llfnci (const char* name);
    void    declare (void);
    void    close   (std::vector<lls> & to);
    virtual ~llfnci(){};
  public:
};

#endif // LLVMCOMPONENT_H
