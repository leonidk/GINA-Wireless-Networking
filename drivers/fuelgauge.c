#include "fuelgauge.h"
#define FG_ADDR 0x55

void fg_init(void) { 
  i2c_init(FG_ADDR);
}

int fg_test(void) {
  static char fg_volt[] = {FG_REG_VOLT};
  static char fg_current[] = {FG_REG_AI};
  static char fg_type[] = {0x00, 0x00, 0x02};
  static char fg_result[2];
  static int *fg_value;
  fg_value = (int*)fg_result;

  I2C_ADDR(FG_ADDR);                         // Set slave address
  while(i2c_send(fg_volt,1));
  //while(i2c_send(fg_type,3));
  
  I2C_WAIT {
    if (UCB0STAT & UCNACKIFG)
      return 1;
  };

  //while(i2c_send(fg_type,1));  
  while(i2c_recv(fg_result, 2));

  I2C_WAIT {
    if (UCB0STAT & UCNACKIFG)
      return 1;
  };
  P2OUT = (fg_result[1] << 4);

  return 0;
}

int fg_read(char value) {
  static char fg_volt[1];
  fg_volt[0] = value;

  static char fg_result[2];
  static int *fg_value;
  fg_value = (int*)fg_result;

  I2C_ADDR(FG_ADDR);                         // Set slave address
  while(i2c_send(fg_volt,1));
  while(i2c_recv(fg_result, 2));

  return *fg_value;
}

void fg_config(void) {
  char i2c_switch_config[] = {0x0f};

}
