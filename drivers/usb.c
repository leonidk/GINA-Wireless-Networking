#include "gina_config.h"
#include "usb.h"
// UCA1

int usb_send(char* str, int len) {
  //while(USB_BUSY);
  if (USB_BUSY) return 1; // still transmitting

  //UCA1TXBUF = *str;     // output first char
  //usb_tx_char_str = str + 1;
  //usb_num_chars_left = len - 1;

  usb_tx_char_str = str;
  usb_num_chars_left = len;
  UC1IE |= UCA1TXIE;                // Enable USCI_A1 RX/TX interrupt

  return 0;
}

void usb_init(void) {
  usb_num_chars_left = 0; // current # of chars remaining to TX

  // Setup UART A1 pins
  UCA1CTL1 = UCSSEL1 + UCSSEL0 + UCRXEIE + UCSWRST; // SMCLK, allow errors, reset
  
  P3SEL |= BIT6 | BIT7; // Enable UCA1 UART pins

  UCA1CTL0 = 0; // LSB first, no parity, 1 Stop bit
  // UCA1BR0 = 138; // 115.2 kbps
  UCA1BR0 = 17; // 921.6 kbps
  UCA1BR1 = 0x00; // 115.2 or 921.6 kbps
  // UCA1MCTL = 0x0E; // High speed mode; 115.2 kbps 
  UCA1MCTL = 0x06; // High speed mode;  921.6 kbps

  UCA1CTL1 &= ~UCSWRST; // Enable
  UC1IE |= UCA1RXIE;                // Enable USCI_A1 RX/TX interrupt
  
  usb_index = 0;
  usb_len = 0;
  usb_state = USB_STATE_HEADER;
  usb_numflags = 0;
  usb_packetsum = 0;
}

void isr_usb_rx(void) {
  register char usbrxc = UCA1RXBUF;
  
  if (usbrxc == USB_HEADER_FLAG)
    ++usb_numflags;
  else 
    usb_numflags = 0;
  
  if ((usb_numflags == USB_HEADER_LEN) && (usb_state != USB_STATE_VERIFIED)) {
    usb_state = USB_STATE_LEN;
    usb_packetsum = 0;
  } else if ((usb_state == USB_STATE_LEN) && (usbrxc <= USB_MAX_LEN)) {
    usb_state = USB_STATE_DATA;
    usb_index = 0;
    usb_len = usbrxc;
  } else if ((usb_state == USB_STATE_DATA) && (usb_index < usb_len)) {
    usb_buffer[usb_index++] = usbrxc;
    usb_packetsum += usbrxc;
  } else if ((usb_state == USB_STATE_DATA) && 
             (usb_index == usb_len) &&
             (usb_packetsum == (char)(~usbrxc))) {
    IRQ_SET(IRQ_USB_RX);
    usb_state = USB_STATE_VERIFIED;
  } else if (usb_state != USB_STATE_VERIFIED)
    usb_state = USB_STATE_HEADER; 
}

void isr_usb_tx(void) {
  if (usb_num_chars_left) {
    UCA1TXBUF = *(usb_tx_char_str++);
    usb_num_chars_left--;
  } else
    UC1IE &= ~UCA1TXIE;
    //UC1IFG &= ~UCA1TXIFG;
}
