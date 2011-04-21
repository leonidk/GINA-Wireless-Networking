#ifndef __GINA_H
#define __GINA_H

#include "msp430x26x.h" 
#include "gina_config.h"

#include "util.h"

#include "radio.h"
#include "pwm.h"
#include "usb.h"
#include "i2c.h"

#include "gyro.h"
#include "magnetometer.h"
#include "large_range_accel.h"
#include "sensitive_accel_temperature.h"

#define SLEEPMSP _BIS_SR(LPM0_bits + GIE)           // Enter LPM0 w/ interrupt
#define WAKEMSP  __bic_SR_register_on_exit(CPUOFF)  // Exit LPM0 after ISR

#define WATCHDOG_OFF WDTCTL = WDTPW + WDTHOLD
// use ACLK/64 ~ 5.333 ms
#define WATCHDOG_RESET WDTCTL = WDTPW + WDTCNTCL + WDTSSEL + WDTIS1 + WDTIS0

char* bytes;
char* len;
volatile int gina_button_value; 
volatile static char *gina_display;

void gina_init(void);

#endif
