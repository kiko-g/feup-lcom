#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>
#include "i8042.h"  
#include "utils.h"


/**
 * @brief Subscribes to interrupts from the keyboard controller (KBC)
 * 
 * @param bit_no Used to correctly identify interrupts, by serving as a mask for irq_set
 */
int (keyboard_subscribe_int)(uint8_t *bit_no);


/**
 * @brief Unsubscribes from the interrupts from the keyboard controller (KBC)
 */
int (keyboard_unsubscribe_int)();


/**
 * @brief The KBC's input handler. Retrieves a scancode when the interrupt is generated
 */
void (kbc_ih)();


/**
 * @brief Checks if a scancode is complete, given the fact that there are 2 byte scancodes, therefore needing two reads
 * 
 * @param scancode_array An array containing the scancode that is currently being read. If there is only one byte, position 0 is filled. If there are two, position 0 is filled with the escape code (0xE0), and position 1 is filled with 
 * @param index The position in the array to be filled: if there is only one byte, position 0 is filled. If there are two bytes, position 0 is filled with the escape code (0xE0), and position 1 is filled with the specific scancode
 * @return true if the scancode is complete, false otherwise
 */
bool (check_complete_scancode)(uint8_t scancode_array[], uint8_t *index);


/**
 * @brief Wrapper function to count the number of sys_inb_calls
 * 
 * @param port The IO port from where the value will be read
 * @param value Variable to hold the value read
 * @return 0 if the reading is successful, any other value if otherwise
 */
int sys_inb_with_count(int port, uint8_t *value);


/**
 * @brief Function that reads a scancode from the output buffer, checking if there are errors in the status of the keyboard
 */
void (get_scan_code)();


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
 * @brief Reenables keyboard interrupts after using polling
 *
 * @return 0 if successful, any other value if otherwise
 */
int reenable_keyboard_interrupts();


#endif /* _LCOM_KEYBOARD_H */
