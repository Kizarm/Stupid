#include "libwasm.h"
#include "../llvm/PesApi.h"
#include "timer.h"

extern "C" void   EXPORT(Loop)  ();
extern "C" void   EXPORT(init)  (const int memlen);
extern "C" void   EXPORT(DigitalInput) (const int port, const int val);
extern "C" void   EXPORT(AnalogInput)  (const int port, const int val);
extern "C" void   IMPORT(JSDisplay)      (const char * ptr, const int len);
extern "C" void   IMPORT(JsSetOutput)    (const int port, const int val);
extern "C" void   IMPORT(JsAnalogOutput) (const int port, const int val);
////////////////////////////////////////////////////////////////////////////////////
typedef void (*VHandlePR) (RamDef_t*);
VHandlePR   PSetRamBasePtr;
VHandleV    PSimple;
StationInfo GStationInfo;
RamDef_t    Variables;
SystemCheck GSystemCheck;

static PesApi_t * API;
extern "C" {
  extern void __wasm_call_ctors  ();
  RamDef_t * RamBasePtr;
};
static void WasmWordChangeDriver  (WORD * ptr, WORD newdata) {  // lze pouzit pro sitova volani
  return; // unused
  if (*ptr == newdata) return;
  const int diff = ptr - Variables.Words;
  printf("0x%04X => 0x%04X\n", diff, newdata);
}
static void WasmDisplayTextDriver (char * str) {
  JSDisplay (str, strlen (str));
}

void init (const int memlen) {
  _HEAP_MAX = reinterpret_cast<char*> (memlen);   // před prvním voláním malloc() - může být i v konstruktorech
  __wasm_call_ctors();                            // nutné volání statických konstruktorů pokud máme statické třídy
  
  printf("Module initialized\n\n");
  API = GetPesApiPtr ();
  PSimple        = API->MainPass;
  PSetRamBasePtr = API->SetRamBase;
  PSetRamBasePtr (& Variables);
  API->WordChangeDriver  = WasmWordChangeDriver;
  API->DisplayTextDriver = WasmDisplayTextDriver;
  memcpy (&GStationInfo, API->Info, sizeof (StationInfo));
  /**/
  printf ("Station adr = %d, RAM_BEGIN=0x%04X, RAM_END=0x%04X\n",
    GStationInfo.NetAddr, GStationInfo.DataBegin, GStationInfo.DataEnd);
  /**/
  Variables.SYS.RESET = 1;
}
void DigitalInput (int port, int val) {
  // printf("Set port %d to %d\n", port, val);
  WORD & VarDX0 = Variables.SYS.DX0;
  const WORD m = 1u << port;
  if (val) VarDX0 |=  m;
  else     VarDX0 &= ~m;
}
void AnalogInput (int port, int val) {
  //printf("Set port %d to %d\n", port, val);
  WORD * ptr = & Variables.SYS.I0;
  ptr[port] = WORD (val);
}
static void DY0_Out () {
  static WORD oDy0 = 0;
  WORD    & VarDY0 = Variables.SYS.DY0;
  if (oDy0 != VarDY0) {
    for (int n=0; n<16; n++) {
      const WORD m = 1u << n;
      if (VarDY0 & m) {
        if (!(oDy0 & m)) { JsSetOutput (n, 1); oDy0 |=  m; }
      } else {
        if   (oDy0 & m)  { JsSetOutput (n, 0); oDy0 &= ~m; }
      }
    }
  }
}
static void AnalogOut () {
  static WORD o [2];
  if (o[0] != Variables.SYS.O0) {
    JsAnalogOutput (0, Variables.SYS.O0);
    o [0] = Variables.SYS.O0;
  }
  if (o[1] != Variables.SYS.O1) {
    JsAnalogOutput (1, Variables.SYS.O1);
    o [1] = Variables.SYS.O1;
  }
}
//static int count = 0;
void Loop () {
  DY0_Out  ();
  AnalogOut();
  PSimple  ();
  GSystemCheck.check();
  //printf("Pass %d\n", count++);
}
