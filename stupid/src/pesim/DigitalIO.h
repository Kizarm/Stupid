#ifndef DIGITALIO_H
#define DIGITALIO_H
#include <QtCore>
#include <QtGui>
#include <QWidget>
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "../pesim/inc/iogbl.h"

class DigitalIO : public QWidget {
  Q_OBJECT
  public:
    DigitalIO    (QWidget * parent, const QString & name, const IO_Dir dir, const bool bistable = false);
    ~DigitalIO   ();
    
    void Enable   (bool b);
    void SetState (bool s);
    bool GetState (void) const {
      return state;
    }
    void StateChange (void);
    void SetBistable (const bool b) {
      bistable = b;
    }
  public slots:
    void ChangeIcon  (void);
    
    void ButtPressed  (void);
    void ButtReleased (void);
  signals:
    void IconChange  (void);
  private:
    // QWidget     * widget;
    QVBoxLayout * verticalLayout;
    QLabel      * label;
    QPushButton * pushButton;

    QPixmap       * on, * off;
    const IO_Dir  direction;
    bool          state;
    bool          bistable;
};

#endif // DIGITALIO_H
