#include <stdio.h>
#include "../pesim/inc/adac.h"
#include "AnalogIO.h"

ADCI::ADCI (const unsigned int no, uint16_t * p) :
  port(GIODescriptor.adc), number(no), data(p) {
  if (no >= MaxAD) {
    fprintf (stderr, "[%d]: !!! Position %d > maximum No. %d ADC\n", GIODescriptor.NetAddr, no, MaxIO-1);
    return;
  }
  if (port[no].used) {
    fprintf (stderr, "[%d]: !!! ADC %d already used\n", GIODescriptor.NetAddr, no);
    return;
  }
  port[no].used = true;
  GSystemCheck.append(this);
}
ADCI::~ADCI() {

}
void ADCI::pass ( void ) {
  * data = port[number].io->GetValue();
}

/**************************************************************************************************/
DACO::DACO (const unsigned int no, uint16_t * p) :
  port(GIODescriptor.dac), number(no), data(p) {    
  value = 0;
  if (no >= MaxAD) {
    fprintf (stderr, "[%d]: !!! Position %d > maximum No. %d DAC\n", GIODescriptor.NetAddr, no, MaxIO-1);
    return;
  }
  if (port[no].used) {
    fprintf (stderr, "[%d]: !!! DAC %d already used\n", GIODescriptor.NetAddr, no);
    return;
  }
  port[no].used = true;
  GSystemCheck.append(this);
}
DACO::~DACO() {
}
void DACO::pass ( void ) {
  uint16_t v = * data;
  if (value != v) {
    value = v;
    port[number].io->SetValue (value);
  }
}

