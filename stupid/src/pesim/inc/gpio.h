#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <stdint.h>
//#include "bit.h"
#include "iogbl.h"

class GPIO : public LoopBase {

  public:
    GPIO (IOPort *const p, const unsigned int no, const IO_Dir dir, uint16_t * d);
    virtual ~GPIO();
    
    void SetBistable (bool b);
    void pass (void);
  private:
    IOPort * const port;
    const unsigned number;
    const IO_Dir   direction;
    uint16_t * data;
};
static IOPort * const IOPortX = GIODescriptor.inp;
static IOPort * const IOPortY = GIODescriptor.out;

#endif // GPIO_H
