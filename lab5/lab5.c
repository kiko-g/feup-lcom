// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include <stdint.h>
#include <stdio.h>
// Any header files included below this line should have been created by you
#include "keyboard.h"
#include "video_card.h"
#include "sprite.h"
#include "macros.h"
#include "timer.c"

extern int kbd_hook_id;
extern uint32_t global_interrupt_counter;
extern uint8_t scancode;
extern bool status_error;



int main(int argc, char *argv[])
{
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");
    lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");
    lcf_log_output("/home/lcom/labs/lab5/output.txt");
    // handles control over to LCF - handles command line arguments and invokes the right function
    if (lcf_start(argc, argv)) return 1;
    // LCF clean up tasks, must be the last statement before return]
    lcf_cleanup();

    return 0;
}


int(video_test_init)(uint16_t mode, uint8_t delay)
{
  if(vg_init(mode) == NULL) {
    printf("Error at setting graphics mode\n");
    return 1;
  } 

  sleep(delay);
  
  if(vg_exit()) {
    printf("Error at vg_exit()\n");
    return 1;
  }

  return 0;
}


int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    if(vg_init(mode) == NULL)
    {
      vg_exit();
      return 1;
    }

    int r, ipc_status;
    uint8_t irq_set;
    message msg;

    if(vg_draw_rectangle(x, y, width, height, color))
    {
      printf("Error when drawing rectangle\n");
      if(vg_exit()) {
        printf("Error at vg_exit()\n");
        return 1;
      }
      return 1;
    }

    if(keyboard_subscribe_int(&irq_set)) {
      printf("Failed to subscribe to kb interrupts\n");
      return 1;
    }

    while(scancode != ESC_BREAK_CODE)
    {
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
              if (msg.m_notify.interrupts & irq_set) // subscribed interrupt
              {
                  kbc_ih(); // calls the interrupt handler
                  if(status_error) return 1;
              }
             break;

            default:
             break;
          }
        }
    }

    // unsubscribes from kbc interrupts, after finishing the intended procedure
    if (keyboard_unsubscribe_int()) {
      printf("Failed to unsubscribe kb interrupts\n");
      return 1;
    }

    if (vg_exit()) {
      printf("Error at vg_exit()\n");
      return 1;
    }

    return 0;
}


int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if(vg_init(mode) == NULL)
  {
    vg_exit();
    return 1;
  }

  if(vg_draw_rectangle_matrix(mode, no_rectangles, first, step))
  {
    printf("Error when drawing rectangle matrix\n");
    if(vg_exit()) {
      printf("Error at vg_exit()\n");
      return 1;
    }
    return 1;
  }

  int r, ipc_status;
  uint8_t irq_set;
  message msg;

  if(keyboard_subscribe_int(&irq_set)) {
    printf("Failed to subscribe to kb interrupts\n");
    return 1;
  }

  while(scancode != ESC_BREAK_CODE)
  {
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
          if (msg.m_notify.interrupts & irq_set) // subscribed interrupt
          {
              kbc_ih(); // calls the interrupt handler
              if(status_error) return 1;
          }
          break;

        default:
          break;
      }
    }
  }

  // unsubscribes from kbc interrupts, after finishing the intended procedure
  if (keyboard_unsubscribe_int()) {
    printf("Failed to unsubscribe kb interrupts\n");
    return 1;
  }

  if (vg_exit()) {
    printf("Error at vg_exit()\n");
    return 1;
  }

  return 0;
}


int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y)
{
  // switches to indexed video mode
  if(vg_init(INDEXED_COLOR) == NULL)
  {
    vg_exit();
    return 1;
  }  

  // get the pixmap from the XPM
  xpm_image_t img;
  uint8_t *map = xpm_load(xpm, XPM_INDEXED, &img);
  
  if(map == NULL)
  {
    printf("\nError in xpm_load()\n\n");
    vg_exit();
    return 1;
  }

  draw_pixmap(map, img, x, y);

  int r, ipc_status;
  uint8_t irq_set;
  message msg;

  if(keyboard_subscribe_int(&irq_set)) {
    printf("Failed to subscribe to kb interrupts\n");
    return 1;
  }  

  while(scancode != ESC_BREAK_CODE)
  {
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
          if (msg.m_notify.interrupts & irq_set) // subscribed interrupt
          {
              kbc_ih(); // calls the interrupt handler
              if(status_error) return 1;
          }
          break;

        default:
          break;
      }
    }
  }

  // unsubscribes from kbc interrupts, after finishing the intended procedure
  if (keyboard_unsubscribe_int()) {
    printf("Failed to unsubscribe kb interrupts\n");
    return 1;
  }

  if (vg_exit()) {
    printf("Error at vg_exit()\n");
    return 1;
  }

  return 0;
}


int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  
  int r, ipc_status;
  uint8_t irq_set_timer, irq_set_keyboard;
  message msg; 

  if (timer_subscribe_int(&irq_set_timer)) {
    printf("Timer subscription failed.\n");
    return 1;
  }

  if (keyboard_subscribe_int(&irq_set_keyboard)) {
    printf("Keyboard subscription failed.\n");
    return 1;
  }
  
  // switches to indexed video mode
  if(vg_init(INDEXED_COLOR) == NULL)
  {
    vg_exit();
    return 1;
  } 

  // determines distance to travel and creates the sprite
  int distance;
  Sprite* s;

  // vertical movement
  if(xi == xf) {
    distance = abs(yf-yi);

    if(speed > 0) {
      if(yi < yf)
        s = create_sprite(xpm, xi, yi, 0, speed);
      else
        s = create_sprite(xpm, xi, yi, 0, -speed);
    }
    else {
      if(yi < yf)
         s = create_sprite(xpm, xi, yi, 0, 1);
      else 
         s = create_sprite(xpm, xi, yi, 0, -1);
    }
  }
  // horizontal movement
  else if(yi == yf) {
    distance = abs(xf-xi);

    if(speed > 0) {
      if(xi < xf)
        s = create_sprite(xpm, xi, yi, speed, 0);
      else
        s = create_sprite(xpm, xi, yi, -speed, 0);
    }
    else {
      if(xi < xf)
        s = create_sprite(xpm, xi, yi, 1, 0);
      else 
        s = create_sprite(xpm, xi, yi, -1, 0);
    }
  }
  else {
    printf("No linear movement\n");
    return 1;
  }

  // frame variables
  uint8_t frame_time = sys_hz() / fr_rate;
  uint8_t frame_counter = 0;

  // loads XPM
  xpm_image_t img;
  uint8_t *map = xpm_load(xpm, XPM_INDEXED, &img);

  // and draws it
  draw_pixmap(map, img, xi, yi);

  while(scancode != ESC_BREAK_CODE)
  {
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
          if (msg.m_notify.interrupts & irq_set_keyboard) // subscribed interrupt
          {
              kbc_ih(); // calls the interrupt handler
              if(status_error) return 1;
          }
          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();

            // positive speed
            if (speed > 0) {

              // only draws when frame rate is correct and when there's distance for the sprite to travel
              if((global_interrupt_counter%frame_time==0) && (distance > 0)) {
                
                clear_sprite(s);

                update_sprite(s);
                distance -= speed;
                
                if(distance < 0) {
                  s->x = xf;
                  s->y = yf;
                }

                draw_sprite(s);
              }
            }
            // negative speed
            else {
              
              if(global_interrupt_counter % frame_time == 0)
                frame_counter++;
              
              if((frame_counter == abs(speed)) && (distance > 0)) {
                frame_counter = 0;

                clear_sprite(s);

                update_sprite(s);
                distance -= 1;

                draw_sprite(s);
              }
            }
          }
          break;

        default:
          break;
      }
    }
  }

  // frees the sprite
  destroy_sprite(s);
  s = NULL;

  // unsubscribes from kbc interrupts, after finishing the intended procedure
  if (keyboard_unsubscribe_int()) {
    printf("Failed to unsubscribe kb interrupts\n");
    return 1;
  }

  // also unsubscribes from timer interrupts
  if(timer_unsubscribe_int() != 0){ 
    printf("An error ocurred!\n");
    return 1;
  }

  if (vg_exit()) {
    printf("Error at vg_exit()\n");
    return 1;
  }

  return 0;
}


int(video_test_controller)()
{

  vg_vbe_contr_info_t c_info;

  if(get_vbe_contr_info(&c_info)){
    printf("Error while trying to get controller info\n");
    return 1;
  }

  if(vg_display_vbe_contr_info(&c_info)){
    printf("Error while trying to display controller info\n");
    return 1;
  }

  return 0;
}
