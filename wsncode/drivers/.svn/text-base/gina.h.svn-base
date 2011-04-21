/*
 * General GINA board routines
 *
 * Authors:
 * Ankur Mehta <mehtank@eecs.berkeley.edu>, November 2010
 */

#ifndef __GINA_H
#define __GINA_H

#include "msp430x26x.h"
#include "types.h"
#include "interrupt.h"

// set 'general interrupt enable' bit
#define ENABLE_INTERRUPTS __bis_SR_register(GIE)

// Low power routines
#define SLEEPMSP _BIS_SR(LPM0_bits + GIE)           // Enter LPM0 w/ interrupt
#define WAKEMSP  __bic_SR_register_on_exit(CPUOFF)  // Exit LPM0 after ISR

// Watchdog timer uses ACLK/64 ~ 5.333 ms
#define WATCHDOG_RESET WDTCTL = WDTPW + WDTCNTCL + WDTSSEL + WDTIS1 + WDTIS0
#define WATCHDOG_OFF WDTCTL = WDTPW + WDTHOLD

// general GINA functions
void gina_startup();
void gina_init();

// board-specific functions
void gina_configure();

void gina_getsensorlist();

void gina_testsensor();
void gina_getsensor();
void gina_enablesensor();
void gina_disablesensor();

// application functions
void init();
void mainloop();

#endif
