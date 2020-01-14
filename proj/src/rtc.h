#ifndef _LCOM_RTC_H_
#define _LCOM_RTC_H_

#include <lcom/lcf.h>

/** @defgroup rtc rtc
 * @{
 *
 * File for programming the real time clock, holding macros, types and useful RTC related functions
 */


/** 
 * @brief type for all date related variables
 */
typedef struct {
    unsigned seconds;
    unsigned minutes;
    unsigned hours;
    unsigned day;
    unsigned month;
    unsigned year;
} Date;

// ------------ TIME MACROS -----------
#define SEC   0x00
#define MNT   0x02
#define HOUR  0x04
#define DAY   0x07
#define MONTH 0x08
#define YEAR  0x09

// ---------- CONTROL MACROS ----------
#define RTC_A 0x0A
#define RTC_B 0x0B
#define RTC_C 0x0C
#define RTC_D 0x0D

#define RTC_IRQ      0x08
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

// -------- REGISTER A MACROS ---------
#define A_UIP BIT(7)        // update in progress (register A)


/**
 * @brief Checks if RTC is updating
 * @return true if RTC is updating, false otherwise
 */
bool rtc_is_updating();


/**
 * @brief converts a BCD number into a BINARY number. If the number isn't in BCD, the number remains in BINARY
 * @return BINARY number
 */
unsigned bcd_convert(unsigned bcd);


/**
 * @brief Get RTC seconds
 * @return seconds 
 */
unsigned get_seconds();


/**
 * @brief Get RTC minutes
 * @return hours
 */
unsigned get_minutes();


/**
 * @brief Get RTC hours
 * @return hours
 */
unsigned get_hours();


/**
 * @brief Get RTC day of the month
 * @return day of month
 */
unsigned get_day();


/**
 * @brief Get RTC month
 * @return month
 */
unsigned get_month();


/**
 * @brief Get RTC year
 * @return year
 */
unsigned get_year();


/**
 * @brief Get whole date
 * @return Date object
 */
Date get_date();


/** 
 * @brief Initialize Date variable with values defined by the group
 * @return Date object
 */
Date init_date();

/** 
 * @brief Prints Date object (console)
 */
void print_date(Date date);



/**
 * @} 
 */

#endif /* _LCOM_RTC_H */


