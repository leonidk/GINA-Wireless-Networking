/*
 * Drivers for the large range accelerometer of the GINA2.2b/c board.
 *
 * Author:
 * Thomas Watteyne <watteyne@eecs.berkeley.edu>, August 2010
 */

#include "openwsn.h"
#include "large_range_accel.h"

//===================================== variables =============================

//register backup variables
uint8_t reg_CTRL_REGC;
uint8_t reg_CTRL_REGB;
bool large_range_accel_configured = FALSE;

//===================================== public ================================

void large_range_accel_init() {
   P5OUT |=  0x10;                               // set P5.4 as output
   P5DIR |=  0x10;                               // set P5.4 high to enable I2C
   i2c_write_register(LARGE_RANGE_ACCEL_I2C_ADDR,
         LARGE_RANGE_ACCEL_REG_CTRL_REGC_ADDR,
         LARGE_RANGE_ACCEL_REG_CTRL_REGC_SETTING);
   i2c_write_register(LARGE_RANGE_ACCEL_I2C_ADDR,
         LARGE_RANGE_ACCEL_REG_CTRL_REGB_ADDR,
         LARGE_RANGE_ACCEL_REG_CTRL_REGB_SETTING);
   large_range_accel_configured = TRUE;
}

void large_range_accel_disable() {
   i2c_write_register(LARGE_RANGE_ACCEL_I2C_ADDR,
                           LARGE_RANGE_ACCEL_REG_CTRL_REGB_ADDR,
                           LARGE_RANGE_ACCEL_REG_CTRL_REGB_SLEEP);
}

void large_range_accel_get_config() {
   if (large_range_accel_configured==TRUE) {
      i2c_read_registers(LARGE_RANGE_ACCEL_I2C_ADDR,
            LARGE_RANGE_ACCEL_REG_CTRL_REGC_ADDR,
            1,
            &reg_CTRL_REGC);
      i2c_read_registers(LARGE_RANGE_ACCEL_I2C_ADDR,
            LARGE_RANGE_ACCEL_REG_CTRL_REGB_ADDR,
            1,
            &reg_CTRL_REGB);
   }
}

void large_range_accel_get_measurement(uint8_t* spaceToWrite) {
   uint8_t i;
   if (large_range_accel_configured==TRUE) {
      i2c_read_registers(LARGE_RANGE_ACCEL_I2C_ADDR,
            LARGE_RANGE_ACCEL_REG_XOUT_H_ADDR,
            6,
            spaceToWrite);
   } else {
      for (i=0;i<6;i++) {
         spaceToWrite[i] = 0;
      }
   }
}
