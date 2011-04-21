#ifndef __RADIO_H
#define __RADIO_H

#include "atmel.h"
#include "spi0.h"
// UCA0

struct radio_config_t {
  reg_ptr enrf_pout;
  reg_ptr enrf_pdir;
  uint8_t enrf_pin;

  reg_ptr slptr_pout;
  reg_ptr slptr_pdir;
  uint8_t slptr_pin;
};

bool radio_busy();
bool radio_canrx();
bool radio_waiting();

int at_send(char* txstr, char* rxstr, int len);
void at_set_reg(char reg, char val);
char at_get_reg(char reg);

void radio_configure(struct radio_config_t r);
void radio_init(void);

void at_config(void);
int at_test(void);

char at_txmode(char chan);
char at_rxmode(char chan);
void at_read (char** bytes_handle, char** len_handle);

void isr_at_irq(void);
void isr_at_rx(void);

#endif