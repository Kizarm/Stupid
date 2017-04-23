#include <QApplication>
#include "mainwindow.h"
#include "platform.h"

int main (int argc, char * argv[]) {
  PlatformPerformanceCalibrate();
  QApplication app (argc, argv);
  MainWindow window;
  window.show();
  return app.exec();
}
