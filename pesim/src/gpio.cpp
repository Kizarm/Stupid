#include "DigitalIO.h"
#include "gpio.h"

GPIO::GPIO (IOPort * const p, const unsigned int no, const IO_Dir dir, uint16_t * d)
  : port (p), number (no), direction (dir), data(d) {
  unsigned addr = GIODescriptor.NetAddr;
  if (direction == IO_Input) {
    if (port != GIODescriptor.inp) {
      fprintf (stderr, "[%d]: !!! Bad port (direction -> IN  to Y%d)\n", addr, no);
    }
  } else {
    if (port != GIODescriptor.out) {
      fprintf (stderr, "[%d]: !!! Bad port (direction -> OUT to X%d)\n", addr, no);
    }
  }
  if (no >= MaxIO) {
    fprintf (stderr, "[%d]: !!! Position %d > maximum No. %d IO\n", addr, no, MaxIO-1);
    return;
  }
  if (p[no].used) {
    fprintf (stderr, "[%d]: !!! IO %d already used\n", addr, no);
    return;
  }
  p[no].used = true;
  GSystemCheck.append(this);
}

GPIO::~GPIO() {
}
void GPIO::SetBistable (bool b) {
  port[number].io->SetBistable (b);
}
void GPIO::pass ( void ) {
    uint16_t n = (1<<number);
  if (direction == IO_Input) {
    bool b = port[number].io->GetState();
    if (b) * data |=  n;
    else   * data &= ~n;
  } else {
    uint16_t u = * data;
    if (u & n) port[number].io->SetState(true);
    else       port[number].io->SetState(false);
  }
}

