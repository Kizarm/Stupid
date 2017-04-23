#ifndef WATCHLOOP_H
#define WATCHLOOP_H

#include <QThread>

class CPesNet;
class QMutex;
class CUSBPes;
class WatchElement;

class WatchLoop : public QThread  {
  Q_OBJECT
  public:
    WatchLoop (QObject * parent = 0);
    virtual ~WatchLoop ();
    void run (void);
    void lock  (bool state);
    void Start (bool state);
    void ForceStop   (void);
    void Delay (int ms) {
      msleep (ms);
    };
    
    QMutex * getMutex (void)        { return mutex; };
    void setPes  (CPesNet * p)      { pes = p;      };
    void setList (QList<WatchElement*>*l) {list = l;};
  signals:
    void VariableChanged (int row);
    void Terminated      (void);
  private:
    QList<WatchElement *> * list;
    QMutex                * mutex;
    CPesNet               * pes;
    volatile bool           loop;
};

#endif // WATCHLOOP_H
