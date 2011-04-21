#include "camera.h"
#include "../commands/commands.h"
#include "util.h"

/////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////

#define WRONG_CHUNK -1
#define WRONG_CHECKSUM -2
#define GOOD_CHUNK 0

int camera_checksum;

static char FORCE_RESET[6] = {(char)0xAA, (char)0x08, (char)0x00, (char)0x00, (char)0x00, (char)0xFF};
static char SYNC[6]        = {(char)0xAA, (char)0x0D, (char)0x00, (char)0x00, (char)0x00, (char)0x00};
static char SYNC_ACK[6]    = {(char)0xAA, (char)0x0E, (char)0x0D, (char)0x00, (char)0x00, (char)0x00};
static char CAM_INIT[6]    = {(char)0xAA, (char)0x01, (char)0x00, (char)0x07, (char)0x00, (char)0x03};
static char SET_SIZE[6]    = {(char)0xAA, (char)0x06, (char)0x08, (char)(CAMERA_CHUNK_LEN + 6), (char)0x00, (char)0x00};
static char TAKE_PIC[6]    = {(char)0xAA, (char)0x04, (char)0x05, (char)0x00, (char)0x00, (char)0x00};
static char GET_CHUNK[6]   = {(char)0xAA, (char)0x0E, (char)0x00, (char)0x00, (char)0x00, (char)0x00};
static char PIC_ACK[6]     = {(char)0xAA, (char)0x0E, (char)0x00, (char)0x00, (char)0xF0, (char)0xF0};

typedef enum CAMERA_RETURN_VALUE {
  CAMERA_RET_NONE,
  CAMERA_RET_ACK,
  CAMERA_RET_NACK,
  CAMERA_RET_SYNC,
  CAMERA_RET_SIZE,
  CAMERA_RET_DATA
} camera_return_t;

camera_return_t camera_result = CAMERA_RET_NONE;

typedef enum CAMERA_STATES {
  CAMERA_OFF,
  CAMERA_RESET,
  CAMERA_SYNC,
  CAMERA_SYNC_ACK,
  CAMERA_PREINIT,
  CAMERA_INIT,
  CAMERA_SIZE,
  CAMERA_TAKEPIC,
  CAMERA_TAKEPIC1,
  CAMERA_TAKEPIC2,
  CAMERA_COUNTCHUNKS,
  CAMERA_GETCHUNK,
  CAMERA_CHUNKWAIT,
  CAMERA_PICDONE
} camera_state_t;
camera_state_t camera_state = CAMERA_OFF;
char camera_state_change = 1;

typedef enum CAMERA_ACTIONS {
  CAMERA_NONE,
  CAMERA_WAIT,
  CAMERA_DONE
} camera_action_t;
camera_action_t camera_tx_action = CAMERA_NONE;
camera_action_t camera_rx_action = CAMERA_NONE;

// UART:
char* uart_tx_char_str;      // string to TX
// int uart_num_chars_left = 0; // current # of chars remaining to TX
char uart_num_recv_chars = 0; // current # of chars RX'ed

void camera_init() {
  UCA1CTL1 |= UCSWRST;
  P3SEL |= 0xC0;                             // P3.6,7 = USCI_A1 TXD/RXD
  BCSCTL2 &= 0xF7;                          // SMCLK = DC0 (16MHz)
  UCA1CTL1 |= UCSSEL_2;                     // BRCLK = SMCLK

  UCA1BR0 = 138;                           // baudrate = 115200
  UCA1BR1 = 0x00;                           //

  UCA1MCTL = UCBRS2 + UCBRS0;               // Modulation UCBRSx = 5
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UC1IE |= UCA1RXIE;                        // Enable USCI_A1 RX interrupt

  camera_image_num = 0;
  camera_num_chunks = 0;
  camera_current_chunk = 0;
  camera_result_len = 0;
}

int uart_send(char* str, int len) {
  if (UART_BUSY) return 1; // still transmitting

  uart_tx_char_str = str;
  uart_num_chars_left = len;
  UC1IE |= UCA1TXIE;                // Enable USCI_A1 RX/TX interrupt

  return 0;
}

void send_command(char* command){
  uart_num_recv_chars = 0;
  camera_result_len = 6;
  uart_send(command, 6);
}

void camera_setstate(camera_state_t fsmstate) {
  camera_state = fsmstate;
  camera_state_change = 1;
}

void camera_reset() {
  camera_setstate(CAMERA_OFF);
}

void camera_nextchunk() {
  if (++camera_current_chunk >= camera_num_chunks)
    camera_setstate(CAMERA_PICDONE);
  else
    camera_setstate(CAMERA_GETCHUNK);
}

#define TIMEOUT 51

void dostate(char* command, camera_return_t result, camera_state_t next_state, camera_state_t bad_state) {
  static int timeout_counter = 0;
  
  if (camera_state_change) {
    send_command(command);
    if (result != CAMERA_RET_NONE)
      camera_rx_action = CAMERA_WAIT;
    else {
      camera_rx_action = CAMERA_DONE;
      camera_result = result;
    }
    camera_tx_action = CAMERA_WAIT;
    camera_state_change = 0;
    timeout_counter = 0;
  }
  
  if (camera_tx_action != CAMERA_DONE)
    return;
  
  timeout_counter++;
  
  if (camera_rx_action == CAMERA_DONE) {
    if (camera_result == result)
      camera_setstate(next_state);
  }
  
  if (timeout_counter > TIMEOUT)
    camera_setstate(bad_state);
}

void camera_loop() { 
  int size;
  
  switch (camera_state) {
  case CAMERA_OFF:
    camera_setstate(CAMERA_RESET);
    break;
    
  case CAMERA_RESET:
    LEDS_SET(0x00);
    dostate(FORCE_RESET, CAMERA_RET_NONE, CAMERA_SYNC, CAMERA_RESET);
    break;

  case CAMERA_SYNC:
    dostate(SYNC, CAMERA_RET_SYNC, CAMERA_SYNC_ACK, CAMERA_SYNC);
    break;
    
  case CAMERA_SYNC_ACK:
    dostate(SYNC_ACK, CAMERA_RET_NONE, CAMERA_INIT, CAMERA_SYNC);    
    break;

  case CAMERA_INIT:
    dostate(CAM_INIT, CAMERA_RET_ACK, CAMERA_SIZE, CAMERA_INIT);
    break;

  case CAMERA_SIZE:
    dostate(SET_SIZE, CAMERA_RET_ACK, CAMERA_TAKEPIC, CAMERA_SIZE);
    break;
    
  case CAMERA_TAKEPIC:
    LEDS_XOR(0x11);
    dostate(TAKE_PIC, CAMERA_RET_SIZE, CAMERA_COUNTCHUNKS, CAMERA_TAKEPIC1);
    break;

  case CAMERA_TAKEPIC1:
    LEDS_XOR(0x22);
    dostate(TAKE_PIC, CAMERA_RET_SIZE, CAMERA_COUNTCHUNKS, CAMERA_TAKEPIC2);
    break;

  case CAMERA_TAKEPIC2:
    LEDS_XOR(0x44);
    dostate(TAKE_PIC, CAMERA_RET_SIZE, CAMERA_COUNTCHUNKS, CAMERA_RESET);
    break;

  case CAMERA_COUNTCHUNKS:
    LEDS_SET(0x00);
    camera_image_num++;
    size = (camera_input[4] << 8) + camera_input[3];
    camera_num_chunks = size/CAMERA_CHUNK_LEN;
    if (size%CAMERA_CHUNK_LEN > 0)
      camera_num_chunks++;
    camera_current_chunk = 0;
    camera_setstate(CAMERA_GETCHUNK);
    
  case CAMERA_GETCHUNK:
    if (camera_state_change)
      camera_chunkready = 0;
    
    GET_CHUNK[4] = (char)camera_current_chunk;
    dostate(GET_CHUNK, CAMERA_RET_DATA, CAMERA_CHUNKWAIT, CAMERA_RESET);
    break;

  case CAMERA_PICDONE:
    camera_chunkready = 0;
    dostate(PIC_ACK, CAMERA_RET_NONE, CAMERA_TAKEPIC, CAMERA_RESET);
  }  
}

//////////////////////////////////////// 
// Camera interrupt service routines
////////////////////////////////////////

void isr_camera_tx(void) {
  if (camera_tx_action == CAMERA_WAIT)
    camera_tx_action = CAMERA_DONE;
}

void isr_camera_rx(void) {
  if (camera_rx_action == CAMERA_WAIT)
    camera_rx_action = CAMERA_DONE;
}

void isr_camera_chunk(void) {
  if (camera_rx_action == CAMERA_WAIT)
    camera_rx_action = CAMERA_DONE;

  char chk = (camera_checksum - camera_input[camera_result_len - 2]) & 0xff;

  if (camera_input[0] != (char)camera_current_chunk) // wrong chunk
    camera_setstate(CAMERA_GETCHUNK);  
  else if (chk != camera_input[camera_result_len - 2]) // invalid checksum
    camera_setstate(CAMERA_GETCHUNK);
  else 
    camera_chunkready = 1;
}

//////////////////////////////////////// 
// UART interrupt service routines
////////////////////////////////////////

void isr_uart_tx(void) {
  if (uart_num_chars_left) {
    UCA1TXBUF = *(uart_tx_char_str++);
    uart_num_chars_left--;
  } else {
    IRQ_SET(IRQ_CAMERA_TX);
    UC1IE &= ~UCA1TXIE;
  }
}

void cam_set_result(camera_return_t result) {
  camera_result = result;
  camera_result_len = 6;
  uart_num_recv_chars = 0;
  IRQ_SET(IRQ_CAMERA_RX);
}

void isr_uart_rx(void) {
  if (uart_num_recv_chars == 0)
    camera_checksum = 0;
  
  camera_input[uart_num_recv_chars] = UCA1RXBUF;
  camera_checksum += camera_input[uart_num_recv_chars];
  uart_num_recv_chars++;
  
  if (uart_num_recv_chars == 6) {
    if (camera_input[0] == 0xaa) {
      switch (camera_input[1]) {
      case 0x0e:
        cam_set_result(CAMERA_RET_ACK);
        return;
      case 0x0f:
        cam_set_result(CAMERA_RET_NACK);
        return;
      case 0x0d:
        cam_set_result(CAMERA_RET_SYNC);
        return;
      case 0x0a:
        cam_set_result(CAMERA_RET_SIZE);
        return;
      }
    }
  
    camera_result_len = 6 + (camera_input[3] << 8) + camera_input[2];
  
  } else if (uart_num_recv_chars == camera_result_len) {
    uart_num_recv_chars = 0;
    camera_result = CAMERA_RET_DATA;
    IRQ_SET(IRQ_CAMERA_CHUNK);
  } else if (uart_num_recv_chars >= CAMERA_MAX_INPUT_LEN)
    uart_num_recv_chars = 0;
  
}