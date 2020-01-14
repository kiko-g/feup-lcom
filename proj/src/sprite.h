#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

/** @defgroup sprite sprite
 * @{
 *
 * File for sprite type, holding functions and types to manage sprites
 */


/**
 * @brief struct Sprite
 * 
 * @param x the x coordinate of the current position
 * @param y the y coordinate of thecurrent position
 * @param xspeed the speed in the x axis
 * @param yspeed the speed in the y axis
 * @param img struct that stores the information about the image to display
 * @param map address of the allocated memory to where the image was read
 */
typedef struct {
  int x, y;
  int xspeed, yspeed;
  xpm_image_t img;
  uint8_t *map;
} Sprite;



/**
 * @brief Creates a sprite object
 * 
 * @param xpm an xpm-like pixamp to display
 * @param x the x coordinate of the starting position
 * @param y the y coordinate of the starting position
 * @param xspeed the speed in the x axis
 * @param yspeed the speed in the y axis
 * @return Sprite*, created sprite. NULL when xpm is invalid
 */
Sprite* create_sprite(xpm_map_t xpm, int x, int y, int xspeed, int yspeed);


/**
 * @brief draws a sprite on the screen
 * 
 * @param sp a pointer to the sprite to be drawn
 * @return int, 0 if successful, anything else if otherwise
 */
int draw_sprite(Sprite* sp);


/**
 * @brief draws a sprite on the screen with rotation
 * 
 * @param sp a pointer to the sprite to be drawn
 * @param angle the current angle (integer) in degrees (from 0 to 359)
 * @return int, 0 if successful, anything else if otherwise
 */
int draw_sprite_with_rotation(Sprite* sp, uint16_t angle);


/**
 * @brief Clears the screen in the location of a normal sprite, painting the background in its place
 * 
 * @param sp a pointer to the sprite to be cleared
 * @param background a pointer to the sprite to be drawn
 * @return int 
 */
int clear_sprite_with_cover(Sprite* sp, Sprite* background);


/**
 * @brief Clears the screen in the location of a rotating sprite, painting the background in its place
 * 
 * @param sp a pointer to the sprite to be cleared
 * @param angle angle of the sprite
 * @param background a pointer to the sprite to be drawn
 * @return int 
 */
int clear_sprite_with_rotation(Sprite* sp, uint16_t angle, Sprite* background);


/**
 * @brief Updates the coordinates of a sprite
 * 
 * @param sp a pointer to the sprite to be updated
 */
void update_sprite(Sprite* sp);


/**
 * @brief clears the screen in the location of the sprite, that is, paints it black
 * 
 * @param sp a pointer to the sprite to be cleared
 */
void clear_sprite(Sprite* sp);


/**
 * @brief checks the collision between two sprites, regarding their size
 * 
 * @param sp main sprite
 * @param obstacle obstacle's sprite
 * @return int 0 if there were no collisions, 1 if otherwise
 */ 
int check_collision_by_size(Sprite *sp, Sprite *obstacle);



/**
 * @brief checks the collision between two sprites, one of them being the groind (uses ground color)
 * 
 * @param sp main sprite
 * @param bakground background sprite 
 * @param forbidden_color the color that will determine the collision (ground color)
 * @return int 0 if there were no collisions, 1 if otherwise
 */ 
int check_collision_with_background(Sprite *sp, Sprite *background, uint32_t forbidden_color);


/**
 * @brief unallocates memory from the sprite
 * 
 * @param sp pointer to the sprite to be destroyed
 */
void destroy_sprite(Sprite *sp);


/**
 * @brief converts an angle in degrees to radians
 * 
 * @param degrees angle in degrees
 * @return double angle in radians
 */
double degrees_to_radians(double degrees);

/**
 * @} 
 */


#endif /* _LCOM_SPRITE_H */

