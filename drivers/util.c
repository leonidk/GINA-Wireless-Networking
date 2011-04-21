#include "util.h"

void spinleds (int imax, int jmax) {
  char x = 0;
  while(1) 
    spinonce(imax, jmax, &x, 1);
}

void spinonce (int imax, int jmax, volatile char* breakvar, char breakval) {
  int bm1 = 0x0a;
  int bm2 = 0x11;
  int k;
  LEDS_SET(0x04);                          // Toggle P1.0 using exclusive-OR

  for (k = 0; k < 8; k++) {
    volatile int i, j;

    LEDS_ROT;
    bm1 <<= 1;
    bm1 = (bm1 & 0xFF) + ((bm1 & 0x100)>>8);
    bm2 <<= 1;
    bm2 = (bm2 & 0xFF) + ((bm2 & 0x100)>>8);

    i = imax;                              // Delay
    do {
      for (j = 0; j < jmax; j++)
        if (*breakvar == breakval)
          return;
      LEDS_XOR(bm1);
      i--;
      LEDS_XOR(bm2);
      LEDS_XOR(bm2);
      i--;
      LEDS_XOR(bm1);
    } while (i != 0);
  }
}


void erroronce (int imax, int jmax, volatile char* breakvar, char breakval) {
  volatile int i, j;
  LEDS_SET(0xaa);
  for (i=0; i<imax; i++) for (j=0; j<jmax; j++)
    if (*breakvar == breakval)
      return;
  LEDS_SET(0x55);
  for (i=0; i<imax; i++) for (j=0; j<jmax; j++)
    if (*breakvar == breakval)
      return;
}

char strcmp(const char* str1, const char* str2, int len){
  int i;
  for (i=0; i<len; i++)
    if (str1[i] != str2[i])
      return 1;
  return 0;
}

#define MAXINTERR 0x5000
int pid(int e, int* ei, int* ed, int kp, int ki, int kd) {
  int d, retval = 0;

  *ei = *ei + e;
  if (*ei < -MAXINTERR) *ei = -MAXINTERR;
  if (*ei > MAXINTERR) *ei = MAXINTERR;

  d = e - *ed;
  *ed = e;
  
  retval += (kp > 0) ? (e << kp) : (e >> -kp);
  retval += (ki > 0) ? (*ei << ki) : (e >> -ki);
  retval += (kd > 0) ? (d << kd) : (e >> -kd);

  return retval;
}