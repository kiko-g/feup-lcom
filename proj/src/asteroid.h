#ifndef _LCOM_ASTEROID_H_
#define _LCOM_ASTEROID_H_

#include <lcom/lcf.h>
#include <math.h>
#include "sprite.h"
#include "video_card.h"


/** @defgroup asteroid asteroid
 * @{
 *
 * File for dealing with asteroid object
 */



typedef struct {
  Sprite* sprite;
  bool active;
} Asteroid;

/** 
 * @brief Creates a asteroid object
 * 
 * @param xpm asteroid image
 * @return Asteroid object
 */
Asteroid* create_asteroid(xpm_map_t xpm);

/** 
 * @brief Frees the memory held by asteroid object
 * 
 * @param asteroid asteroid object to be freed
 */
void destroy_asteroid(Asteroid* asteroid);

/** 
 * @brief draws asteroid sprite
 * 
 * @param asteroid asteroid object to be drawn
 * @return 0 upon success, 1 otherwise
 */
int draw_asteroid(Asteroid* asteroid);

/** 
 * @brief Updates the state of the asteroid
 * 
 * @param spacecraft Pointer to the asteroid object
 * @return 0 upon success, non zero otherwise
 */

int update_asteroid(Asteroid* asteroid);


/**
 * @} 
 */


#endif /* _LCOM_ASTEROID_H */

