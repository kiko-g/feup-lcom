#ifndef _LCOM_UTILS_H_
#define _LCOM_UTILS_H_

#include <lcom/lcf.h>
#include <stdint.h>

/**
 * @brief Wrapper function that reads an 8 bit variable, instead of 16 bit that is used in the original sys_inb
 * 
 * @param port IO port from where the value will be read
 * @param value Variable that stores the value read
 * @return 0 if successful, any other value if otherwise
 */
int (util_sys_inb)(int port, uint8_t *value);


/**
 * @brief Helper function that extracts the least significant byte from a 2 byte variable 
 * 
 * @param val Variable from where the byte will be extracted
 * @param lsb Variable that stores the extracted value
 * @return 0 if successful, any other value if otherwise
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb);


/**
 * @brief Helper function that extracts the most significant byte from a 2 byte variable 
 * 
 * @param val Variable from where the byte will be extracted
 * @param lsb Variable that stores the extracted byte
 * @return 0 if successful, any other value if otherwise
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb);


#endif /* _LCOM_UTILS_H */
