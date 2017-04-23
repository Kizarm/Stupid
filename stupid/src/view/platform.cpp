#include "platform.h"

static uint64_t Performance1mS = 0;

#ifdef UNIX
#include <unistd.h>
#include <time.h>
const char PathFileSeparator = '/';
//! TODO: for windows
void PlatformPerformanceCounter (int64_t * count) {
  clock_t cl = clock ();
  int64_t ms = CLOCKS_PER_SEC / 1000;
  * count = cl / ms;
}
void PlatformSleep (unsigned int msec) {
  usleep (1000 * msec);
}
void PlatformPerformanceCalibrate (void) {
  Performance1mS = CLOCKS_PER_SEC / 1000;
}
#else // ! UNIX
#include <windows.h>
const char PathFileSeparator = '\\';

void PlatformPerformanceCalibrate (void) {
  LARGE_INTEGER freq;
  QueryPerformanceFrequency(&freq);
  Performance1mS = freq.QuadPart / 1000;
  if (Performance1mS == 0UL) Performance1mS = 1;
}

void PlatformPerformanceCounter (int64_t * count) {
  LARGE_INTEGER wd;
  QueryPerformanceCounter (&wd);
  *count = wd.QuadPart / Performance1mS;
  return;
}

void PlatformSleep (unsigned int msec) {
  Sleep (msec);
}

#endif // UNIX

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
/*
void AfxMessageBox (const char * msg, int , void * ) {
  printf (SRED"ErrMsg: "SBLUE"\"%s\""SDEFC"\n", msg);
}
*/
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
