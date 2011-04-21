/*
 * Driver for the Leds of the GINA2.2b/c boards..
 *
 * Authors:
 * Ankur Mehta <mehtank@eecs.berkeley.edu>, August 2010
 * Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2010
 */

#ifndef __LEDS_H
#define __LEDS_H

#include "types.h"

//configuration
struct leds_config_t {
  reg_ptr pout;
  reg_ptr pdir;
  uint8_t mask;
};

//prototypes
void leds_configure(struct leds_config_t l);
void leds_init();

void leds_set(uint8_t leds_value);
void leds_circular_shift();
void leds_increment();

#endif
