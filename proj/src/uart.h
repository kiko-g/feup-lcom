#ifndef _LCOM_UART_H_
#define _LCOM_UART_H_

/** @defgroup uart uart
 * @{
 *
 * File for serial port macros
 */

// IRQS
#define UART_COM1_IRQ   4
#define UART_COM2_IRQ   3

// I/O Ports's Base Addresses
#define COM1_BASE_ADDR  0x3F8
#define COM2_BASE_ADDR  0x2F8

// Registers
#define RBR   0   // Receiver Buffer
#define THR		0   // Transmitter Holding
#define IER	  1   // Interrupt Enable
#define	IIR		2   // Interupt Indentification
#define	FCR		2   // FIFO Control 
#define LCR		3   // Line Control 
#define MCR   4   // Modem Control 
#define LSR		5   // Line Status 
#define DLL		0   // Divisor Latch LSB
#define DLM		1   // Divisor Latch MSB

// Control Register's (LCR) Bits
#define WORLD_LENGTH_SEL    (BIT(1) | BIT(0))
#define NO_STOP_BITS        BIT(2)
#define PARITY              (BIT(5) | BIT(4) | BIT(3))
#define SET_BREAK_ENABLE    BIT(6)
#define DLAB                BIT(7)

#define WORD_LENGTH_5				0
#define WORD_LENGTH_6				BIT(0)
#define WORD_LENGTH_7			  BIT(1)
#define WORD_LENGTH_8			  (BIT(1) | BIT(0))

#define STOP_BITS_1					0
#define STOP_BITS_2				  BIT(2)

#define PARITY_NONE					0
#define PARITY_ODD					BIT(3)
#define PARITY_EVEN					(BIT(4) | BIT(3))
#define PARITY_1        		(BIT(5) | BIT(3))
#define PARITY_0			      (BIT(5) | BIT(4) | BIT(3))

// Line Status Register's (LSR) Bits
#define RECEIVER_READY      BIT(0)
#define OVERRUN_ERROR       BIT(1)
#define PARITY_ERROR        BIT(2)
#define FRAMING_ERROR       BIT(3)
#define BREAK_INTERRUPT     BIT(4)
#define THR_EMPTY           BIT(5)
#define TRANSMITTER_EMPTY   BIT(6)

// Interrupt Enable Register's (IER) Bits
#define ENABLE_RECEIVED_DATA_INTERRUPT          BIT(0)
#define ENABLE_TRANSMITTER_EMPTY_INTERRUPT      BIT(1)
#define ENABLE_RECEIVER_LINE_STATUS_INTERRUPT   BIT(2)
 
// Interrupt Identification Register's (IIR) Bits
#define INTERRUPT_STATUS                BIT(0)
#define INTERRUPT_STATUS_PENDING        0
#define INTERRUPT_STATUS_NOT_PENDING    BIT(0)

#define INTERRUPT_ORIGIN                   (BIT(3) | BIT(2) | BIT(1))
#define INTERRUPT_ORIGIN_MODEM_STATUS      0
#define INTERRUPT_ORIGIN_TRANS_EMPTY       BIT(1)
#define INTERRUPT_ORIGIN_CHAR_TIMEOUT      BIT(3)
#define INTERRUPT_ORIGIN_DATA_AVAILABLE    BIT(2)
#define INTERRUPT_ORIGIN_LINE_STATUS       (BIT(2) | BIT(1))

#define FIFO_64  BIT(5)

#define FIFO_STATUS           (BIT(7) | BIT(6))
#define FIFO_STATUS_NO_FIFO   0
#define FIFO_STATUS_UNUSABLE  BIT(7)
#define FIFO_STATUS_ENABLED   (BIT(7) | BIT(6))

// FIFO Control Register (FCR)'s Bits
#define ENABLE_FIFO					  BIT(0)
#define CLEAR_RECEIVE_FIFO		BIT(1)
#define CLEAR_TRANSMIT_FIFO		BIT(2)

#define FIFO_INTERRUPT_TRIGGER_LEVEL_1			0
#define FIFO_INTERRUPT_TRIGGER_LEVEL_4			BIT(6)
#define FIFO_INTERRUPT_TRIGGER_LEVEL_8			BIT(7)
#define FIFO_INTERRUPT_TRIGGER_LEVEL_14			(BIT(7) | BIT(6))

// Retries
#define UART_MAX_TRIES  5
#define WAIT_UART tickdelay(micros_to_ticks(10000))


/**
 * @} 
 */


#endif /* _LCOM_UART_H */
