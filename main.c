#include "drivers/gina.h"
#include "commands/commands.h"
/*
uint32_t chips[16] = {0xD9C3522E, 0xED9C3522, 0x2ED9C352, 0x22ED9C35,
                      0x522ED9C3, 0x3522ED9C, 0xC3522ED9, 0x9C3522ED,
                      0x8C96077B, 0xB8C96077, 0x7B8C9607, 0x77B8C960,
                      0x077B8C96, 0x6077B8C9, 0x96077B8C, 0xC96077B8};
*/
unsigned int chips2[32] = {0xD9C3, 0x522E, 0xED9C, 0x3522, 0x2ED9, 0xC352, 0x22ED, 0x9C35,
                           0x522E, 0xD9C3, 0x3522, 0xED9C, 0xC352, 0x2ED9, 0x9C35, 0x22ED,
                           0x8C96, 0x077B, 0xB8C9, 0x6077, 0x7B8C, 0x9607, 0x77B8, 0xC960,
                           0x077B, 0x8C96, 0x6077, 0xB8C9, 0x9607, 0x7B8C, 0xC960, 0x77B8};

char printout[256];
int helitx_int[(AT_MAX_PACKET_LEN + 4)>>1];
int helitx2_int[(AT_MAX_PACKET_LEN + 4)>>1];
int helirx_int[(AT_MAX_PACKET_LEN + 4)>>1];
char *helitx;
char *helitx2;
char *helirx;
int helilen;

uint8_t *magdata, *xldata, *gyrodata, *adcdata;
int *ctrls;

static char *helicnt;
static unsigned int helicnt_int;
volatile char test = 'U';

void end_flip(unsigned int* mem, int len)
{
  int i=0;
  for(;i < len; i++)
    mem[i] = ((mem[i]&0xFF00)>>8) + ((mem[i]&0x00FF)<<8);
  return;
}

void measureonce() {
  gyro_get_measurement(gyrodata);
  large_range_accel_get_measurement(xldata);
  magnetometer_get_measurement(magdata);
  sensitive_accel_temperature_get_measurement(adcdata);

  *helicnt = (helicnt_int >> 8) & 0xFF;
  *(helicnt+1) = helicnt_int & 0xFF;
  helicnt_int++;
}

void helisend() {
  at_txmode(cmd_channel);  
  while(at_send(helitx, helirx, 8*((int)helitx[1]) )) cmd_wait();
  AT_WAIT cmd_wait();
  AT_TX_START;
}  

void setcontrols(char* buf) {
  int p11 = buf[0] + (buf[1] << 8);
  int p12 = buf[2] + (buf[3] << 8);
  int p41 = buf[4] + (buf[5] << 8);
  int p42 = buf[6] + (buf[7] << 8);
  int p43 = buf[8] + (buf[9] << 8);

  pwm_setmotor(1, p11);
  pwm_setmotor(2, p12);
  pwm_setservo(1, p41);
  pwm_setservo(2, p42);
  pwm_setservo(3, p43);
}
/*
void init() {
  // "....IiSxSyS1S3StMxMyMzLxLyLzGtGxGyGzCmCtCaCe"
  helitx = (char*)helitx_int;
  helirx = (char*)helirx_int;
  helilen = 6; // without commands
  
  helicnt = &helitx[4];

  adcdata = (uint8_t*)&helitx[6];
  magdata = (uint8_t*)&helitx[16];
  xldata = (uint8_t*)&helitx[22];
  gyrodata = (uint8_t*)&helitx[28];

  ctrls = (int*)&helitx[36];

  helitx[0] = AT_FRAME_TX;
  helitx[1] = helilen; 
  helitx[2] = 'A';
  helitx[3] = 'B';
  helitx[4] = 'C';
  helitx[5] = 'D';
  helitx[6] = 'E';
  helitx[7] = 'F';
  helitx[8] = 'G';
  helitx[9] = 'H';

  
  gina_display = ((char*)helicnt)+1;
  helicnt_int = 0;

  cmd_init();
  IRQ_ENABLE;
  cmd_mode = CMD_MODE_WAIT;  
  cmd_ledmode = CMD_LEDMODE_CNT;
  cmd_tickmode = CMD_TICK;
}
*/

void conv_low_to_high(unsigned int* dst,char* src,int len)
{
  int i=0;
  for(;i < len; i++)
  {
    dst[ (i<<2) ] = chips2[(src[i] & 0xF0) >>3];
    dst[ (i<<2) + 1] = chips2[(src[i] & 0xF0) >>3];
    dst[ (i<<2) + 2] = chips2[((src[i] & 0x0F)<<1)];
    dst[ (i<<2) + 3] = chips2[((src[i] & 0x0F)<<1) + 1];
  }
}

void init() {
  // "....IiSxSyS1S3StMxMyMzLxLyLzGtGxGyGzCmCtCaCe"
  helitx = (char*)helitx_int;
  helitx2 = (char*)helitx2_int;
  helirx = (char*)helirx_int;
  helilen = 6; // without commands
  
  helicnt = &helitx[4];

  adcdata = (uint8_t*)&helitx[6];
  magdata = (uint8_t*)&helitx[16];
  xldata = (uint8_t*)&helitx[22];
  gyrodata = (uint8_t*)&helitx[28];

  ctrls = (int*)&helitx[36];

  
  
  helitx[0] = AT_FRAME_TX;
  helitx[1] = 1;
  helitx[2] = (chips2[0]&0xFF00) >> 8;
  helitx[3] = (chips2[0]&0x00FF);
  //helitx[4] = (chips2[1]&0xFF00) >> 2;
  //helitx[5] = (chips2[1]&0xFF);
  helitx[4] = (chips2[1]&0xFF00) >> 8;
  helitx[5] = (chips2[1]&0x00FF);
  //end_flip((unsigned int*)&helitx[2],2);
  /*helitx2[2] = 'A';
  helitx2[3] = 'B';
  helitx2[4] = 'C';
  helitx2[5] = 'D';
  helitx2[6] = 'E';
  helitx2[7] = 'F';
  helitx2[8] = 'G';
  helitx2[9] = 'H';

  conv_low_to_high((unsigned int*)&helitx[2],&helitx2[2],4);
  */
  gina_display = ((char*)helicnt)+1;
  helicnt_int = 0;

  cmd_init();
  IRQ_ENABLE;
  cmd_mode = CMD_MODE_WAIT;  
  cmd_ledmode = CMD_LEDMODE_CNT;
  cmd_tickmode = CMD_TICK;
}


void start() {
  if (at_test()) { 
    // radio failed!
    at_state = AT_STATE_RX_READY;
    cmd_mode = CMD_MODE_ERROR;
    return;
  }
  
  at_config();
  cmd_channel = 20;
  at_rxmode(cmd_channel);
  
  //gyro_init();
  //large_range_accel_init();
 // magnetometer_init();
  sensitive_accel_temperature_init();
   
  cmd_mode = CMD_MODE_SPIN;
  high_speed_cfg();
  return;
}

int main( void ) {
//  WDTCTL = WDTPW + WDTHOLD; P2DIR = 0xFF; spinleds(200,50);
  
  gina_init();
  init();
  start();
   
  helisend();
  //usb_send((char*)&test,1);
  memset(printout,0,256);
  while(1) {
    LEDS_ROT;
    
    cmd_loop();

    if (cmd_mode == CMD_MODE_IMU_LOOP) {
      measureonce();
      helisend();
    }
    
    if ((at_state != AT_STATE_RX_READY &&
         at_state != AT_STATE_RX_WAITING)) { 
      at_rxmode(0);
      AT_CLR_IRQ;
    }

    if (cmd_tickmode == CMD_TICK) {
      PWM_WAIT
        if (at_state == AT_STATE_RX_WAITING)
          at_read(&bytes, &len);
    } else
        if (at_state == AT_STATE_RX_WAITING)
          at_read(&bytes, &len);      
  }
  return 0;
}

#pragma vector=IRQ_VECTOR
__interrupt void gina_ISR (void) {
  if (IRQ_GET(IRQ_AT_RX_DONE)) {           //  Radio pkt rec'd
    char *buf = bytes;
    int i = 0;
    char cmd = cmd_parsepacket(&buf, *len, &at_state, AT_STATE_RX_READY);
    if (cmd == CMD_SETCONTROLS)
      setcontrols(buf);
    IRQ_CLR(IRQ_AT_RX_DONE);
    
    printout[0]=*len + '0';
    memcpy(&printout[2],buf);
    printout[*len]='\n';
    //buf[*len+1]='\0';
    usb_send(&printout[0],*len+1+2);

  } else if (IRQ_GET(IRQ_AT_RX_WAIT)) {           //  Radio pkt waiting
    // at_read(&bytes, &len);
    IRQ_CLR(IRQ_AT_RX_WAIT);

  } else if (IRQ_GET(IRQ_USB_RX)) {           //  USB pkt rec'd
    char *buf = usb_buffer;
    cmd_parsepacket(&buf, 0, &usb_state, USB_STATE_HEADER);
    IRQ_CLR(IRQ_USB_RX);

  } else
    IRQ_CLR(0xff);
}
