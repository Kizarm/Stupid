#ifndef IO_GBL_H
#define IO_GBL_H
#include <vector>

enum IO_Dir {
  IO_Input = 0,
  IO_Output
};
enum NET_Dir {
  NET_Input  = 1,
  NET_Output = 2,
  NET_Both   = 3,
};

class QtWrap;
class DigitalIO;
class AnalogINP;
class AnalogOUT;

static const unsigned MaxIO = 16;
static const unsigned MaxAD = 2;

struct IOPort {
  DigitalIO * io;
  bool        used;
};
struct ADPort {
  AnalogINP * io;
  bool        used;
};
struct DAPort {
  AnalogOUT * io;
  bool        used;
};
struct IODescriptors {
  unsigned NetAddr;
  QtWrap * wrap;
  IOPort inp [MaxIO];
  IOPort out [MaxIO];
  ADPort adc [MaxAD];
  DAPort dac [MaxAD];
};
extern IODescriptors GIODescriptor;

extern void init (void);
extern void loop (void);

class LoopBase {
  public:
    LoopBase(){};
    virtual void pass (void)=0;
};
struct SystemCheck {
  std::vector<LoopBase*> passes;
  void append (LoopBase * p) {
    passes.push_back(p);
  }
  void check (void) {
    unsigned i, n= passes.size();
    for (i=0; i<n; i++) {
      LoopBase * p = passes.at(i);
      p->pass();
    }
  }
};
extern SystemCheck GSystemCheck;

#endif // IO_GBL_H