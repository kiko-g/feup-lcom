#include <lcom/lcf.h>
#include "rtc.h"


bool rtc_is_updating() {
  unsigned number = 0;
  sys_outb(RTC_ADDR_REG, RTC_A);
  sys_inb(RTC_DATA_REG, &number);

  if(A_UIP & number) return false;
  else return true;
}


unsigned bcd_convert(unsigned bcd) {
    return (unsigned)(10*((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}


unsigned get_seconds() {
	unsigned number = 0;
	sys_outb(RTC_ADDR_REG, SEC);
	sys_inb(RTC_DATA_REG, &number);

	return bcd_convert(number);
}


unsigned get_minutes() {
	unsigned number = 0;
	sys_outb(RTC_ADDR_REG, MNT);
	sys_inb(RTC_DATA_REG, &number);

	return bcd_convert(number);
}


unsigned get_hours() {
	unsigned number = 0;
	sys_outb(RTC_ADDR_REG, HOUR);
	sys_inb(RTC_DATA_REG, &number);

	return bcd_convert(number);
}


unsigned get_day() {
	unsigned number = 0;
	sys_outb(RTC_ADDR_REG, DAY);
	sys_inb(RTC_DATA_REG, &number);

	return bcd_convert(number);
}


unsigned get_month() {
	unsigned number = 0;
	sys_outb(RTC_ADDR_REG, MONTH);
	sys_inb(RTC_DATA_REG, &number);

	return bcd_convert(number);
}


unsigned get_year() {
	unsigned number = 0;
	sys_outb(RTC_ADDR_REG, YEAR);
	sys_inb(RTC_DATA_REG, &number);

	return bcd_convert(number);
}



Date get_date() 
{
    Date date = init_date(); //just for comparison below
    
    do {
      if(!rtc_is_updating()) 
      {
          date.day = get_day();
          date.year = get_year();
          date.month = get_month();
          date.hours = get_hours();
          date.minutes = get_minutes();
          date.seconds = get_seconds();
      }
    } while((int)date.day == -1);

    return date;
}



Date init_date() 
{
    Date date;
    date.day = -1;
    date.month = 0;
    date.hours = 0;
    date.minutes = 0;
    date.seconds = 0;
    date.year = 0;

    return date;
}

void print_date(Date date) {
  printf("%02d:%02d:%02d | %02d/%02d/%d\n", date.hours, date.minutes, date.seconds, date.day, date.month, date.year);
}
