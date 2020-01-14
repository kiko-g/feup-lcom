// Lcom's headers
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
// Useful headers
#include <stdbool.h>
#include <stdint.h>
// Group's created headers
#include "keyboard.h"
#include "mouse.h"
#include "space_lander.h"
#include "video_card.h"
#include "serial_port.h"
#include "uart_protocol.h"
#include "macros.h"

// Interrupt variables
uint8_t irq_set_timer, irq_set_keyboard, irq_set_mouse, irq_set_serial_port;



int main(int argc, char *argv[]) 
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int(proj_main_loop)(int argc, char *argv[]) {

  // Configures the serial port
  if(serial_port_configure_com1())
    return 1;

  // Subscribes to interrupts from the serial port
  if (serial_port_com1_subscribe_int(&irq_set_serial_port)) {
    printf("Unable to subscribe to the serial port's interrupts\n");
    return 1;
  }

  // Subscribes to interrupts from the timer
  if (timer_subscribe_int(&irq_set_timer)) {
    printf("Unable to subscribe to the timer's interrupts\n");
    return 1;
  }

  // Subscribes to interrupts from the keyboard
  if (keyboard_subscribe_int(&irq_set_keyboard)) {
    printf("Unable to subscribe to the keyboard's interrupts\n");
    return 1;
  }

  // Subscribes to interrupts from the mouse
  if (mouse_subscribe_int(&irq_set_mouse)) {
    printf("Unable to subscribe to the mouse's interrupts\n");
    return 1;
  }

  // Enables mouse's data reporting
  mouse_disable_int();
  if(mouse_enable_data_rep()) {
    printf("Failed to enable the mouse's data reporting\n");
    return 1;
  }
  mouse_enable_int();
  
  // Sets the video mode and initializes graphics
  if (vg_init(DIRECT_COLOR_MODE) == NULL) {
    vg_exit();
    return 1;
  }

  // Starts the game
  start_lunar_lander();

  // Determine the events generated and handles them, in a loop, until the game's ended
  play_lunar_lander();

  // Disables mouse's data reporting
  mouse_disable_int();
  if(mouse_disable_data_rep()) {
    printf("Unable to disable the mouse's data reporting\n");
    return 1;  
  }
  mouse_enable_int();
  
  // Unsubscribes from the mouse's interrupts
  if (mouse_unsubscribe_int()) {
    printf("Unable to unsubscribe from the keyboard's interrupts\n");
    return 1;
  }

  // Unsubscribes from the keyboard's interrupts
  if (keyboard_unsubscribe_int()) {
    printf("Unable to unsubscribe from the keyboard's interrupts\n");
    return 1;
  }

  // Unsubscribes from the timer's interrupts
  if (timer_unsubscribe_int()) {
    printf("Unable to unsubscribe from the keyboard's interrupts\n");
    return 1;
  }

  // Unsubscribes from the serial ports's interrupts
  if (serial_port_com1_unsubscribe_int()) {
    printf("Unable to unsubscribe from the keyboard's interrupts\n");
    return 1;
  }  

  // Exits video mode
  if (vg_exit())
    return 1;

  // Frees the double buffer
  destroy_buffer();

  return 0;
}
