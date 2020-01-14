#include "keyboard.h"

/* Global variables */
int kbd_hook_id = KBD_IRQ;
uint8_t scancode;
uint32_t sys_inb_counter = 0;
bool status_error = false;


// will only count the uses of sys_inb when in LAB3, because of the flag
#ifdef LAB3
int sys_inb_with_count(int port, uint8_t *value) {
	sys_inb_counter++;
	return util_sys_inb(port, value);
}
#else
int sys_inb_with_count(int port, uint8_t *value) {
	return util_sys_inb(port, value);
}
#endif


int (keyboard_subscribe_int)(uint8_t *bit_no) 
{
    *bit_no = BIT(kbd_hook_id); // sets the bitmask for irq_set

    //function returns OK on success
    if(sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK) return 1;
    else return 0;
}


int (keyboard_unsubscribe_int)()
{	
  //function returns OK on success 
  if(sys_irqrmpolicy(&kbd_hook_id) != OK) return 1;
  else return 0;
}


void (kbc_ih)()
{	
	get_scan_code();
}


bool (check_complete_scancode)(uint8_t scancode_array[], uint8_t *index)
{
	if (status_error) return false;
	
	// if there was an incompleted code, stores it in position 0 (index is 0)
	if (scancode == INCOMPLETE_CODE) {
		scancode_array[*index] = INCOMPLETE_CODE;
		*index = 1;
		return false;
	}

	else {
		scancode_array[*index] = scancode;
		return true;
	}
}

void (get_scan_code)() 
{
	uint8_t status;
	uint8_t scancode_value;
	sys_inb_with_count(KBD_STATUS_PORT, &status);

  if(status & OUT_BUF_FULL)
  {
		sys_inb_with_count(KBD_OUT_BUF, &scancode_value);
		if((status & (PARITY_ERROR | RECEIVE_TIME_OUT)) == 0)
			scancode = scancode_value;

		else {
      status_error = true;
      return;
    }
	}

	else { 
    status_error = true; 
    return; 
  }

	status_error = false;
}


int kbc_issue_command(uint8_t cmd) 
{
	uint8_t status;
	uint8_t num_tries = 0;

	while(num_tries < MAX_TRIES)
  {
		if(util_sys_inb(KBD_STATUS_PORT, &status)) return 1;  // reads the status of the keyboard
    if((status & INPT_BUF_FULL) == 0) // loops while 8042 input buffer is not empty
    {
      sys_outb(KBD_INPT_BUF_COM, cmd);
			return 0;
		}
		num_tries++;
		WAIT_KBC;
	}

	return 1;
}


int kbc_issue_command_with_arg(uint8_t cmd, uint8_t arg) 
{
	uint8_t status;
	uint8_t num_tries = 0;

	// issues the command byte and error checks
	if(kbc_issue_command(cmd)) return 1;

	// issues the argument and error checks
	while(num_tries < MAX_TRIES) 
  {
		// reads the status of the keyboard
		if(util_sys_inb(KBD_STATUS_PORT, &status)) return 1; 
		
		// loops while 8042 input buffer is not empty
		if((status & INPT_BUF_FULL) == 0) 
    {	
			sys_outb(KBD_INPT_BUF_ARG, arg);	
			return 0;
		}
		
		num_tries++;

		WAIT_KBC;
	}

	return 1;
}


int kbc_read_return_value(uint8_t *value) 
{
	uint8_t status;
	uint8_t num_tries = 0;

	while(num_tries < MAX_TRIES) 
  {
		if(util_sys_inb(KBD_STATUS_PORT, &status)) return 1;
		
		// loops while 8042 output buffer is empty
		if(status & OUT_BUF_FULL) 
    {
			// reads the return value from the output buffer
			if(util_sys_inb(KBD_OUT_BUF, value)) return 1;

			// checks if there was an error
			if((status &(PARITY_ERROR | TRANSMIT_TIME_OUT)) == 0) return 0; 
			else return 1;
		}

		WAIT_KBC;
	}

	return 1;
}


int reenable_keyboard_interrupts() 
{	
	if(kbc_issue_command(READ_CMD_BYTE)) return 1;
  
  uint8_t command_byte;
  if(kbc_read_return_value(&command_byte)) return 1;

  uint8_t arg = command_byte | ENABLE_INTERRUPTS;
  if(kbc_issue_command_with_arg(WRITE_CMD_BYTE, arg)) return 1;

	return 0;
}
