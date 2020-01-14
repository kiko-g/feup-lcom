#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"
#include "i8042.h"
#include "i8254.h"
// Global variables used in 'keyboard.c'
extern uint32_t sys_inb_counter;
extern uint8_t  scancode;
extern uint32_t global_interrupt_counter;


int main(int argc, char *argv[])
{
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");
  lcf_log_output("/home/lcom/labs/lab3/output.txt");
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();

  return 0;
}


int(kbd_test_scan)() 
{
  uint8_t irq_set;
  int ipc_status, r;
  message msg;

  uint8_t scancode_array[2];
  uint8_t index = 0;

  // subscribes to kbc interrupts
  if (keyboard_subscribe_int(&irq_set)) {
    printf("Failed to subscribe\n");
    return 1;
  }

  while (scancode != ESC_BREAK_CODE) 
  {
    // get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    // received notification
    if (is_ipc_notify(ipc_status)) 
    {
      switch (_ENDPOINT_P(msg.m_source))
      {
        case HARDWARE: // hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) 
          {
            kbc_ih(); // calls the interrupt handler
            
            if (check_complete_scancode(scancode_array, &index)) 
            {
                // checks if the scancode is complete, because there are sometimes 2 byte scancodes
                kbd_print_scancode(!(scancode & MAKE_CODE), index + 1, scancode_array);
                index = 0; //resets the index of array access
            }
          }
         break;

        default:
         break;
      }
    }

    else { }
  }

  // unsubscribes from kbc interrupts, after finishing the intended procedure
  if (keyboard_unsubscribe_int()) {
    printf("Failed to unsubscribe\n");
    return 1;
  }

  // finally, displays the number of sys_inb calls
  kbd_print_no_sysinb(sys_inb_counter);

  return 0;
}

int(kbd_test_poll)() {

  uint8_t scancode_array[2];
  uint8_t index = 0;

  while(scancode != ESC_BREAK_CODE) 
  {
    get_scan_code(); // continuously reads scancodes from KBD_OUT_BUF

    // checks if the scancode is complete, because there are sometimes 2 byte scancodes
    if(check_complete_scancode(scancode_array, &index)) 
    {
      kbd_print_scancode(!(scancode & MAKE_CODE), index + 1, scancode_array);
      index = 0; //resets the index of array access
    }
  }

  // prints the number of sys_inb calls
  kbd_print_no_sysinb(sys_inb_counter);
 
  // reenables keyboard interrupts
  if(reenable_keyboard_interrupts()) {
    printf("Unable to reenable keyboard interrupts\n");
    return 1;
  }

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  
  uint8_t irq_set_kbd;
  uint8_t irq_set_timer;
  int ipc_status, r;
  message msg;

  uint8_t scancode_array[2];
  uint8_t index = 0;

  // subscribes to kbc interrupts
  if (keyboard_subscribe_int(&irq_set_kbd)) {
    printf("Failed to subscribe keyboard interrupts\n");
    return 1;
  }

  if (timer_subscribe_int(&irq_set_timer)) {
    printf("Failed to subscribe timer 0 interrupts\n");
    return 1;
  }

  while ((scancode != ESC_BREAK_CODE) && ((global_interrupt_counter < n * sys_hz()))) 
  {
    // get a request message
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }


    // received notification
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source))
      {
        // hardware interrupt notification
        case HARDWARE:

          // subscribed interrupt
          if (msg.m_notify.interrupts & irq_set_kbd) {

            kbc_ih(); // calls the interrupt handler

            // checks if the scancode is complete, because there are sometimes 2 byte scancodes
            if (check_complete_scancode(scancode_array, &index)) {

              //prints the complete scancode
              kbd_print_scancode(!(scancode & MAKE_CODE), index + 1, scancode_array);
              index = 0; //resets the index of array access
            }
            global_interrupt_counter = 0;
          }
          else if(msg.m_notify.interrupts & irq_set_timer){
            timer_int_handler();
          }
          break;

        default:
          break;
      }
    }
  }

  // unsubscribes from timer and keyboard interrupts, after finishing the intended procedure
  if (keyboard_unsubscribe_int()) {
    printf("Failed to unsubscribe keyboard interrupts\n");
    return 1;
  }

  if(timer_unsubscribe_int()){
    printf("Failed to unsubscribe timer interrupts\n");
    return 1;
  }

  // finally, displays the number of sys_inb calls
  kbd_print_no_sysinb(sys_inb_counter);

  return 0;
}
