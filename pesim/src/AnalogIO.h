#ifndef ANALOGIO_H
#define ANALOGIO_H

#include <stdint.h>
#include <QWidget>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QProgressBar>

class AnalogINP : public QWidget {
  Q_OBJECT
  public:
    AnalogINP  (QWidget * parent, const QString & name);
    ~AnalogINP ();
    
    void     Enable   (bool b);
    void     SetValue (uint16_t v);
    uint16_t GetValue (void) const {
      return value;
    }
  public slots:
    void SlideChange (int);
    void ValueChange (quint16 v);
  signals:
    void ChangeValue (quint16 v);
  private:
    QLabel * name;
    QLabel * sval;
    QHBoxLayout * horizontalLayout;
    QSlider     * horizontalSlider;

    uint16_t value;
};

class AnalogOUT : public QWidget {
  Q_OBJECT
  public:
    AnalogOUT  (QWidget * parent, const QString & name);
    ~AnalogOUT ();
    
    void     Enable   (bool b);
    void     SetValue (uint16_t v);
    uint16_t GetValue (void) const {
      return value;
    }
  public slots:
    void ValueChange (quint16 v);
  signals:
    void ChangeValue (quint16 v);
  private:
    QLabel * name;
    QHBoxLayout  * horizontalLayout;
    QProgressBar * progressBar;

    uint16_t value;
};

#endif // ANALOGIO_H
