#include "gina_config.h"
#include "radio.h"
// UCA0

int at_send(char* txstr, char* rxstr, int len) {
  if (AT_BUSY) return 1; // still transmitting

  at_tx_char_str = txstr+1;
  at_rx_char_str = rxstr;
  at_chars_left = len-1;
  AT_SPI_ON;
  UCA0TXBUF = *txstr;     // output first char
  return 0;
}

void high_speed_cfg()
{
  at_set_reg(0x0C,0x03);
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

int at_useclock(void) {
  volatile unsigned int i;
  int j = 0;

  at_set_reg(RG_TRX_CTRL_0, CLKM_16MHz);
  AT_WAIT;
  
  do {
    IFG1 &= ~OFIFG;                           // Clear OSCFault flag
    for (i = 0xFF; i > 0; i--);               // Time for flag to set
    j = j+1;
  } while ((IFG1 & OFIFG) && j <= 10);                   // OSCFault flag still set?

  if (j > 10)
    return 1;

  BCSCTL2 = SELM_3 | SELS;                          // MCLK= LFXT1, SMCLK = LFXT1 (safe)
  return 0;
}

void at_init() {
  // UCA0
  at_state = AT_STATE_OFF;
  at_current_channel = 0;
  at_chars_left = 0; // current # of chars remaining to TX
  
  at_sendbuffer[0] = 32;

  AT_SPI_OFF;
  AT_SLP_TR_CLR;
  AT_SPI_PDIR |= AT_SPI_PIN;                     // /SEL output
  AT_SLP_TR_PDIR |= AT_SLP_TR_PIN;               // SLP_TR output

  UCA0CTL1 = UCSSEL1 + UCSSEL0 + UCSWRST;       // SMCLK, reset
  P3SEL |= BIT0 | BIT4 | BIT5;                  // Enable UCA0 SPI
  
  P1IE |= AT_IRQ_PIN;
  
  UCA0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;   // polarity, MSB first, 3-pin SPI
  UCA0BR0 = 0x03;                           // UCLK/4
  UCA0BR1 = 0x00;                           // 0

  UCA0CTL1 &= ~UCSWRST;                     // Initialize USART state machine

  IE2 |= UCA0RXIE;         // Enable USCI_A0 RX/TX interrupt
}

void at_config(void) {
  /////// Initialize radio
  at_set_reg(RG_TRX_STATE, CMD_FORCE_TRX_OFF);
  at_set_reg(RG_IRQ_MASK, 0x08);          // only interrupt on TRX_END
  
#ifdef GINA_VERSION_1
  at_set_reg(RG_PHY_TX_PWR,0x80);         // AutoCRC on, 3dBm output power
#endif
  // AT_SET_REG(RG_XAH_CTRL,0x18);           // 
  
  AT_UNTIL_STATUS(TRX_OFF);  
  AT_CLR_IRQ;                             // Clear pending interrupts
  
  //at_useclock();
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
  
  at_state = AT_STATE_RX_READY;
  return chan;
}

void at_read (char** bytes_handle, char** len_handle) { //, unsigned char** lqi) {
  AT_CLR_IRQ;                             // Clear pending interrupts
  AT_WAIT;
  at_state = AT_STATE_RXING_CMD;
  while(at_send(at_sendbuffer, at_recvbuffer, sizeof(at_sendbuffer)));
  *len_handle = &at_recvbuffer[1];
  *bytes_handle = &at_recvbuffer[2];
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

void isr_at_rx(void) {
  register char rxc;
  rxc = UCA0RXBUF;
  *(at_rx_char_str++) = rxc;
  if (at_chars_left) {
    UCA0TXBUF = *(at_tx_char_str++);
    at_chars_left--;
  } else {
    AT_SPI_OFF;
    if (at_state == AT_STATE_RXING_STR) {
      IRQ_SET(IRQ_AT_RX_DONE);
      at_state = AT_STATE_RXING_LQI;
    }
  }
  
  if (at_state == AT_STATE_RXING_CMD)
    at_state = AT_STATE_RXING_LEN;
  else if (at_state == AT_STATE_RXING_LEN) {
    at_state = AT_STATE_RXING_STR;
    if (rxc < at_chars_left)
      at_chars_left = rxc;
  }
}
