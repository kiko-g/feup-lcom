#include "asteroid.h"
static time_t t;
// static int x_dif;

// --------------------------------------------
Asteroid* create_asteroid(xpm_map_t xpm)
{
  Asteroid* asteroid = (Asteroid*)malloc(sizeof(Asteroid));
  if(asteroid == NULL) return NULL;

  srand((unsigned) time(&t)); //Intializes random number generator
  int start_x = rand() % get_hres();
  int speed_x = rand() % 2;
  
  asteroid->sprite = create_sprite(xpm, start_x, 0, speed_x, 2);
  asteroid->active = false;

  if(asteroid->sprite == NULL) {
    free(asteroid);
    return NULL;
  }
  
  return asteroid;
}

// --------------------------------------------
void destroy_asteroid(Asteroid* asteroid)
{
  destroy_sprite(asteroid->sprite);
  asteroid->sprite = NULL;

  if(asteroid == NULL) return;
  free(asteroid);
}

// --------------------------------------------
int draw_asteroid(Asteroid* asteroid) {
  return draw_sprite(asteroid->sprite);
}

// --------------------------------------------
int update_asteroid(Asteroid* asteroid) {
  asteroid->sprite->x += asteroid->sprite->xspeed;
  asteroid->sprite->y += asteroid->sprite->yspeed;

  if((asteroid->sprite->y - asteroid->sprite->img.height) > (int)get_vres() || 
     (asteroid->sprite->x - asteroid->sprite->img.width)  > (int)get_hres() ||
     (asteroid->sprite->x + asteroid->sprite->img.width)  < 0)
  {
    asteroid->sprite->xspeed = (rand() % 2) - 0;
    asteroid->sprite->x = rand() % get_hres();
    asteroid->sprite->y = 0;
    asteroid->active = false;
  }

  return 0;
}


