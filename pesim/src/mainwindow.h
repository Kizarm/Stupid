#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QVector>
#include "iogbl.h"

class Ui_MainWindow;
class Loop;
class QtWrap;
class QFrame;

class MainWindow : public QMainWindow {
  Q_OBJECT
  public:
    MainWindow (QWidget * parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
  public slots:
  protected:
    // void clean (void);
  private:
    QIcon           ico;
    Ui_MainWindow * ui;
    Loop          * user;
    //QtWrap        * wrap;
    QFrame        * line;
};

#endif // MAINWINDOW_H
