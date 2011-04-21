#ifndef __UTIL_H
#define __UTIL_H

#include "msp430x26x.h" 

// #define LEDS_SET(val) {P2IE = 0x00; P2OUT = val; P2IE = 0xFF;}
// #define LEDS_XOR(val) {P2IE = 0x00; P2OUT ^= val; P2IE = 0xFF;}
// #define LEDS_ROT {P2IE = 0x00; P2OUT <<= 1; if (!P2OUT) P2OUT = 1; P2IE = 0xFF;} 
#define LEDS_GET P2OUT
#define LEDS_SET(val) P2OUT = val
#define LEDS_XOR(val) P2OUT ^= val
#define LEDS_ROT {P2OUT <<= 1; if (!P2OUT) P2OUT = 1;} 

void spinleds (int imax, int jmax);
void spinonce (int imax, int jmax, volatile char* var, char val);
void erroronce (int imax, int jmax, volatile char* var, char val);
char strcmp(const char* str1, const char* str2, int len);
int pid(int e, int* ei, int* ed, int kp, int ki, int kd);

#define NYBPRINT(val)  (val) + ( ((val) > 9) ? ('A' - 10) : ('0') )

#define BYTEPRINT(str, ind, val) { \
  str[ind] = NYBPRINT((val) >> 4); \
  str[ind+1] = NYBPRINT((val) & 0x0F); \
};

#define WORDPRINT(str, ind, val) { \
  BYTEPRINT(str, ind, (val) >> 8); \
  BYTEPRINT(str, ind+2, (val) & 0xFF); \
};

#endif