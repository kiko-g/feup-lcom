#ifndef _LCOM_SPACECRAFT_H_
#define _LCOM_SPACECRAFT_H_

// Lcom's lcf header
#include <lcom/lcf.h>

// Group's created headers
#include "sprite.h"


/** @defgroup spacecraft spacecraft
 * @{
 *
 * File for dealing with the spacecraft (main entity of space lander).
 */



/**
 * @brief Struct that holds information regarding the spacecraft
 * 
 * @param angle The angle, in degrees, that the spacecraft is forming (0 degreees is pointin straight up)
 * @param boost True if the spacecraft is boosting, false if it isn't
 * @param rotate 0 if the spacecraft isn't rotating, 1 if it's rotating anti-clockwise and 2 if it's rotating clockwise
 * @param x The spacecraft's position in the x-axis, using a 'double' for greater precision
 * @param y The spacecraft's position in the y-axis, using a 'double' for greater precision
 * @param speed_x The spacecraft's position in the x-axis, using a 'double' for greater precision
 * @param speed_y The spacecraft's position in the y-axis, using a 'double' for greater precision
 * @param acceleration_x The spacecraft's position in the x-axis, using a 'double' for greater precision
 * @param acceleration_y The spacecraft's position in the y-axis, using a 'double' for greater precision  
 */
typedef struct {
  Sprite* sprite;
  uint16_t angle;
  bool boost;
  uint8_t rotate;
  double x, y;
  double speed_x, speed_y;
  double acceleration_x;
  double acceleration_y;
} Spacecraft;

/**
 * @brief Creates a spacecraft object
 * 
 * @param xpm An x-pixmap of the spacecraft
 * @param x The spacecraft's initial x coordinate
 * @param y The spacecraft's initial y coordinate
 * @return Spacecraft* the created spacecraft
 */
Spacecraft* create_spacecraft(xpm_map_t xpm, int x, int y);

/**
 * @brief Frees the spacecraft object
 * 
 * @param spacecraft Pointer to spacecraft object to be destroyed
 */
void destroy_spacecraft(Spacecraft *spacecraft);

/**
 * @brief Updates the state of the spacecraft
 * 
 * @param spacecraft Pointer to a spacecraft object
 * @return int 0 is successful, anything else if otherwise
 */
int update_spacecraft(Spacecraft* spacecraft);

/**
 * @brief Updates the movement of the spacecraft
 * 
 * @param spacecraft Pointer to a spacecraft object
 * @param new_acc_x The new acceleration in x (depends on the angle)
 * @param new_acc_y The new acceleration in y (depends on the angle)
 * @return int 0 is successful, anything else if otherwise
 */
int update_movement(Spacecraft* spacecraft, double new_acc_x, double new_acc_y);

/**
 * @brief Draws the spacecraft on the screen
 * 
 * @param spacecraft Pointer to a spacecraft object
 * @return int 0 if successful, anything else if otherwise
 */
int draw_spacecraft(Spacecraft* spacecraft);

/**
 * @brief Clears the spacecraft from the screen
 * 
 * @param spacecraft Pointer to a spacecraft object
 * @return int 0 if successful, anything else if otherwise
 */
int clear_spacecraft(Spacecraft* spacecraft, Sprite* background);

/**
 * @brief Sets the spacecraft as rotating to the right
 * 
 * @param spacecraft Pointer to a spacecraft object
 */
void rotate_right(Spacecraft* spacecraft);

/**
 * @brief Sets the spacecraft as rotating to the left
 * 
 * @param spacecraft Pointer to a spacecraft object
 */
void rotate_left(Spacecraft* spacecraft);

/**
 * @brief Stops the spacecraft's rotation
 * 
 * @param spacecraft Pointer to a spacecraft object
 */
void stop_rotating(Spacecraft* spacecraft);

/**
 * @brief Spacecraft starts boosting
 * 
 * @param spacecraft Pointer to a spacecraft object
 */
void start_boost(Spacecraft* spacecraft);

/**
 * @brief Spacecraft stops boosting
 * 
 * @param spacecraft Pointer to a spacecraft object
 */
void stop_boost(Spacecraft* spacecraft);

/**
 * @brief Checks if the spacecraft has landed
 * 
 * @param spacecraft Pointer to a spacecraft object
 * @param level Sprite of the current level
 * @return true If the spacecraft has landed on the right spot
 * @return false If otherwise
 */ 
bool has_landed(Spacecraft* spacecraft, Sprite* level);

/**
 * @brief Checks if the spacecraft colided with the terrain
 * 
 * @param spacecraft Pointer to a spacecraft object
 * @param level Sprite of the current level
 * @param ground_color Color of the ground
 * @return true If the spacecraft colided
 * @return false If the spacecraft is safe
 */
bool detect_colision_with_terrain(Spacecraft* spacecraft, Sprite* level, uint32_t ground_color);


/**
 * @brief Checks if the spacecraft colided with one of the projectiles
 * 
 * @param spacecraft Pointer to a spacecraft object
 * @param projectiles Sprite array of projectiles
 * @param num_enemies The number of enemies
 * @return true If the spacecraft colided
 * @return false If the spacecraft is safe
 */
bool detect_colision_with_projectiles(Spacecraft* spacecraft, Sprite* projectiles[], uint8_t num_enemies);



/**
 * @} 
 */

#endif /* _LCOM_SPACECRAFT_H */
