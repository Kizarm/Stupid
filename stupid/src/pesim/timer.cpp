#include <stdio.h>
#include <time.h>
#include <math.h>
//#include <sys/times.h>
#include "../pesim/inc/timer.h"
#include "../../posix/simple/llvm/PesApi.h"

void QueryPerformanceCounter (uint64_t * count) {
  long            ms; // des. Milliseconds
  time_t          s;  // Seconds
  struct timespec spec;
  clock_gettime(CLOCK_REALTIME, &spec);
  s  = spec.tv_sec;
  ms = round(spec.tv_nsec / 1.0e7);
  * count = 100 * s + ms;
}

STimer::STimer () {
  currms = 0;
  lastms = 0;
  currs  = 0;
  lasts  = 0;
}

STimer::~STimer() {
}

void STimer::pass ( void ) {
  QueryPerformanceCounter(&currms);
  currs = currms / 100;
  if (currms != lastms) {
    inc_ms = currms - lastms;
    lastms = currms;
  } else {
    inc_ms = 0;
  }
  if (currs != lasts) {
    inc_s = currs - lasts;
    lasts = currs;
  } else {
    inc_s = 0;
  }
}
/******************************************************************/
Timer0::Timer0() {
  GSystemCheck.append(this);
}
void Timer0::pass ( void ) {
  STimer::pass();
  if (!Variables.SYS.TEN0) return;
  uint32_t increment;
  if (Variables.SYS.TPA0) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM0) Variables.SYS.T0 -= increment;
  else                    Variables.SYS.T0 += increment;
}
Timer1::Timer1() {
  GSystemCheck.append(this);
}
void Timer1::pass ( void ) {
  STimer::pass();
  if (!Variables.SYS.TEN1) return;
  uint32_t increment;
  if (Variables.SYS.TPA1) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM1) Variables.SYS.T1 -= increment;
  else                    Variables.SYS.T1 += increment;
}

Timer2::Timer2() {
  GSystemCheck.append(this);
}
void Timer2::pass ( void ) {
  STimer::pass();
  if (!Variables.SYS.TEN2) return;
  uint32_t increment;
  if (Variables.SYS.TPA2) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM2) Variables.SYS.T2 -= increment;
  else                    Variables.SYS.T2 += increment;
}
Timer3::Timer3() {
  GSystemCheck.append(this);
}
void Timer3::pass ( void ) {
  STimer::pass();
  if (!Variables.SYS.TEN3) return;
  uint32_t increment;
  if (Variables.SYS.TPA3) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM3) Variables.SYS.T3 -= increment;
  else                    Variables.SYS.T3 += increment;
}

