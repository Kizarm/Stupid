#ifndef QT_WRAP_H
#define QT_WRAP_H

#include <stdio.h>
#include <stdint.h>
#include <QtNetwork/QUdpSocket>
#include <QThread>
#include <QMutex>

class QtWrap : public QObject {
    Q_OBJECT
  public:
    QtWrap (QObject * parent);
    virtual ~QtWrap ();
    unsigned send (const char * data, unsigned len);
    unsigned rcvd (const char * data, unsigned len);
    void DisplayText (const char * text);
  signals:
    void SetText (const QString & str);
  private:
    unsigned     port;
    QUdpSocket * socket;
  public slots:
    void readPendingDatagrams (void);
};

#endif // QT_WRAP_H
