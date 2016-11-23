/*
 */

#ifndef LLVMCOMPONENT_H
#define LLVMCOMPONENT_H
#include <stdio.h>
#include <vector>
#include <string>

class llfnc;

extern llfnc  * currentFunction;
extern unsigned wpSize;

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
    
    lls & operator+= (const char * add) {
      data.append (add);
      return * this;
    }
    lls & operator+= (const lls  & add) {
      data.append(add.data);
      return * this;
    }
    
    friend const lls operator+ (const lls & l, const lls & r) {
      lls tmp;
      tmp += l; tmp += r;
      return tmp;
    }
    friend const lls operator+ (const lls & l, const int r) {
      const int max = 64;
      char  buf [max];
      snprintf (buf, max, "%d", r);
      lls tmp (l);
      tmp += buf;
      return tmp;
    }
    
    operator const char*() const {
      return data.c_str();
    }
    void clear (void) {data.clear();}
  private:
    std::string data;
};

class lle {
  public:
    lle(){};
    lle (ElementType type, const int order, const int ptrn=0);
    lle (const lle & from);
    void setName (lls & newname);
    void setFtype(const int l);
    void addStar (bool a=true);

    lle  getelement (int no);
    lle  getelement (const lle & from);
    
    void store (lle & e);    
    void store (unsigned n);
    lle  load  (void);
    lle  binaryOp   (const char * op, unsigned r);
    ~lle();
  protected:
  public:
    lls         align;
    lls         name;
    lls         type;
    lls         body;
};
class llf : public lle {
  public:
    llf():lle(){};
    llf (ElementType type, const char * name, const int len);
    ~llf(){};
};
class lli : public lle {
  public:
    lli():lle(){};
    lli (const lle & old, const char * name);
    lli (ElementType type, const char * name, const int ptrn=0);
    ~lli(){};
};
class llg : public lle {
  public:
    llg (ElementType type, const char * name);
    ~llg(){};
};

class llfnc {
  public:
    llfnc (const char * name);
    void    prefix (const char * str);
    llfnc & operator<< (lle & add);
    llfnc & operator<< (const char *  add);
    void    write (FILE * to);
    void    exit  (void);
    
    lle     callGetBit (unsigned adr);
    int  newnumber (void) {
      int result = number;
      increment();
      return result;
    }
   ~llfnc ();
  protected:
    void increment (void) {
      number += 1;
    }
  public:
    lli                      carry;
    lli                      RegHl;
    lli                      RegRe;
    
    llf                      stock;
    lli                      stack;
    llg                      Variables;
  private:
    int                      number;
    lls                      name;
    std::vector<lls>         body;
    //char                   * buf;
    //const int                maxbuf;
    int                      labnum;
};
#endif // LLVMCOMPONENT_H
