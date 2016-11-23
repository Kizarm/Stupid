#ifndef PLATFORMSLEEP_H
#define PLATFORMSLEEP_H

#include <stdint.h>

#define MB_OK          1
#define MB_ICONSTOP    2
#define MB_ICONWARNING 4

  #define SRED   "\x1B[31;1m"
  #define SGREEN "\x1B[32;1m"
  #define SBLUE  "\x1B[34;1m"
  #define SDEFC  "\x1B[0m"

#define ReportInfo printf

extern void QueryPerformanceCounter (int64_t * count);
extern void PlatformSleep (unsigned int msec);
extern void AfxMessageBox (const char * msg, int flags, void * ptr);
extern const char PathFileSeparator;

class CString {
public:
  CString () {data=0;};
  operator char*() const {return data;};
  void Format (const char * fmt, ...);
  ~CString();
private:
  char * data;
};

#endif // PLATFORMSLEEP_H
