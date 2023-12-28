#include "libwasm.h"
#include "timer.h"
#include "../llvm/PesApi.h"

extern "C" double IMPORT(JsGetMiliseconds) ();
void QueryPerformanceCounter (uint64_t * count) {
  const double td = JsGetMiliseconds();
  const int64_t t = td;
  * count = t;
}

STimer::STimer () {
  QueryPerformanceCounter(&currms);
  currs  = currms / 1000;
  lastms = currms;
  lasts  = currs;
}
void STimer::loop ( void ) {
  QueryPerformanceCounter(&currms);
  currs = currms / 1000;
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
Timer0::Timer0() : STimer() {
}
void Timer0::pass ( void ) {
  STimer::loop();
  if (!Variables.SYS.TEN0) return;
  uint32_t increment;
  if (Variables.SYS.TPA0) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM0) Variables.SYS.T0 -= Variables.SYS.T0 > increment ? increment : Variables.SYS.T0;
  else                    Variables.SYS.T0 += increment;
}
Timer1::Timer1() : STimer() {
}
void Timer1::pass ( void ) {
  STimer::loop();
  if (!Variables.SYS.TEN1) return;
  uint32_t increment;
  if (Variables.SYS.TPA1) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM1) Variables.SYS.T1 -= Variables.SYS.T1 > increment ? increment : Variables.SYS.T1;
  else                    Variables.SYS.T1 += increment;
}

Timer2::Timer2() : STimer() {
}
void Timer2::pass ( void ) {
  STimer::loop();
  if (!Variables.SYS.TEN2) return;
  uint32_t increment;
  if (Variables.SYS.TPA2) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM2) Variables.SYS.T2 -= Variables.SYS.T2 > increment ? increment : Variables.SYS.T2;
  else                    Variables.SYS.T2 += increment;
}
Timer3::Timer3() : STimer() {
}
void Timer3::pass ( void ) {
  STimer::loop();
  if (!Variables.SYS.TEN3) return;
  uint32_t increment;
  if (Variables.SYS.TPA3) increment = inc_s;
  else                    increment = inc_ms;
  if (Variables.SYS.TDM3) Variables.SYS.T3 -= Variables.SYS.T3 > increment ? increment : Variables.SYS.T3;
  else                    Variables.SYS.T3 += increment;
}

