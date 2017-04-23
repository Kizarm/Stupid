#include "watchloop.h"
#include "watchelement.h"
#include <QMutex>

WatchLoop::WatchLoop (QObject * parent) : QThread (parent) {
  mutex = new QMutex;
  loop  = true;
}
WatchLoop::~WatchLoop() {
  ForceStop();
  delete mutex;
}
void WatchLoop::run (void) {
  unsigned value;
  bool changed, result=true;
  qDebug ("Thread Started");
  while (loop) {
    usleep (100000);
    //qDebug ("Thread Pass");
    mutex->lock();
    do {
      int n = list->size();
      if (!n) break;
      for (int i=0; i<n; i++) {
        WatchElement * e = list->at (i);
        result = e->ValueChanged (value, changed);
        if (!result)   break;
        if (!changed)  continue;
        emit VariableChanged (i);
      }
    } while (false);
    mutex->unlock();
    if (!result) break;
  }
  if (loop) {   // unexpected end loop - error
    loop = false;
    emit Terminated ();
  }
  qDebug ("Konec smycky");
}
void WatchLoop::lock (bool state) {
  if (state) mutex->lock();
  else       mutex->unlock();
}
void WatchLoop::Start (bool state) {
  loop = state;
  if (state) {
    if (isRunning()) return;
    start ();
  }
  else {
    wait (1000);
  }
}
void WatchLoop::ForceStop (void) {
  loop = false;
  wait (1000);
  if (isRunning()) terminate ();
}
