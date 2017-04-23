#include <stdio.h>
#include "Loop.h"
#include "wrap.h"
#include "iogbl.h"
#include "gpio.h"
#include "adac.h"
#include "timer.h"
#include "../../simple/llvm/PesApi.h"


SystemCheck GSystemCheck;
static ADCI I0(0, & Variables.SYS.I0);
static DACO O0(0, & Variables.SYS.O0);
static ADCI I1(1, & Variables.SYS.I1);
static DACO O1(1, & Variables.SYS.O1);

static GPIO X0 (IOPortX, 0, IO_Input, & Variables.SYS.DX0), Y0 (IOPortY, 0, IO_Output, & Variables.SYS.DY0);
static GPIO X1 (IOPortX, 1, IO_Input, & Variables.SYS.DX0), Y1 (IOPortY, 1, IO_Output, & Variables.SYS.DY0);
static GPIO X2 (IOPortX, 2, IO_Input, & Variables.SYS.DX0), Y2 (IOPortY, 2, IO_Output, & Variables.SYS.DY0);
static GPIO X3 (IOPortX, 3, IO_Input, & Variables.SYS.DX0), Y3 (IOPortY, 3, IO_Output, & Variables.SYS.DY0);
static GPIO X4 (IOPortX, 4, IO_Input, & Variables.SYS.DX0), Y4 (IOPortY, 4, IO_Output, & Variables.SYS.DY0);
static GPIO X5 (IOPortX, 5, IO_Input, & Variables.SYS.DX0), Y5 (IOPortY, 5, IO_Output, & Variables.SYS.DY0);
static GPIO X6 (IOPortX, 6, IO_Input, & Variables.SYS.DX0), Y6 (IOPortY, 6, IO_Output, & Variables.SYS.DY0);
static GPIO X7 (IOPortX, 7, IO_Input, & Variables.SYS.DX0), Y7 (IOPortY, 7, IO_Output, & Variables.SYS.DY0);
static GPIO X8 (IOPortX, 8, IO_Input, & Variables.SYS.DX0), Y8 (IOPortY, 8, IO_Output, & Variables.SYS.DY0);
static GPIO X9 (IOPortX, 9, IO_Input, & Variables.SYS.DX0), Y9 (IOPortY, 9, IO_Output, & Variables.SYS.DY0);
static GPIO X10(IOPortX,10, IO_Input, & Variables.SYS.DX0), Y10(IOPortY,10, IO_Output, & Variables.SYS.DY0);
static GPIO X11(IOPortX,11, IO_Input, & Variables.SYS.DX0), Y11(IOPortY,11, IO_Output, & Variables.SYS.DY0);
static Timers TIMS;
// Toto je jedina definice !
RamDef_t Variables;
extern uint16_t swap_bytes (uint16_t word);

void ApiNetDriver (WORD * ptr, WORD newdata) {
  WORD * base = Variables.Words;
  size_t index = (ptr - base);
  WORD olddata = * ptr;
  if ((index < 0x60) || (index >= 0x80)) return; // sitove wordy
  if (olddata == newdata)                return; // stara data jsou stejna
  //-printf ("new WORD at %04lX, %04X => %04X\n", index, olddata, newdata);
  
  const unsigned max = 256;
  char buf [max];
  unsigned char hdr = GIODescriptor.NetAddr;//0x6F;
  unsigned len = snprintf(buf, max, "#%02X:%02lX%04X\r\n", hdr, index, /*swap_bytes*/ (newdata));
  //printf("%s", buf);
  GIODescriptor.wrap->send (buf, len);
}
void ApiDispText (char * str) {
  printf ("DISPLAY : \"%s\"\n", str);
}
void init (void) {
  Variables.SYS.RESET=1;
}
void loop (void) {
  GSystemCheck.check();
  PSimple();
}

Loop::Loop (QObject * parent) : QThread (parent) {
  running = true;
  start();
}
Loop::~Loop() {
  if (isRunning()) terminate ();
  wait();
  /*
  running = false;
  for (;;) {
    if (isFinished()) break;
  }
  */
}
void Loop::run (void) {
  init();
  while (running) {
    usleep (1000);
    loop   ();
  }
}
