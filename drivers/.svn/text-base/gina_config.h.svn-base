#ifndef __GINA_CONFIG_H
#define __GINA_CONFIG_H

#define IRQ_VECTOR COMPARATORA_VECTOR
#define IRQ_ENABLE CACTL1 |= CAIE
#define IRQ_DISABLE CACTL1 &= ~CAIE
#define IRQ_IFG_SET CACTL1 |= CAIFG
#define IRQ_IFG_CLR CACTL1 &= ~CAIFG
volatile char irq_ifg;

#define IRQ_AT_RX_DONE BIT0
#define IRQ_AT_RX_WAIT BIT1
#define IRQ_USB_RX BIT2
#define IRQ_CAMERA_TX BIT3
#define IRQ_CAMERA_RX BIT4
#define IRQ_CAMERA_CHUNK BIT5

#define IRQ_SET(val) { irq_ifg |= val; IRQ_IFG_SET; }
#define IRQ_CLR(val) { irq_ifg &= ~val; if (!irq_ifg) IRQ_IFG_CLR; }
#define IRQ_GET(val) irq_ifg & val

#ifdef GINA_VERSION_1
// Pushbutton
#  define GINA_IRQ_PIN BIT5
// Accel
#  define XL_IRQ_PIN BIT7
// Radio
#  define AT_IRQ_PIN BIT4
#  define AT_SLP_TR_POUT P6OUT
#  define AT_SLP_TR_PDIR P6DIR
#  define AT_SLP_TR_PIN BIT7
#  define AT_SPI_POUT P3OUT
#  define AT_SPI_PDIR P3DIR
#  define AT_SPI_PIN BIT3
// Gyros
#  define ADC_ATMP INCH_0
#  define ADC_ITMP INCH_1
#  define ADC_ZOUT INCH_2
#  define ADC_XOUT INCH_3
#  define ADC_YOUT INCH_4
#  define ADC_XPA INCH_5
#  define ADC_YPA INCH_6
#  define ADC_P6SEL 0x7F
// IRCam
#  define IR_EN_POUT P5OUT
#  define IR_EN_PDIR P5DIR
#  define IR_EN_PIN BIT7
#  define IR_CLK_PSEL P5SEL
#  define IR_CLK_PDIR P5DIR
#  define IR_CLK_PIN BIT5

#elif defined(GINA_VERSION_2_0)
// Pushbutton
#  define GINA_IRQ_PIN BIT7
// Accel
#  define XL_IRQ_PIN BIT5
// Radio
#  define AT_IRQ_PIN BIT6
#  define AT_SLP_TR_POUT P4OUT
#  define AT_SLP_TR_PDIR P4DIR
#  define AT_SLP_TR_PIN BIT5
#  define AT_SPI_POUT P4OUT
#  define AT_SPI_PDIR P4DIR
#  define AT_SPI_PIN BIT6
// Gyros
#  define ADC_ATMP INCH_2
#  define ADC_ITMP INCH_1
#  define ADC_ZOUT INCH_7
#  define ADC_XOUT INCH_3
#  define ADC_YOUT INCH_4
#  define ADC_XPA INCH_5
#  define ADC_YPA INCH_6
#  define ADC_P6SEL 0xFE
// IRCam
#  define IR_EN_POUT P1OUT
#  define IR_EN_PDIR P1DIR
#  define IR_EN_PIN BIT0
#  define IR_CLK_PSEL P1SEL
#  define IR_CLK_PDIR P1DIR
#  define IR_CLK_PIN BIT4
#endif


#elif defined(GINA_VERSION_2_2C)
// Gyro
#  define GINA_IRQ_PIN BIT5
// Accel
#  define XL_IRQ_PIN BIT7
// Radio
#  define AT_IRQ_PIN BIT6
#  define AT_SLP_TR_POUT P4OUT
#  define AT_SLP_TR_PDIR P4DIR
#  define AT_SLP_TR_PIN BIT7
#  define AT_SPI_POUT P4OUT
#  define AT_SPI_PDIR P4DIR
#  define AT_SPI_PIN BIT0

#endif
