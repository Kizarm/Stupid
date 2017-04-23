#ifdef QT_WIDGETS_LIB
#include <QtWidgets/QApplication>
#else  // QT_WIDGETS_LIB
#include <QApplication>
#endif // QT_WIDGETS_LIB
//#include <QWindowsStyle>
#include "pesim.h"
#include <stdio.h>
#include <stdlib.h>

#include "../../../posix/simple/llvm/PesApi.h"

int main (int argc, char** argv) {
  if (argc < 2) {
    printf ("Usage :\n\t%s drivername.so\n", argv[0]);
  }
  const char * so = argv[1];
  char * pd = getenv ("PWD"); // full path name
  const int max = strlen(pd) + strlen(so) + 2;
  char buf [max];
  if (strstr(so, "/")) snprintf(buf, max, "%s",        so);
  else                 snprintf(buf, max, "%s/%s", pd, so);
  printf ("Use: \"%s\"\n", buf);
  if (LoadLibPes(buf)) {
    return -1;
  }
  
  QApplication app(argc, argv);
//QApplication::setStyle (new QWindowsStyle);
  SimDialog foo;
  foo.StartSimulator();
  int n = app.exec();
  return n;
}
