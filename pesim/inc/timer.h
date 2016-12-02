#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>
#include "iogbl.h"

class Timer : public LoopBase {
  public:
    Timer ();
    virtual ~Timer();
    virtual void pass (void);
    uint32_t inc_s;
    uint32_t inc_ms;
  private:
    uint64_t currms;
    uint64_t lastms;
    uint64_t currs;
    uint64_t lasts;
};
class Timer0 : public Timer {
  public:
    Timer0();
    virtual ~Timer0(){};
    void pass (void);
};
class Timer1 : public Timer {
  public:
    Timer1();
    virtual ~Timer1(){};
    void pass (void);
};
class Timer2 : public Timer {
  public:
    Timer2();
    virtual ~Timer2(){};
    void pass (void);
};
class Timer3 : public Timer {
  public:
    Timer3();
    virtual ~Timer3(){};
    void pass (void);
};

class Timers {
public:
  Timers() : Tim0(), Tim1(), Tim2(), Tim3() {};
  Timer0 Tim0;
  Timer1 Tim1;
  Timer2 Tim2;
  Timer3 Tim3;
};

#endif // TIMER_H
