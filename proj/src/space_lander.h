#ifndef _SPACE_LANDER_H_
#define _SPACE_LANDER_H_

#define ASTEROID_AMOUNT 3
#define ASTEROID_REGEN_TIME 120
#define ROCKET_AMOUNT 5
#define ROCKET_COOLDOWN_TIME 25

#define LANDING        0x00FF00
#define LEVEL1_GROUND  0x858D8A
#define LEVEL2_GROUND  0xD16E12
#define LEVEL3_GROUND  0xC6BF8C


/** @defgroup spacelander spacelander
 * @{
 *
 * Main file that holds game states and deals with the game's logic (space lander)
 */



/*********************/
/**      Enums      **/
/*********************/

/**
 * @brief An enum for the game's state machine, to determine the current game state and act accordingly
 */
typedef enum {
  MENU,        //!< Game is in the main menu
  LOAD,        //!< A given level is being loaded
  GAME,        //!< A given level is being played
  GAME_WON,
  GAME_OVER,   //!< The current level has been finished
  MULTIPLAYER,
  QUIT         //!< Game has ended
} GameStates;

/**
 * @brief An enum for the possible menu events
 */
typedef enum {
  WAIT_MENU,
  DRAW,
  PLAY,
  WAIT_PLAY,
  RULES,
  WAIT_RULES,
  HIGHSCORES,
  QUIT_BUTTON
} MenuStates;

/**
 * @brief An enum for the possible player types
 */
typedef enum {
  SINGLE,
  FIRST,
  SECOND
} PlayerStates;

/**
 * @brief An enum for the possible levels
 */
typedef enum {
  LEVEL_1,    //!< First level, the Moon
  LEVEL_2,    //!< Second level, Mars
  LEVEL_3,    //!< Third level, the Ender
} Levels;

/**
 * @brief An enum for the possible keyboard events
 */
typedef enum {
  LEFT_ARROW_PRESS,     //!< Left arrow's been pressed, starting the ship's anti-clockwise rotation
  LEFT_ARROW_RELEASE,   //!< Left arrow's been released, stopping the ship's anti-clockwise rotation
  RIGHT_ARROW_PRESS,    //!< Right arrow's been pressed, starting the ship's clockwise rotation
  RIGHT_ARROW_RELEASE,  //!< Right_arrow's been released, stopping the ship's clockwise rotation
  SPACE_PRESS,          //!< Space's been pressed, starting the ship's boost
  SPACE_RELEASE,        //!< Space's been pressed, stopping the ship's boost
  ESCAPE                //!< Escape key's been released, stopping the game
} KeyboardEvents;

/**
 * @brief An enum for the possible mouse events
 */
typedef enum {
  LEFT_BUTTON_PRESSED,    //!< Left button's been pressed,
  LEFT_BUTTON_RELEASED,   //!< Left button's been pressed,  
  RIGHT_BUTTON_PRESSED,   //!< Right button's been pressed,
  RIGHT_BUTTON_RELEASED,  //!< Right button's been pressed,
  LEFT_MOVEMENT,          //!< Movement to the left's been made, rotating the ship anti-clockwise
  RIGHT_MOVEMENT,         //!< Movement to the right's been made, rotating the ship clockwise
  MOVEMENT //TESTING
} MouseEvents;

/** @brief An enum for the possible timer events */
typedef enum {
  UPDATE,               //!< Updates graphics and spaceship
  GENERATE_ASTEROID     //!< Generates an asteroid
} TimerEvents;


/*********************/
/**     Structs     **/
/*********************/

struct {
  bool timer_occured;
  bool keyboard_occured;
  bool mouse_occured;
} events;

struct {
  KeyboardEvents type;
} keyboard_event;

struct {
  TimerEvents type;
} timer_event;

struct {
  MouseEvents type;
  int x;
  int y;
} mouse_event;


/*********************/
/**    Functions    **/
/*********************/

/**
 * @brief Starts lunar lander's resources
 */
void start_lunar_lander();

/**
 * @brief Main function that handles the interrupts from the devices, determines the events generated and acts accordingly
 */


void play_lunar_lander();

/**
 * @brief Unallocates the game's resources
 */
void destroy_lunar_lander();

/**
 * @brief Handles the generated events, depending on the current game state
 * 
 */
void event_handler();



/**
 * @} 
 */


#endif /* _SPACE_LANDER_H */
