#ifndef __FUELGAUGE_H
#define __FUELGAUGE_H

#include "msp430x26x.h" 
#include "gina_config.h"
#include "i2c.h"

void fg_init(void);
int fg_test(void);
void fg_config(void);
int fg_read(char);

// #define IR_PREP while(i2c_send(ir_str6, 1))
// #define IR_READ(var) while(i2c_recv(var, 13))

#define isr_fg_rx() isr_i2c_rx()

#define FG_REG_CNTL       0x00
#  define FG_CNTL_STATUS    {0x00, 0x00}
#  define FG_CNTL_TYPE      {0x00, 0x01}
#  define FG_CNTL_FWVER     {0x00, 0x02}
#  define FG_CNTL_SWVER     {0x00, 0x03}
#  define FG_CNTL_CHKSUM    {0x00, 0x04}
#  define FG_CNTL_RSTDATA   {0x00, 0x05}
#  define FG_CNTL_RSVD      {0x00, 0x06}

#define FG_REG_AR         0x02
#define FG_REG_ARTTE      0x04
#define FG_REG_TEMP       0x06
#define FG_REG_VOLT       0x08
#define FG_REG_FLAGS      0x0a
#define FG_REG_NAC        0x0c
#define FG_REG_FAC        0x0e
#define FG_REG_RM         0x10
#define FG_REG_FCC        0x12
#define FG_REG_AI         0x14
#define FG_REG_TTE        0x16
#define FG_REG_TTF        0x18
#define FG_REG_SI         0x1a
#define FG_REG_STTE       0x1c
#define FG_REG_MLI        0x1e
#define FG_REG_MLTTE      0x20
#define FG_REG_AE         0x22
#define FG_REG_AP         0x24
#define FG_REG_TTECP      0x26
#define FG_REG_RSVD       0x28
#define FG_REG_CC         0x2a
#define FG_REG_SOC        0x2c

#endif
