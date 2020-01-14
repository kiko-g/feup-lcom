#ifndef _LCOM_UART_PROTOCOL_H_
#define _LCOM_UART_PROTOCOL_H_

#include <lcom/lcf.h>
#include <stdbool.h>


/** @defgroup uart_protocol uart_protocol
 * @{
 *
 * File for uart protocol 
 */


#define BIT_RATE  19200

#define MAX_MSG_SIZE        2
#define BLOWN_UP_MSG_SIZE   1
#define LANDED_MSG_SIZE     1
#define CONNECT_MSG_SIZE    2

/**
 * @brief Configures the serial port
 * 
 * @return 0 if successful, 1 if otherwise
 */
int (serial_port_configure_com1)();

/**
 * @brief Checks if a complete message has been received through the serial port. Specific to Space Lander
 * 
 * @param msg A pointer to an array of characters (string), that contains the message received until the moment of the function call
 * @param length The current length of the string
 * @return true if it's been completely received, false if otherwise
 */
bool (serial_port_received_all)(char** msg, uint16_t* length);

/**
 * @} 
 */

#endif /* _LCOM_UART_PROTOCOL_H */
