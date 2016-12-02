#include <QtGui/QApplication>
#include <QWindowsStyle>
#include "mainwindow.h"
#include <stdio.h>

#include "../../simple/llvm/PesApi.h"

int main(int argc, char** argv) {
  const char * library = "/home/mrazik/lib/libpes.so";
  if (LoadLibPes(library)) {
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
