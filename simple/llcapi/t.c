//#include <stdio.h>

static volatile int sp = 1;

unsigned int pole [16];

void test (void) {
  sp = pole[1];
  volatile float f = sp;
  sp = f;
}

int main (void) {
  test();
  return 0;
}
