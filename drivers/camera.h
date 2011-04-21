#ifndef __CAMERA_H
#define __CAMERA_H

#include "msp430x26x.h" 
#include "gina_config.h"

#define CAMERA_CHUNK_LEN 94

#define CAMERA_MAX_INPUT_LEN 255
char camera_input[CAMERA_MAX_INPUT_LEN];

int uart_num_chars_left;

int camera_image_num;
int camera_num_chunks;
int camera_current_chunk;
int camera_result_len;
int camera_chunkready;

#define UART_PUT(str) while(uart_send(str, sizeof(str)))
#define UART_BUSY uart_num_chars_left
#define UART_WAIT while(UART_BUSY)

char camera_tx[CAMERA_CHUNK_LEN + 4 + 2 + 3];
char camera_rx[CAMERA_CHUNK_LEN + 4 + 2 + 3];

void camera_init();
void camera_loop();
void camera_reset();
void camera_nextchunk();

void isr_uart_rx(void);
void isr_uart_tx(void);

void isr_camera_rx(void);
void isr_camera_tx(void);
void isr_camera_chunk(void);

#endif