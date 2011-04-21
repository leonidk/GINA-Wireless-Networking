#ifndef __USB_H
#define __USB_H

#include "msp430x26x.h" 

// UCA1

volatile int usb_num_chars_left; // current # of chars remaining to TX
volatile char* usb_tx_char_str;           // pointer to remaining chars to tx

#define USB_MAX_LEN 128
char usb_buffer[USB_MAX_LEN];
volatile char usb_index;
volatile char usb_len;

#define USB_STATE_HEADER 0
#define USB_STATE_LEN 1
#define USB_STATE_DATA 2
#define USB_STATE_CHECKSUM 3
#define USB_STATE_VERIFIED 4

volatile char usb_state;
volatile int usb_numflags;
volatile char usb_packetsum;

#define USB_HEADER_FLAG 0x80
#define USB_HEADER_LEN 3

#define USB_PUT(str) while(usb_send(str, sizeof(str)))
#define USB_BUSY usb_num_chars_left
#define USB_WAIT while(USB_BUSY)

int usb_send(char* str, int len);
void usb_init(void);

void isr_usb_rx(void);
void isr_usb_tx(void);

#endif