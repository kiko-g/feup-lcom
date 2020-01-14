#include "uart_protocol.h"
#include "serial_port.h"
#include "utils.h"
#include "uart.h"


int (serial_port_configure_com1)() {
  serial_port_set_bit_rate(19200);

  uint32_t LCR_byte = WORD_LENGTH_8 | STOP_BITS_1 | PARITY_ODD;
  if(serial_port_write_LCR(LCR_byte)) {
    return 1;
  }

  uint32_t IER_byte = ENABLE_RECEIVED_DATA_INTERRUPT | ENABLE_RECEIVER_LINE_STATUS_INTERRUPT;
  if(serial_port_write_IER(IER_byte)) {
    return 1;
  }

  uint32_t FCR_byte = FIFO_INTERRUPT_TRIGGER_LEVEL_1 | CLEAR_TRANSMIT_FIFO | CLEAR_RECEIVE_FIFO | ENABLE_FIFO;
  if(serial_port_write_FCR(FCR_byte)) {
    return 1;
  }

  return 0;
}


bool (serial_port_received_all)(char** msg, uint16_t* length) 
{
  if(*length == 0) {
    return false;
  }
  else if (*length > MAX_MSG_SIZE) {
    free(*msg);
    *length = 0;
    *msg = NULL;
    return false;
  }

  switch((*msg)[0]) {
    // Landed
    case 'L':
      if(*length == LANDED_MSG_SIZE)
        return true;

      return false;
      break;

    // Blown up
    case 'B':
      if(*length == BLOWN_UP_MSG_SIZE)
        return true;
      
      return false;

    // Connect
    case 'C':
      if(*length == CONNECT_MSG_SIZE)
        return true;
      
      return false;
      break;

    default:
      *length = 0;

      free(*msg);
      *msg = NULL;
      
      return false;
      
  }

  return 0;
}

