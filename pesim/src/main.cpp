#include <QtGui/QApplication>
#include <QWindowsStyle>
#include "mainwindow.h"
#include <stdio.h>

#include "../../simple/llvm/PesApi.h"

int main (int argc, char** argv) {
  if (argc < 2) {
    printf ("Usage :\n\t%s drivername.so\n", argv[0]);
  }
  
  if (LoadLibPes(argv[1])) {
    return -1;
  }
  
  QApplication app(argc, argv);
//QApplication::setStyle (new QWindowsStyle);
  MainWindow foo;
  foo.show();
  int n = app.exec();
  ExitLibPes();
  return n;
}
