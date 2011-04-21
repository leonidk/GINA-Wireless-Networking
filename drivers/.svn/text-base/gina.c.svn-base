#include "gina.h"

void gina_init(void) {
  // Stop watchdog timer to prevent time out reset
  WATCHDOG_OFF;

  if (CALBC1_16MHZ ==0xFF || CALDCO_16MHZ == 0xFF) {  
    while(1);                               // If calibration constants erased
  }                                         // do not load, trap CPU!!

  BCSCTL1 = CALBC1_16MHZ;                   // Set DCO to 16MHz
  DCOCTL = CALDCO_16MHZ;                    // Set DCO to 16MHz
  BCSCTL3 = LFXT1S_2;                       // Set ACLK = VLOCLK (12 kHz)

  P1DIR = 0;
  P1IE = GINA_IRQ_PIN;                     // Enable pushbutton interrupt
  P1IES = GINA_IRQ_PIN;                    // on Hi-Lo edge
  P1IFG = 0;

  P2DIR = 0xFF;                             // Set P2.0 to output direction
  P2OUT = 0;

  _BIS_SR(GIE);                             // enable interrupts

  at_init();
  pwm_init();
  usb_init();
  
  gina_button_value = 0;
  *gina_display = 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR (void) {
  if (P1IFG & AT_IRQ_PIN) {           //  Radio interrupt
    isr_at_irq();
    P1IFG &= ~AT_IRQ_PIN;
  } else if (P1IFG & XL_IRQ_PIN) {    //  XL Interrupt
    // isr_xl_irq();
    P1IFG &= ~XL_IRQ_PIN;
  } else if (P1IFG & GINA_IRQ_PIN) {    //  Switch on GINA board
    // mode = 0;
    if (!pwm_button_debounce) {
      pwm_button_debounce = 100;
      gina_button_value += 1;
    }
    P1IFG &= ~GINA_IRQ_PIN;
  } else
    P1IFG = 0;
}

// UCA0 = radio ; UCB0 = ir camera
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIAB0RX_ISR(void) {
  if (IFG2 & UCA0RXIFG)
    isr_at_rx();
  else
    P2OUT = 0xff;
}

// USCI_B0 Data ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void) {
  // isr_ir_rx();
}

// UCA1 = usb ; UCB1 = xl
#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCIAB1RX_ISR(void) {
  if ( ((UC1IFG & UCB1RXIFG) && (UC1IE & UCB1RXIE)) ||
        (UCB1STAT & UCNACKIFG) ) {
    i2c_rxInterrupt();                         // implemented in I2C driver
  }
  if ( (UC1IFG & UCA1RXIFG) && (UC1IE & UCA1RXIE) ){
    isr_usb_rx();
  }
}

#pragma vector=USCIAB1TX_VECTOR
__interrupt void USCIAB1TX_ISR(void) {
  if ( ((UC1IFG & UCB1TXIFG) && (UC1IE & UCB1TXIE)) ||
       ((UC1IFG & UCB1RXIFG) && (UC1IE & UCB1RXIE)) ) {
    i2c_txInterrupt();                         // implemented in I2C driver
  }
  if ( (UC1IFG & UCA1TXIFG) && (UC1IE & UCA1TXIE) ){
    isr_usb_tx();
  }
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR (void) {
   ADC12IFG &= ~0x1F;                            // clear interrupt flags
   __bic_SR_register_on_exit(CPUOFF);            // restart CPU
}