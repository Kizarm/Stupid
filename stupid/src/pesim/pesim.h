#ifndef PESIMHEAD_H
#define PESIMHEAD_H

#ifdef QT_WIDGETS_LIB
#include <QtWidgets/QDialog>
#else
#include <QDialog>
#endif
#include <QIcon>
#include <QVector>
#include "../pesim/inc/iogbl.h"

class Ui_SimDialog;
class Loop;
class QtWrap;
class QFrame;
class QCloseEvent;

class SimDialog : public QDialog {
  Q_OBJECT
  public:
    SimDialog (QWidget * parent = 0, Qt::WindowFlags flags = 0);
    void StartSimulator (void);
    ~SimDialog();
  public slots:
  protected:
    void closeEvent ( QCloseEvent * event );
  private:
    QIcon           ico;
    Ui_SimDialog  * ui;
    Loop          * user;
    QtWrap        * wrap;
    QFrame        * line;
};

#endif // PESIMHEAD_H
