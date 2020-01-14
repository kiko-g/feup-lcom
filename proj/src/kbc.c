#include "kbc.h"

int kbc_issue_command(uint8_t cmd) {
	uint8_t status;
	uint8_t num_tries = 0;

	while(num_tries < MAX_TRIES) {
		// reads the status of the keyboard
		if(util_sys_inb(KBC_STATUS_PORT, &status))
			return 1; 
		
		// loops while 8042 input buffer is not empty
		if( (status & INPT_BUF_FULL) == 0 ) {
			
			sys_outb(KBC_INPT_BUF_COM, cmd);
			return 0;
		}
		
		num_tries++;

		WAIT_KBC;
	}

	return 1;
}


int kbc_issue_command_with_arg(uint8_t cmd, uint8_t arg) {
	uint8_t status;
	uint8_t num_tries = 0;

	// issues the command byte and error checks
	if(kbc_issue_command(cmd))
		return 1;

	// issues the argument and error checks
	while(num_tries < MAX_TRIES) {
		// reads the status of the keyboard
		if(util_sys_inb(KBC_STATUS_PORT, &status))
			return 1; 
		
		// loops while 8042 input buffer is not empty
		if( (status & INPT_BUF_FULL) == 0 ) {
			
			sys_outb(KBC_INPT_BUF_ARG, arg);
			
			return 0;
		}
		
		num_tries++;

		WAIT_KBC;
	}

	return 1;
}


int kbc_read_return_value(uint8_t *value) {
	uint8_t status;
	uint8_t num_tries = 0;

	while(num_tries < MAX_TRIES) {

		if(util_sys_inb(KBC_STATUS_PORT, &status))
			return 1;
		
		// loops while 8042 output buffer is empty
		if(status & OUT_BUF_FULL) {

			// reads the return value from the output buffer
			if(util_sys_inb(KBC_OUT_BUF, value))
				return 1;

			// checks if there was an error
			if ((status &(PARITY_ERROR | TRANSMIT_TIME_OUT)) == 0 )
				return 0;
			else
				return 1;
		}

		num_tries++;
		WAIT_KBC;
	}

	return 1;
}


int kbc_read_acknowledgment(uint8_t *acknowledgment_byte) {
  int num_tries = 0;
  uint8_t status;

  while(num_tries < MAX_TRIES)
  {
    num_tries++;
	  if(util_sys_inb(KBC_STATUS_PORT, &status)) 
      continue;
    
    if((status & OUT_BUF_FULL) == 0)
      continue;
    
    if(util_sys_inb(KBC_OUT_BUF, acknowledgment_byte)) 
      continue;

    if((*acknowledgment_byte != ACK) && (*acknowledgment_byte != NACK) && (*acknowledgment_byte != ERROR))
      continue;

    return 0;
  }

  return 1;
}
