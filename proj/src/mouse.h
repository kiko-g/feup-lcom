#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <lcom/lcf.h>
#include "kbc.h"


/** @defgroup mouse mouse
 * @{
 *
 * File for programming with the PS2 mouse
 */


/**
 * @brief Subscribes to interrupts from the mouse
 * @param bit_no Used to correctly identify interrupts, by serving as a mask for irq_set
 * @return 0 upon success, 1 otherwise
 */
int (mouse_subscribe_int)(uint8_t *bit_no);


/**
 * @brief Unsubscribes from the mouse's interrupts
 * @return 0 upon success, 1 otherwise
 */
int (mouse_unsubscribe_int)();


/**
 * @brief Enables the mouse's interrupts, while already subscribed
 */
void (mouse_enable_int)();


/**
 * @brief Disables the mouse's interrupts, while already subscribed
 */
void (mouse_disable_int)();


/**
 * @brief The KBC's input handler. Retrieves a scancode when the interrupt is generated
 */
void (mouse_ih)();


/** 
 * @brief check if packet array is complete, build it otherwise
 * @param packet_array array containing bytes sent by PS2 mouse
 * @param packet_index index of byte in packet array (0..2) 
 * @return true upon success, false otherwise
 */
bool check_complete_packet(uint8_t packet_array[], uint8_t *packet_index);


/** 
 * @brief parse/convert info inside packet array to a packet struct
 * @param packet_array array containing bytes sent by PS2 mouse
 * @param pp packet struct to be built
 */
void create_packet_struct(uint8_t packet_array[], struct packet *pp);


/**
 * @brief Issues a command to the mouse
 * @param cmd The command byte to be issued
 * @return 0 if the command was issued successfully, any other value otherwise  
 */
int mouse_issue_command(uint8_t cmd);


/** 
 * @brief Enable stream mode (Issue STREAM_MODE command)
 * @return 0 if the command was issued successfully, any other value otherwise 
 */
int mouse_set_stream_mode();


/** 
 * @brief Enbale data reporting (Issue ENABLE_DATA_REPORT command)
 * @return 0 if the command was issued successfully, any other value otherwise 
 */
int mouse_enable_data_rep();


/** 
 * @brief Disable data reporting (Issue DISABLE_DATA_REPORT command)
 * @return 0 if the command was issued successfully, any other value otherwise 
 */
int mouse_disable_data_rep();


/** 
 * @brief Detects an event and identifies it
 * @param pp Pointer to a packet
 * @return struct mouse_ev, the object with event detected and movement in x and y
 */
struct mouse_ev mouse_event_detector(struct packet *pp);


/**
 * @brief An enum with all the possible drawing states
 */
enum state_t { INIT, DRAW_UP, MIDPOINT, DRAW_DOWN, END };


/**
 * @brief A struct that holds information about a drawing state
 */
struct Drawing {
  enum state_t state;
  int16_t dx;  // mouse x-displacement: rightwards is positive
  int16_t dy;  // mouse y-displacement: upwards is positive  
};


/**
 * @brief Processes the event detected
 * @param drawing A struct that holds information about the drawing state
 * @param event_st The current mouse event (button clicked, mouse moved, etc.)
 * @param x_len The length traveled in the x-axis, for each line drawn
 * @param tolerance Maximum tolerance
 */ 
void process_event(struct Drawing* drawing, struct mouse_ev event, uint8_t x_len, uint8_t tolerance);

/**
 * @} 
 */


#endif /* _LCOM_MOUSE_H */
