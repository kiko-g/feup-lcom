#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>

/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */


/* General macros */

#define KBD_IRQ 1

#define DELAY_US 20000
#define WAIT_KBC tickdelay(micros_to_ticks(DELAY_US))
#define MAX_TRIES 5


/* I/O port addresses */

#define KBD_STATUS_PORT   0x64
#define KBD_OUT_BUF       0x60
#define KBD_INPT_BUF_COM  0x64
#define KBD_INPT_BUF_ARG  0x60


/* Status register masks */

#define OUT_BUF_FULL	     BIT(0)
#define INPT_BUF_FULL	     BIT(1)
#define TRANSMIT_TIME_OUT  BIT(5)
#define RECEIVE_TIME_OUT   BIT(6)
#define PARITY_ERROR       BIT(7)


/* Scancodes */

#define MAKE_CODE       BIT(7)
#define INCOMPLETE_CODE 0xE0
#define ESC_MAKE_CODE   0x01
#define ESC_BREAK_CODE  0x81


/* KBC commands */

#define WRITE_CMD_BYTE  0x60
#define READ_CMD_BYTE   0x20

/* KBC arguments */

#define ENABLE_INTERRUPTS 1


#endif /* _LCOM_I8042_H */
