#ifndef LLVMSTACK_H
#define LLVMSTACK_H
#include <stdint.h>

class LLVMStack {
  public:
    LLVMStack();
    ~LLVMStack();
    
    void Declare (void);
    void Define  (const int depth, const char * wide);
    void ZeroTest(void);
    
    void PushAddrInBytes   (unsigned addr, unsigned long & per);
    void PushWordAtCurrent (unsigned long & per);
    void PopWordAtCurrent  (unsigned long & per);
    void PopWord           (unsigned long & per);
    void CurrentWordPtr    (unsigned long & per);
    void PushImmWord       (unsigned short imm, unsigned long & per);
    void PopWordPtr        (unsigned long & per);
    void DecrementStack    (int minus);
    
    void PushImmReal       (uint64_t realImage, unsigned long & per);
    void CurrentRealPtr    (unsigned long & per);
    void PopReal           (unsigned long & per);
    void PushReal          (unsigned long & per);
    void PopRealPtr        (unsigned long & per);
    
    void clear (void) {sp=0;}
    unsigned short depth (void) {return sp;};
  protected:
    void BitCastTo         (unsigned long & per, const char * to);
    void GetElementPtr     (unsigned long & per, int ofs=0);
  private:
    int sp;
};

#endif // LLVMSTACK_H
