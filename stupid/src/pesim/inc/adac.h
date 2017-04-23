#ifndef ADAC_H
#define ADAC_H

#include <stdint.h>
#include "iogbl.h"

class ADCI : public LoopBase {
  public:
    ADCI  (const unsigned int no, uint16_t * p);
    ~ADCI ();
    void pass (void);
  private:
    ADPort * const port;
    const unsigned number;
    uint16_t * data;
};

class DACO : public LoopBase {
  public:
    DACO  (const unsigned int no, uint16_t * p);
    ~DACO ();
    void pass (void);
  private:
    DAPort * const port;
    const unsigned number;
    uint16_t   value;
    uint16_t * data;
};

#endif // ADAC_H
