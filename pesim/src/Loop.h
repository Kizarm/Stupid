#ifndef LOOP_H
#define LOOP_H

#include <QThread>

class Loop : public QThread {
  public:
    Loop (QObject * parent);
    ~Loop();
    
    void run (void);

  private:
    bool running;

};

#endif // LOOP_H
