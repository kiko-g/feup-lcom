#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>

//Constants for programming the i8042 Keyboard.

/* General macros */
#define KBD_IRQ   1
#define MOUSE_IRQ 12
#define DELAY_US 20000
#define WAIT_KBC tickdelay(micros_to_ticks(DELAY_US))
#define DELAY(n) tickdelay(micros_to_ticks(n))
#define MAX_TRIES 5


/* I/O port addresses */
#define KBC_STATUS_PORT   0x64
#define KBC_OUT_BUF       0x60
#define KBC_INPT_BUF_COM  0x64
#define KBC_INPT_BUF_ARG  0x60


/* Status register masks */
#define OUT_BUF_FULL	      BIT(0)
#define INPT_BUF_FULL	      BIT(1)
#define TRANSMIT_TIME_OUT   BIT(5)
#define RECEIVE_TIME_OUT    BIT(6)
#define PARITY_ERROR        BIT(7)


/* PS2 mode */
#define AUXILIARY_DEVICE    BIT(5)
#define PS2_TIME_OUT        BIT(6)


/* Scancodes */
#define MAKE_CODE           BIT(7)
#define INCOMPLETE_CODE     0xE0
#define ESC_MAKE_CODE       0x01
#define ESC_BREAK_CODE      0x81


/* KBC commands */
#define WRITE_CMD_BYTE      0x60
#define READ_CMD_BYTE       0x20
#define WRITE_MOUSE         0xD4
#define ENABLE_DATA_REPORT  0xF4
#define DISABLE_DATA_REPORT 0xF5
#define STREAM_MODE         0xEA
#define READ_DATA           0xEB


/* Mouse acknowledgment bytes */
#define ACK     0xFA
#define NACK    0xFE
#define ERROR   0xFC


/* Packet struct */
#define LB      BIT(0)
#define RB      BIT(1)
#define MB      BIT(2)
#define X_SIGN  BIT(4)
#define Y_SIGN  BIT(5)
#define X_OVF   BIT(6)
#define Y_OVF   BIT(7)


/* Mouse packet */
#define FIRST_PACKET  BIT(3)


/* KBC arguments */
#define ENABLE_INTERRUPTS 1


#endif /* _LCOM_I8042_H */
