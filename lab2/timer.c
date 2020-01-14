#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>

#include "i8254.h"

//GLOBAL VARIABLES
uint32_t global_interrupt_counter = 0; 
int hook_id = TIMER0_IRQ;

int (timer_set_frequency)(uint8_t timer, uint32_t freq)
{
    if(freq > TIMER_MAX_FREQ) { 
      //div would be equal to 0, due to it being stored in an integer
      printf("Frequency is too high\n");
      return 1;
    }

    if(freq < TIMER_MIN_FREQ) { 
      //div would be bigger than the maximum capacity for an uint_16 (overflow)
      printf("Frequency is too low\n");
      return 1;
    }

    uint8_t st;
    if(timer_get_conf(timer, &st)) {
      printf("Unable to retrieve the timer's configuration\n");
      return 1;
    }

    uint8_t control_word = TIMER_SEL(timer) | TIMER_LSB_MSB | (st & TIMER_PRESERVE_MODE);
    uint16_t div = TIMER_FREQ / freq;

    if(sys_outb(TIMER_CTRL, control_word)) {
      printf("Unable to send the control word\n");
      return 1;
    }

    uint8_t div_lsb;
    uint8_t div_msb;

    util_get_LSB(div, &div_lsb);
    util_get_MSB(div, &div_msb);

    if(sys_outb(TIMER_PORT(timer), div_lsb)) {
      printf("Unable to send lsb\n");
      return 1;
    }

    if(sys_outb(TIMER_PORT(timer), div_msb)) {
      printf("Unable to send msb\n");
      return 1;
    }

    return 0;
}



int (timer_subscribe_int)(uint8_t *bit_no)
{
    *bit_no = BIT(hook_id); //sets the bitmask for irq_set

    //function returns OK on success
    if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id) != OK) 
        return 1;

    else return 0;
}



int (timer_unsubscribe_int)()
{
    //function returns OK on success 
    if(sys_irqrmpolicy(&hook_id) != OK) return 1;
    else return 0;
}



void (timer_int_handler)()
{
    global_interrupt_counter++;
    return;
}


int (timer_get_conf)(uint8_t timer, uint8_t *st) 
{
  uint8_t control_word = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer); 

  if(sys_outb(TIMER_CTRL, control_word)) {
    printf("Unable to send the read-back command");
    return 1;
  }

  uint8_t timer_port = TIMER_PORT(timer);
  if(util_sys_inb(timer_port, st)) {
    printf("Unable to read the configuration from the timer's port");
    return 1;
  }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{
  union timer_status_field_val conf;

  switch(field)
  {
    case tsf_all:
      conf.byte = st;
      break;
    
    case tsf_initial:
      conf.in_mode = ((st & TIMER_LSB_MSB) >> 4);
      break;

    case tsf_mode:
      conf.count_mode = ((st & TIMER_COUNT_MODE_MASK) >> 1);
     
      //don't care bits must be 0
      if(conf.count_mode > 5) {
        conf.count_mode = conf.count_mode ^ TIMER_INIT_DCB_MASK;
      }
      break;

    case tsf_base:
      conf.bcd = st & TIMER_BCD;
      break;

    default:
      printf("Invalid status field\n");
      return 1;
  }

  if(timer_print_config(timer, field, conf)) {
    printf("Unable to print configuration\n");
    return 1;
  }


  return 0;
}
