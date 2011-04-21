/*
 * General GINA board routines
 *
 * Authors:
 * Ankur Mehta <mehtank@eecs.berkeley.edu>, November 2010
 */

#include "gina.h"
#include "commands.h"
#include "leds.h"
#include "radio.h"

#ifdef USB
#  include "usb.h"
#endif

// fields
volatile int irq_ifg = 0;

char* bytes;
char* len;

void gina_startup() {
  WATCHDOG_OFF;                                 // disable watchdog timer
  BCSCTL1 = CALBC1_16MHZ;                       // MCLK at 16MHz
  DCOCTL  = CALDCO_16MHZ;

  ENABLE_INTERRUPTS;
}

void gina_init() {
  leds_init();
  radio_init();
# ifdef USB
  usb_init();
# endif
}

void main() {
  gina_startup();
  gina_configure();
  gina_init();
  init();

  while(1) {
    cmd_loop();

    mainloop();
    
    if (radio_canrx())
      at_rxmode(0);

    if (cmd_tickmode == CMD_TICK) {
      // PWM_WAIT
        if (radio_waiting())
          at_read(&bytes, &len);
    } else
      if (radio_waiting())
        at_read(&bytes, &len);      
  }

}