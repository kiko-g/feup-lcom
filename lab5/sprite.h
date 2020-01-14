#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

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
 */
void draw_sprite(Sprite* sp);

/**
 * @brief updates the coordinates of a sprite
 * 
 * @param sp a pointer to the sprite to be updated
 */
void update_sprite(Sprite* sp);

/**
 * @brief clears the screen in the location of
 * the sprite, that is, paints it black
 * 
 * @param sp 
 */
void clear_sprite(Sprite* sp);

/**
 * @brief unallocates memory from the sprite
 * 
 * @param sp pointer to the sprite to be destroyed
 */
void destroy_sprite(Sprite *sp);


#endif /* _LCOM_SPRITE_H */
