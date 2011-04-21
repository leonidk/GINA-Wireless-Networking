/*
 * GINA LED driver
 *
 * Authors:
 * Ankur Mehta <mehtank@eecs.berkeley.edu>, November 2010
 */

#include "leds.h"

//fields
struct leds_config_t leds_config;

uint8_t leds_lsb;
uint8_t leds_msb;

//aliases
#define LEDS_POUT (*leds_config.pout)
#define LEDS_PDIR (*leds_config.pdir)
#define LEDS_MASK (leds_config.mask)

void leds_configure(struct leds_config_t l) {
  leds_config = l;
}

void leds_init() {  
  LEDS_PDIR |=  LEDS_MASK;                              // enable LED output
  LEDS_POUT &= ~LEDS_MASK;                              // all LEDs off

  leds_lsb = ((LEDS_MASK << 1) ^ LEDS_MASK) & LEDS_MASK;
  leds_msb = ((LEDS_MASK >> 1) ^ LEDS_MASK) & LEDS_MASK;
}

//shifts the LEDs by one position
void leds_circular_shift() {
  uint8_t temp_leds;

  temp_leds = LEDS_POUT & LEDS_MASK;                    // retrieve current status of LEDs
  temp_leds <<= 1;                                      // shift by one position
  temp_leds &= LEDS_MASK;                               //

  if (temp_leds == 0)                                   // if no LEDs on, switch on first one
    temp_leds = leds_lsb;

  leds_set(temp_leds);
}

//increments the binary number displayed on the LEDs
void leds_increment() {
  uint8_t leds_counter;
  leds_counter = ((LEDS_POUT & LEDS_MASK) + leds_lsb);
  leds_set(leds_counter);
}

void leds_set(uint8_t leds_value) {
  leds_value &= LEDS_MASK;                         // set LED bits
  leds_value |= LEDS_POUT & ~LEDS_MASK;            // restore other bits on LED port
  LEDS_POUT = leds_value;                           // set LED output  
}