#include <stdio.h>

static volatile int sp = 1;

unsigned int pole [16];

void test (void) {
  sp = pole[1];
  volatile float f = sp;
  sp = f;
}
void disp (const char * name) {
  puts (name);
}

int main (void) {
  test();
  disp("pokus");
  return 0;
}
