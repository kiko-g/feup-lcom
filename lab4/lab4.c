// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
// Any header files included below this line should have been created by you
#include "i8254.h"
#include "i8042.h"
#include "mouse.h"

// Global variables
extern int mouse_hook_id;
extern uint32_t global_interrupt_counter;


int main(int argc, char *argv[])
{
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");
  lcf_log_output("/home/lcom/labs/lab4/output.txt");
  if(lcf_start(argc, argv)) return 1;
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt)
{
  uint8_t irq_set;
  int ipc_status, r;
  message msg;

  uint8_t packet_array[3];
  uint8_t index = 0;

  if(mouse_subscribe_int(&irq_set) != OK) {
    printf("Failed to subscribe\n");
    return 1;
  }

  mouse_disable_int();

  if(mouse_enable_data_rep()) {
    printf("Failed to enable data report\n");
    return 1;
  }

  mouse_enable_int();



  while (cnt > 0)
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
        // hardware interrupt notification
        case HARDWARE:
          // subscribed interrupt
          if (msg.m_notify.interrupts & irq_set)
          {
            // calls the interrupt handler
            mouse_ih();

            // checks if the packet is complete and if it's synchronized
            if (check_complete_packet(packet_array, &index))
            {
              struct packet pp;
              create_packet_struct(packet_array, &pp);
              mouse_print_packet(&pp);

              index = 0; // resets the index of array access
              cnt--;
            }
          }
        break;

        default:
        break;
      }
    }

    else { }
  }
  
  mouse_disable_int();

  if(mouse_disable_data_rep()) {
    printf("Unable to disable data reporting\n");
    return 1;  
  }

  mouse_enable_int();

  if(mouse_unsubscribe_int()) {
    printf("Unable to unsubscribe from the mouse's interrupts\n");
    return 1;
  }


  uint8_t reset_cmd = minix_get_dflt_kbc_cmd_byte();
  if(kbc_issue_command_with_arg(WRITE_CMD_BYTE, reset_cmd)) {
    printf("Unable to reset mouse to its original configuration\n");
    return 1;
  }

  return 0;
}


int(mouse_test_remote)(uint16_t period, uint8_t cnt)
{
  uint8_t packet_array[3];
  uint8_t index = 0;

  while(cnt > 0)
  {
    // request a packet
    if(index == 0) 
      mouse_issue_command(READ_DATA);

    // continuously reads packets from KBD_OUT_BUF
    mouse_ih();

    // checks if the packet is complete and if it's synchronized
    if(check_complete_packet(packet_array, &index))
    {
      struct packet pp;
      create_packet_struct(packet_array, &pp); 
      mouse_print_packet(&pp);

      index = 0; // resets the index of array access
      cnt--;
      DELAY(period * 1000);
    }
  }


  if(mouse_set_stream_mode()) {
    printf("Failed to set stream mode\n");
    return 1;
  }

  if(mouse_disable_data_rep()) {
    printf("Unable to disable data reporting\n");
    return 1;
  }

  uint8_t reset_cmd = minix_get_dflt_kbc_cmd_byte();

  if(kbc_issue_command_with_arg(WRITE_CMD_BYTE, reset_cmd)) {
    printf("Unable to reset mouse to its original configuration\n");
    return 1;
  }  

  return 0;
}


int(mouse_test_async)(uint8_t idle_time)
{
  uint8_t irq_set_mouse, irq_set_timer;
  int ipc_status, r;
  message msg;

  uint8_t packet_array[3];
  uint8_t index = 0;
  
  if (mouse_subscribe_int(&irq_set_mouse) != OK) {
    printf("Failed to subscribe\n");
    return 1;
  }

  mouse_disable_int();

  if (mouse_enable_data_rep()) {
    printf("Failed to enable data report\n");
    return 1;
  }

  mouse_enable_int();

  if (timer_subscribe_int(&irq_set_timer)) {
    printf("Failed to subscribe timer 0 interrupts\n");
    return 1;
  }



  while (global_interrupt_counter < idle_time * sys_hz())
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
          if (msg.m_notify.interrupts & irq_set_mouse)
          {
            // calls the interrupt handler
            mouse_ih();

            // checks if the packet is complete and if it's synchronized
            if (check_complete_packet(packet_array, &index))
            {
              struct packet pp;
              create_packet_struct(packet_array, &pp);
              mouse_print_packet(&pp);
              index = 0; // resets the index of array access
            }

            global_interrupt_counter = 0;
          }

          else if (msg.m_notify.interrupts & irq_set_timer) 
            timer_int_handler();
        break;

        default:
         break;
      }
    }

    else { }
  }

  mouse_disable_int();
  
  if (mouse_disable_data_rep()) {
    printf("Unable to disable data reporting\n");
    return 1;
  }
  
  mouse_enable_int();

  if (mouse_unsubscribe_int()) {
    printf("Unable to unsubscribe from the mouse's interrupts\n");
    return 1;
  }

  if (timer_unsubscribe_int()) {
    printf("Failed to unsubscribe timer interrupts\n");
    return 1;
  }

  uint8_t reset_cmd = minix_get_dflt_kbc_cmd_byte();
  if (kbc_issue_command_with_arg(WRITE_CMD_BYTE, reset_cmd)) {
    printf("Unable to reset mouse to its original configuration\n");
    return 1;
  }

  return 0;
}



int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance)
{
  uint8_t irq_set;
  int ipc_status, r;
  message msg;

  uint8_t packet_array[3];
  uint8_t index = 0;

  struct Drawing gesture;
  gesture.state = INIT;

  if (mouse_subscribe_int(&irq_set) != OK) {
    printf("Failed to subscribe\n");
    return 1;
  }

  mouse_disable_int();
  
  if(mouse_enable_data_rep()){
    printf("Failed to enable data report\n");
    return 1;
  }
  
  mouse_enable_int();
  
  while (gesture.state != END)
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
            // calls the interrupt handler
            mouse_ih();

            // checks if the packet is complete and if it's synchronized
            if (check_complete_packet(packet_array, &index))
            {
              struct packet pp;
              struct mouse_ev event;
              create_packet_struct(packet_array, &pp);
              mouse_print_packet(&pp);
              index = 0; // resets the index of array access
              
              event = mouse_event_detector(&pp);
              process_event(&gesture, event, x_len, tolerance);
            }

          }
        break;

        default:
         break;
      }
    }

    else { } //do nothing
  }



  mouse_disable_int();
  
  if (mouse_disable_data_rep()) {
    printf("Unable to disable data reporting\n");
    return 1;
  }
  
  mouse_enable_int();

  if (mouse_unsubscribe_int()) {
    printf("Unable to unsubscribe from the mouse's interrupts\n");
    return 1;
  }

  uint8_t reset_cmd = minix_get_dflt_kbc_cmd_byte();
  if (kbc_issue_command_with_arg(WRITE_CMD_BYTE, reset_cmd)) {
    printf("Unable to reset mouse to its original configuration\n");
    return 1;
  }

  return 0;
}

