#ifndef _LCOM_SERIAL_PORT_H_
#define _LCOM_SERIAL_PORT_H_

#include <lcom/lcf.h>


/** @defgroup serialport serialport
 * @{
 *
 * File for programming with the serial port
 */


/**
 * @brief Subscribes to interrupts from the serial port
 * 
 * @param bit_no Used to correctly identify interrupts, by serving as a mask for irq_set
 * @return 0 if successful, 1 if otherwise
 */
int (serial_port_com1_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes from the interrupts from the serial port
 * 
 * @return 0 if successful, 1 if otherwise
 */
int (serial_port_com1_unsubscribe_int)();

/**
 * @brief Reads the data from the IIR's port
 * 
 * @param IIR_byte Pointer to a variable that will hold the value read
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_read_IIR(uint32_t* IIR_byte);

/**
 * @brief Reads the data from the LSR's port
 * 
 * @param LSR_byte Pointer to a variable that will hold the value read
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_read_LSR(uint32_t* LSR_byte);

/**
 * @brief Reads the data from the RBR's port
 * 
 * @param RBR_byte Pointer to a variable that will hold the value read
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_read_RBR(uint32_t* RBR_byte);

/**
 * @brief Reads the data from the IER's port
 * 
 * @param IER_byte Pointer to a variable that will hold the value read
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_read_IER(uint32_t* IER_byte);

/**
 * @brief Writes data to the IER's port
 * 
 * @param IER_byte Variable that will be written to the port
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_write_IER(uint32_t IER_byte);

/**
 * @brief Writes data to the FCR's port
 * 
 * @param FCR_byte Variable that will be written to the port
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_write_FCR(uint32_t FCR_byte);

/**
 * @brief Writes data to the THR's port
 * 
 * @param THR_byte Variable that will be written to the port
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_write_THR(uint32_t THR_byte);

/**
 * @brief Reads the data from the LCR's port
 * 
 * @param LCR_byte Pointer to a variable that will hold the value read
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_read_LCR(uint32_t* LCR_byte);

/**
 * @brief Writes data to the LCR's port
 * 
 * @param LCR_byte Variable that will be written to the port
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_write_LCR(uint32_t LCR_byte);

/**
 * @brief Gives access to the divisor latch
 * 
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_access_DL();

/**
 * @brief Removes access to the divisor latch
 * 
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_deaccess_DL();

/**
 * @brief Sets the bit rate
 * 
 * @param bit_rate Bit rate to be set
 * @return int 0 if successful, 1 if otherwise
 */ 
int serial_port_set_bit_rate(uint16_t bit_rate);

/**
 * @brief Writes data to the DL's ports
 * 
 * @param DLM_byte Most significant byte
 * @param DLL_byte Least significant byte
 * @return int 0 if successful, 1 if otherwise
 */
int serial_port_write_DL(uint8_t DLM_byte, uint8_t DLL_byte);

/**
 * @brief Flushes the serial port's data buffers
 * 
 */
void serial_port_flush();


/**
 * @brief Receives messages from the serial port
 * 
 * @param msg A pointer to an array of characters (string), to be filled with the message
 * @param length The current length of the string
 */
void (serial_port_receive_msg)(char** msg, uint16_t* length);

/**
 * @brief Send a character through the serial port
 * 
 * @param byte The character to be sent
 */
void (serial_port_send_char)(char byte);

/**
 * @brief Send a message through the serial port
 * 
 * @param byte The string to be sent
 */
void (serial_port_send_msg)(char* msg, unsigned int length);


/**
 * @} 
 */

#endif /* _LCOM_SERIAL_PORT_H */
