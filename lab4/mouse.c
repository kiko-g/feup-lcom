#include "mouse.h"
//Global variables 
static bool status_error = false;
uint8_t packet;
int mouse_hook_id = 2;

int (mouse_subscribe_int)(uint8_t *bit_no)
{
    *bit_no = BIT(mouse_hook_id);
    if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK)
      return 1;
    else return 0;
}

int (mouse_unsubscribe_int)()
{
    if(sys_irqrmpolicy(&mouse_hook_id) != OK) return 1;
    else return 0;
}


void (mouse_enable_int)() { sys_irqenable(&mouse_hook_id); }
void (mouse_disable_int)() { sys_irqdisable(&mouse_hook_id); }


void (mouse_ih)() 
{
	uint8_t status;
	uint8_t packet_temporary;

	util_sys_inb(KBC_STATUS_PORT, &status);

  if(((status & (PARITY_ERROR | PS2_TIME_OUT)) == 0) && ((status & AUXILIARY_DEVICE) != 0)) {
    util_sys_inb(KBC_OUT_BUF, &packet);    
	  status_error = false;    
  }
  else {
    util_sys_inb(KBC_OUT_BUF, &packet_temporary);
    status_error = true;
  }
}


bool check_complete_packet(uint8_t packet_array[], uint8_t *packet_index) 
{
  if(status_error) return false;
  if(*packet_index == 0) 
  {
    if(!(packet & FIRST_PACKET)) return false;
    packet_array[0] = packet;
    (*packet_index)++;
  }
  else {
    packet_array[*packet_index] = packet;
    (*packet_index)++;
  }
  if((*packet_index) == 3) return true;

  return false;
}


void create_packet_struct(uint8_t packet_array[], struct packet *pp)
{
  // Places packets bytes
  pp->bytes[0] = packet_array[0];
  pp->bytes[1] = packet_array[1];
  pp->bytes[2] = packet_array[2];

  // Determines buttons pressed
  pp->lb = (packet_array[0] & LB);
  pp->rb = (packet_array[0] & RB);
  pp->mb = (packet_array[0] & MB);

  // Calculates displacement
  pp->x_ov = (packet_array[0] & X_OVF);
  pp->y_ov = (packet_array[0] & Y_OVF);

  // Determines if x is in two's complement
  if (packet_array[0] & X_SIGN) pp->delta_x = (packet_array[1] | 0xFF00);
  else pp->delta_x = packet_array[1];

  // Determines if x is in two's complement
  if (packet_array[0] & Y_SIGN) pp->delta_y = (packet_array[2] | 0xFF00);
  else pp->delta_y = packet_array[2];
}


int mouse_issue_command(uint8_t cmd) 
{
	uint8_t acknowledgment_byte = NACK;
  uint8_t num_tries = 0;

  while(acknowledgment_byte == NACK && num_tries < MAX_TRIES) 
  {
    if(kbc_issue_command_with_arg(WRITE_MOUSE, cmd)) return 1;
    if(kbc_read_acknowledgment(&acknowledgment_byte)) return 1;
    if(acknowledgment_byte == ERROR) return 1;

    num_tries++;
  }

  return acknowledgment_byte != ACK;
}


int mouse_set_stream_mode()  { return mouse_issue_command(STREAM_MODE); }
int mouse_enable_data_rep()  { return mouse_issue_command(ENABLE_DATA_REPORT); }
int mouse_disable_data_rep() { return mouse_issue_command(DISABLE_DATA_REPORT); }


struct mouse_ev mouse_event_detector(struct packet *pp)
{
  static struct packet prev_pp;
  struct mouse_ev event;

    // press left button
    if (pp->lb == 1 && prev_pp.lb == 0 && pp->rb == 0 && prev_pp.rb == 0) {
    event.delta_x = pp->delta_x;
    event.delta_y = pp->delta_y;
    event.type = LB_PRESSED;
  }

  // release left button
  else if (pp->lb == 0 && prev_pp.lb == 1 && pp->rb == 0 && prev_pp.rb == 0) {
    event.delta_x = pp->delta_x;
    event.delta_y = pp->delta_y;
    event.type = LB_RELEASED;
  }

  // press right button
  else if (pp->lb == 0 && prev_pp.lb == 0 && pp->rb == 1 && prev_pp.rb == 0) {
    event.delta_x = pp->delta_x;
    event.delta_y = pp->delta_y;
    event.type = RB_PRESSED;
  }

  // release right button
  else if (pp->lb == 0 && prev_pp.lb == 0 && pp->rb == 0 && prev_pp.rb == 1) {
    event.delta_x = pp->delta_x;
    event.delta_y = pp->delta_y;
    event.type = RB_RELEASED;
  }

  // mouse movement detected
  else if(pp->lb == prev_pp.lb && pp->rb == prev_pp.rb && (pp->delta_x != 0 || pp->delta_y != 0)){
    event.delta_x = pp->delta_x;
    event.delta_y = pp->delta_y;
    event.type = MOUSE_MOV;
  }

  // any other button pressed
  else{
    event.delta_x = 0;
    event.delta_y = 0;
    event.type = BUTTON_EV;
  }


  prev_pp = *pp;
  return event;
}


void process_event(struct Drawing *drawing, struct mouse_ev event, uint8_t x_len, uint8_t tolerance)
{
  switch(drawing->state)
  {
    case INIT:
      drawing->dx = 0;
      drawing->dy = 0;
      if(event.type == LB_PRESSED)
        drawing->state = DRAW_UP;
      break;

    case DRAW_UP:
      if(event.type == MOUSE_MOV)
      {
        //back to start if there are x/y detours gretater than tolerance
        if ((event.delta_x < 0) && (abs(event.delta_x) >= tolerance))
          drawing->state = INIT; //negative x detour (went left too much)
        
        else if ((event.delta_y < 0) && (abs(event.delta_y) >= tolerance))
          drawing->state = INIT; //negative y detour (went down too much)

        else{
          drawing->dx += event.delta_x;
          drawing->dy += event.delta_y;
        }
      }


      //go back to start upon any other button event
      else if(event.type == BUTTON_EV)
        drawing->state = INIT;


      else if(event.type == LB_RELEASED)
      {
        int16_t x = (int16_t)abs(drawing->dx);
        int16_t y = (int16_t)abs(drawing->dy);
        float slope = (float) y / (float) x;

        // check for correct slope and drawn length 
        // if so, ready to enter middle point
        if ((slope > 1) && (x >= x_len)) {
          drawing->dx = 0;
          drawing->dy = 0;
          drawing->state = MIDPOINT;
        }

        //go to start if failed condition above after releasing left button
        else drawing->state = INIT;
      }

      break;

    case MIDPOINT:      
      if(event.type == MOUSE_MOV){
        drawing->dx += event.delta_x;
        drawing->dy += event.delta_y;
        if((abs(drawing->dx) > tolerance) || (abs(drawing->dy) > tolerance))
          drawing->state = INIT; // went too far in any direction (positive/negative in x/y)
      }

      //buttons different from LB or RB arent accepted
      else if(event.type == BUTTON_EV)
        drawing->state = INIT;

      //draw up again
      else if(event.type == LB_PRESSED) 
        drawing->state = DRAW_UP;

      //draw down
      else if(event.type == RB_PRESSED) {
        drawing->dx = 0;
        drawing->dy = 0;
        drawing->state = DRAW_DOWN;
      }

      break;

    case DRAW_DOWN:
      if(event.type == MOUSE_MOV)
      {
        //back to start if there are x/y detours gretater than tolerance
        if((event.delta_x < 0) && (abs(event.delta_x) >= tolerance))
          drawing->state = INIT; //negative x detour (went left too much)

        else if((event.delta_y > 0) && (abs(event.delta_y) >= tolerance))
          drawing->state = INIT; //positive y detour (went up too much)

        else{
          drawing->dx += event.delta_x;
          drawing->dy += event.delta_y;
        }
      }

      //go back to start upon any other button event
      else if (event.type == BUTTON_EV)
        drawing->state = INIT;

      else if(event.type == RB_RELEASED)
      {
        int16_t x = (int16_t)abs(drawing->dx);
        int16_t y = (int16_t)abs(drawing->dy);
        float slope = (float)(y/x); //absolute value of slope abs(x) / abs(y)

        // check for correct slope and drawn length 
        // if so, ready to enter end state
        if ((slope > 1) && (x >= x_len))
          drawing->state = END;

        //go to start if failed condition above after releasing right button
        else drawing->state = INIT;
      }

      break;

    case END:
      break;

    default:
      break;
  }

  return;
}
