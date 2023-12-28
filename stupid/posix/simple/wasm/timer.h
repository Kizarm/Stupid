#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

class STimer {
  public:
    STimer ();
    void loop (void);
    uint32_t inc_s;
    uint32_t inc_ms;
  private:
    uint64_t currms;
    uint64_t lastms;
    uint64_t currs;
    uint64_t lasts;
};
class Timer0 : public STimer {
  public:
    Timer0();
    void pass (void);
};
class Timer1 : public STimer {
  public:
    Timer1();
    void pass (void);
};
class Timer2 : public STimer {
  public:
    Timer2();
    void pass (void);
};
class Timer3 : public STimer {
  public:
    Timer3();
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
struct SystemCheck {
  Timers tims;
  explicit SystemCheck () : tims() {
  };
  void check (void) {
    tims.Tim0.pass();
    tims.Tim1.pass();
    tims.Tim2.pass();
    tims.Tim3.pass();
  }
};

#endif // TIMER_H
