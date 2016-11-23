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

class lle {
  public:
    lle (ElementType type, const char * name=0, int ptrn=0, int number=0);
    lle (ElementType type, const int order);
    lle (const lle & from, const char * name);
    lle (const lle & from);
    std::string & getBody (void);
    lle & getelement (int no);
    lle & getelement (const lle & from);
    lle & getglobal  (int no);
    
    void store (lle & e);
    lle  load  (void);
    lle  binaryOp   (const char * op, unsigned r);
    ~lle();
  protected:
    void setFullName (void);
  private:
    int         nume;
    int         ptrn;
    ElementType type;
    std::string name;
    std::string full;
    std::string styp;
    std::string body;
    bool        ainc;
    int         ordr;
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
    //lle     binaryOp   (const char * op, lle & l, unsigned r);
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
    lle                      carry;
    lle                      RegHl;
    lle                      RegRe;
    
    lle                      stock;
    lle                      stack;
    lle                      Variables;
  private:
    int                      number;
    std::string              name;
    std::vector<std::string> body;
    char                   * buf;
    const int                maxbuf;
    int                      labnum;
};

#endif // LLVMCOMPONENT_H
