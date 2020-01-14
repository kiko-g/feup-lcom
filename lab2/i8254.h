#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_
#include <lcom/lcf.h>
/** @defgroup i8254
 * Constants for programming the i8254 Timer. Needs to be completed */

#define TIMER_FREQ 1193182        //clock frequency for timer in PC and AT
#define TIMER0_IRQ 0              //Timer 0 IRQ line
#define TIMER_MAX_FREQ 1193182    //maximum clock frequency accepted
#define TIMER_MIN_FREQ 19         //minimum clock frequency accepted


/* I/O port addresses */
#define TIMER_0        0x40       //Timer 0 count register
#define TIMER_1        0x41       //Timer 1 count register
#define TIMER_2        0x42       //Timer 2 count register
#define TIMER_CTRL     0x43       //Control register
#define TIMER_PORT(n) (0x40+n)    //General Timer count register
#define SPEAKER_CTRL   0x61       //Register for speaker control


/* Timer selection: bits 7 and 6 */
#define TIMER_SEL0   0x00               //Control Word for Timer 0
#define TIMER_SEL1   BIT(6)             //Control Word for Timer 1
#define TIMER_SEL2   BIT(7)             //Control Word for Timer 2
#define TIMER_SEL(n) ((0x00 + n) << 6)  //Generic Timer Control Word
#define TIMER_RB_CMD (BIT(7) | BIT(6))  //Read Back Command


/* Register selection: bits 5 and 4 */
#define TIMER_LSB     BIT(4)                  //Initialize Counter LSB only */
#define TIMER_MSB     BIT(5)                  //Initialize Counter MSB only */
#define TIMER_LSB_MSB (TIMER_LSB | TIMER_MSB) //Initialize LSB first and MSB afterwards */


/* Operating mode: bits 3, 2 and 1 */
#define TIMER_SQR_WAVE (BIT(2) | BIT(1)) //Mode 3: square wave generator */
#define TIMER_RATE_GEN BIT(2)            //Mode 2: rate generator */


/* Counting mode: bit 0 */
#define TIMER_BIN 0x00 //Count in binary */
#define TIMER_BCD 0x01 //Count in BCD */


/* READ-BACK COMMAND FORMAT */
#define TIMER_RB_COUNT_  BIT(5)
#define TIMER_RB_STATUS_ BIT(4)
#define TIMER_RB_SEL(n)  BIT((n) + 1)


/* READ-BACK COMMAND MASKS */
#define TIMER_COUNT_MODE_MASK (BIT(3) | BIT(2) | BIT(1)) //Mask for count mode */
#define TIMER_INIT_DCB_MASK   BIT(2)                     //Mask for "don't care bits" in inital mode


/* FREQUENCY SETTING MASKS */
#define TIMER_PRESERVE_MODE (BIT(3) | BIT(2) | BIT(1) | BIT(0)) 
//Mask for preserving the timer's first 4 bits in the configuration


#endif /* _LCOM_I8254_H */
