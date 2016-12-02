#ifndef DIGITALIO_H
#define DIGITALIO_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "iogbl.h"

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
