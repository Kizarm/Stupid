#include "platform.h"

#ifdef UNIX
#include <unistd.h>
#include <time.h>
const char PathFileSeparator = '/';
//! TODO: for windows
void QueryPerformanceCounter (int64_t * count) {
  clock_t cl = clock ();
  int64_t ms = CLOCKS_PER_SEC / 1000;
  * count = cl / ms;
}
void PlatformSleep (unsigned int msec) {
  usleep (1000 * msec);
}
#else // ! UNIX
#include <windows.h>
const char PathFileSeparator = '\\';

void PlatformSleep (unsigned int msec) {
  Sleep (msec);
}

#endif // UNIX

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void AfxMessageBox (const char * msg, int flags, void * ptr) {
  printf (SRED"ErrMsg: "SBLUE"\"%s\""SDEFC"\n", msg);
}
void CString::Format (const char * fmt, ...) {
  if (data) delete [] data;
  int len = strlen (fmt) + 256;
  data = new char [len];
  va_list arglist;
  va_start (arglist, fmt);
  vsnprintf (data, len, fmt, arglist);
  va_end (arglist);
}
CString::~CString() {
  if (data) delete [] data;
}
