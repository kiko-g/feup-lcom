#include "serial_port.h"
#include "utils.h"
#include "uart.h"

// Global variable
int serial_port_hook_id = 4;


int (serial_port_com1_subscribe_int)(uint8_t *bit_no) 
{
  *bit_no = BIT(serial_port_hook_id); // sets the bitmask for irq_set

	if(sys_irqsetpolicy(UART_COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_port_hook_id) != OK) {
    return 1;
  }

  else return 0;
}


int (serial_port_com1_unsubscribe_int)() 
{
  if(sys_irqrmpolicy(&serial_port_hook_id) != OK) {  
    return 1;
  }

  else return 0;
}


int serial_port_read_IIR(uint32_t* IIR_byte) 
{
  if(sys_inb(COM1_BASE_ADDR + IIR, IIR_byte)){
		return 1;
	}

	return 0;
}


int serial_port_read_LSR(uint32_t* LSR_byte) 
{
  if(sys_inb(COM1_BASE_ADDR + LSR, LSR_byte)){
		return 1;
	}

	return 0;
}


int serial_port_read_RBR(uint32_t* RBR_byte) 
{
	uint32_t IIR_byte;
	uint8_t try = 0;

  while(try < UART_MAX_TRIES) {
    if((serial_port_read_IIR(&IIR_byte) == 0) && (IIR_byte & INTERRUPT_ORIGIN_DATA_AVAILABLE)) {
      sys_inb(COM1_BASE_ADDR + RBR, RBR_byte);
      return 0;
    }
    //WAIT_UART;
    try++;
  }

  return 1;
}


int serial_port_read_IER(uint32_t* IER_byte) {
  if(sys_inb(COM1_BASE_ADDR + IER, IER_byte)){
		return 1;
	}

	return 0;
}


int serial_port_write_IER(uint32_t IER_byte) {
  if(sys_outb(COM1_BASE_ADDR + IER, IER_byte)){
		return 1;
	}

	return 0;  
}



int serial_port_write_FCR(uint32_t FCR_byte) {
  if(sys_outb(COM1_BASE_ADDR + FCR, FCR_byte)){
		return 1;
	}

	return 0;  
}


int serial_port_write_THR(uint32_t THR_byte) 
{
	uint32_t LSR_byte;
	uint8_t try = 0;
  
  while(try < 100) {
    if((serial_port_read_LSR(&LSR_byte) == 0) && (LSR_byte & TRANSMITTER_EMPTY)) {
      sys_outb(COM1_BASE_ADDR + THR, THR_byte);
      return 0;
    }
    //WAIT_UART;
    try++;
  }

  return 1;
}

int serial_port_write_LCR(uint32_t LCR_byte) {
  if(sys_outb(COM1_BASE_ADDR + LCR, LCR_byte)){
		return 1;
	}

	return 0;  
}

int serial_port_read_LCR(uint32_t* LCR_byte) {
  if(sys_inb(COM1_BASE_ADDR + LCR, LCR_byte)) {
    return 1;
  }
  
  return 0;
}

int serial_port_access_DL(){
    uint32_t LCR_byte;

    if (serial_port_read_LCR(&LCR_byte)) {
      return 1;
    }

    LCR_byte = (LCR_byte | DLAB);

    return (serial_port_write_LCR(LCR_byte));
}

int serial_port_deaccess_DL(){
    uint32_t LCR_byte;

    if (serial_port_read_LCR(&LCR_byte)) {
      return 1;
    }

    uint32_t no_access = (uint32_t) ~DLAB;
    LCR_byte = (LCR_byte & no_access);

    return (serial_port_write_LCR(LCR_byte));
}


int serial_port_write_DL(uint8_t DLM_byte, uint8_t DLL_byte) {
  if(serial_port_access_DL()) {
    return 1;
  }

  if(sys_outb(COM1_BASE_ADDR + DLM, DLM_byte) && sys_outb(COM1_BASE_ADDR + DLL, DLL_byte)) {
    return 1;
  }

  if(serial_port_deaccess_DL()) {
    return 1;
  }

  return 0;
}

int serial_port_set_bit_rate(uint16_t bit_rate) {
  uint16_t set = 115200/bit_rate;

  uint8_t msb, lsb;
  util_get_MSB(set, &msb);
  util_get_LSB(set, &lsb);

  if(serial_port_write_DL(msb, lsb))
    return 1;

  return 0;
}

void serial_port_flush() {
  uint32_t status;
  if(serial_port_read_LSR(&status))
    return;

  while(status & RECEIVER_READY) {
    uint32_t aux;

    if(serial_port_read_RBR(&aux))
      return;

    if(serial_port_read_LSR(&status))
      return;
  }
}


void (serial_port_receive_msg)(char** msg, uint16_t* length) 
{
  uint32_t byte_received;

  if(serial_port_read_RBR(&byte_received))
    return;

  (*length) = (*length) + 1;
	*msg = (char *) realloc (*msg, sizeof(char)*(*length));
  (*msg)[(*length)-1] = byte_received;

}


void (serial_port_send_char)(char byte) 
{
  serial_port_write_THR(byte);
}


void (serial_port_send_msg)(char* msg, unsigned int length) 
{
  for (unsigned int i = 0; i < length; ++i) {
    serial_port_send_char(msg[i]);
  }
}
