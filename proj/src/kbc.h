#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include "i8042.h"
#include "utils.h"


/** @defgroup kbc kbc
 * @{
 *
 * File for dealing with the KBC (used in the keyboard and the mouse)
 */



/**
 * @brief Issues a command to the KBC
 * 
 * @param cmd The command byte to be issued
 * @return 0 if the command was issued successfuly, any other value if otherwise 
 */
int kbc_issue_command(uint8_t cmd);


/**
 * @brief Issues a command to the KBC with an argument
 * 
 * @param cmd The command byte to be issued
 * @param arg The argument to be issued
 * @return 0 if the command was issued successfuly, any other value if otherwise 
 */
int kbc_issue_command_with_arg(uint8_t cmd, uint8_t arg);


/**
 * @brief Reads the return value of a KBC command
 * 
 * @param value Variable to store the return value of the command
 * @return 0 if the command return was read successfuly, any other value if otherwise
 */
int kbc_read_return_value(uint8_t *value);


/** 
 * @brief Read acknowledgment byte
 * @param acknowledgment_byte
 * @return 0 if acknowledgment_byte is ACK, any other value otherwise
 */
int kbc_read_acknowledgment(uint8_t *acknowledgment_byte);


/**
 * @} 
 */



#endif /* _LCOM_KBC_H */
