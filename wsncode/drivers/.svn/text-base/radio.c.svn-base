#include "radio.h"
#include "interrupt.h"

// UCA0

// states
#define AT_STATE_OFF 0x00
#define AT_STATE_TX_READY 0x01
#define AT_STATE_TXING 0x02
#define AT_STATE_RX_READY 0x03
#define AT_STATE_RX_WAITING 0x10
#define AT_STATE_RXING 0x11

// fields
struct radio_config_t radio_config;
volatile char at_state;
volatile int at_current_channel;

char at_tx_buf[2];    // transmit buffer for register control
char at_rx_buf[2];    // receive buffer for register control

// aliases
#define AT_SLP_TR_POUT *radio_config.slptr_pout
#define AT_SLP_TR_PDIR *radio_config.slptr_pdir
#define AT_SLP_TR_PIN radio_config.slptr_pin

#define AT_SLP_TR_CLR AT_SLP_TR_POUT &= ~AT_SLP_TR_PIN
#define AT_SLP_TR_SET AT_SLP_TR_POUT |= AT_SLP_TR_PIN
#define AT_TX_START { AT_SLP_TR_SET; AT_SLP_TR_CLR; }

#define AT_CLR_IRQ at_get_reg(RG_IRQ_STATUS);          // read interrupt

#define AT_READ_REG(reg) (0x80 | reg)
#define AT_WRITE_REG(reg) (0xC0 | reg)

#define AT_FRAME_TX 0x60
#define AT_FRAME_RX 0x20

#define AT_REG_VAL at_rx_buf[1]
#define AT_WAIT while(radio_busy())
#define AT_UNTIL_STATUS(status)  while((at_get_reg(RG_TRX_STATUS) & 0x1F) != status)

bool radio_busy() {
  return spi0_busy();
}

void radio_configure(struct radio_config_t r) {
  // UCA0
  radio_config = r;
  at_state = AT_STATE_OFF;
  at_current_channel = 0;
}

void radio_init() {  
  *radio_config.enrf_pout &= ~radio_config.enrf_pin; 
  *radio_config.enrf_pdir |= radio_config.enrf_pin;                    // /SEL output

  AT_SLP_TR_CLR;
  AT_SLP_TR_PDIR |= AT_SLP_TR_PIN;               // SLP_TR output
  
  spi0_init();
}

void at_set_reg(char reg, char val) {
  at_tx_buf[0] = AT_WRITE_REG(reg);
  while(at_send(at_tx_buf, at_rx_buf, 2));
  at_tx_buf[1] = val;
}

char at_get_reg(char reg) {
  at_tx_buf[0] = AT_READ_REG(reg);
  while(at_send(at_tx_buf, at_rx_buf, 2));
  at_tx_buf[1] = 0;
  AT_WAIT;
  return AT_REG_VAL;
}

bool radio_canrx() {
  return (at_state != AT_STATE_RX_READY &&
          at_state != AT_STATE_RX_WAITING);
}
    
bool radio_waiting() {
  return (at_state == AT_STATE_RX_WAITING);
}

void at_config(void) {
  /////// Initialize radio
  at_set_reg(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
  at_set_reg(RG_IRQ_MASK, 0x08);          // only interrupt on TRX_END
  
  AT_UNTIL_STATUS(TRX_OFF);  
  AT_CLR_IRQ;                             // Clear pending interrupts
}

int at_test(void) {
  char atreg = at_get_reg(RG_MAN_ID_0);
  if (atreg != 0x1F)
    return 1; // failure to communicate
  return 0; // success
}

char at_txmode(char chan) {
  if (chan) {
    at_current_channel = chan;
    if (chan < 11) chan = 11;
    if (chan > 26) chan = 26;
    at_set_reg(RG_PHY_CC_CCA,0x20 + chan);         // Set channel
  }
  
  at_set_reg(RG_TRX_STATE, CMD_PLL_ON);
  AT_UNTIL_STATUS(PLL_ON);        // TODO: interrupt instead of busy wait
  
  at_state = AT_STATE_TX_READY;
  return chan;
}

char at_rxmode(char chan) {
  if (chan) {
    at_current_channel = chan;
    if (chan < 11) chan = 11;
    if (chan > 26) chan = 26;
    at_set_reg(RG_PHY_CC_CCA,0x20 + chan);         // Set channel
  }

  at_set_reg(RG_TRX_STATE, CMD_RX_ON);
  AT_UNTIL_STATUS(RX_ON);
  
  AT_CLR_IRQ;

  at_state = AT_STATE_RX_READY;
  return chan;
}


int at_send(char* txstr, char* rxstr, int len) {
  return spi0_send(txstr, rxstr, len, radio_config.enrf_pout, radio_config.enrf_pin);
}

void at_read (char** bytes_handle, char** len_handle) {
  AT_CLR_IRQ;                             // Clear pending interrupts
  AT_WAIT;
  at_state = AT_STATE_RXING;
  while (spi0_read(bytes_handle, len_handle, radio_config.enrf_pout, radio_config.enrf_pin));
}

void isr_at_irq(void) {
  // AT_GET_REG(RG_IRQ_STATUS);          // read interrupt
  switch (at_state) {
  case AT_STATE_TXING:
    at_state = AT_STATE_TX_READY;
    break;
  case AT_STATE_RX_READY:
    at_state = AT_STATE_RX_WAITING;
    IRQ_SET(IRQ_AT_RX_WAIT);
    break;
  }
}
