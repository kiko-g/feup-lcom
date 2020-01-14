// Group's created headers
#include "spacecraft.h"
#include "video_card.h"
// Useful header
#include <math.h> 


// A static variable that holds the value of gravity's acceleration
static double gravity = 0.075;

// A static variable that holds the value of the boost's force 
static double boost_force = -0.25;

// Maximum velocity for the ship, so that it's more controlable
static double max_speed = 0.25;

// Air friction that slows down the ship in the x direction
static double air_friction = 0.02;




Spacecraft* create_spacecraft(xpm_map_t xpm, int x, int y) 
{
  // Allocates space for the "object"
  Spacecraft* spacecraft = (Spacecraft*) malloc(sizeof(Spacecraft));
  if(spacecraft == NULL) return NULL;

  // Creates a sprite with no speed
  spacecraft->sprite = create_sprite(xpm, x, y, 0, 0); 
  
  if(spacecraft->sprite == NULL) {
    free(spacecraft);
    return NULL;
  }

  // Sets the spacecraft's parameters
  spacecraft->x = (double)x;
  spacecraft->y = (double)y;

  spacecraft->speed_x = 0;
  spacecraft->speed_y = 0;

  spacecraft->acceleration_x = 0;
  spacecraft->acceleration_y = gravity;

  spacecraft->boost = 0;
  spacecraft->rotate = 0;
  spacecraft->angle = 0;  

  return spacecraft;
}


void destroy_spacecraft(Spacecraft *spacecraft) 
{
  destroy_sprite(spacecraft->sprite);
  spacecraft->sprite = NULL;

  if(spacecraft == NULL) return;
  free(spacecraft);
}


int update_spacecraft(Spacecraft* spacecraft) 
{  
  // Updates the angle of the spacecraft
  switch(spacecraft->rotate) 
  {
    case 0:
      break;
    
    case 1:
      if(spacecraft->angle <= 0) {
        spacecraft->angle = 359;
      }
      else {
        spacecraft->angle = (spacecraft->angle - 2);
      }
      break;

    case 2:
      if(spacecraft->angle >= 359) {
        spacecraft->angle = 0;
      }
      else {
        spacecraft->angle = (spacecraft->angle + 2);
      }
      break;

    default:
      return 1;
  }

  // Updates the velocity of the spacecraft
  if(spacecraft->boost) {
    double new_acc_x = (boost_force * -sin(degrees_to_radians(spacecraft->angle)));
    double new_acc_y = (boost_force * cos(degrees_to_radians(spacecraft->angle)) + gravity);
    update_movement(spacecraft, new_acc_x, new_acc_y); 
  }
  else { 
    update_movement(spacecraft, 0, gravity); 
  }

  return 0;
}


int update_movement(Spacecraft* spacecraft, double new_acc_x, double new_acc_y) 
{
  // Sets the acceleration
  spacecraft->acceleration_x = new_acc_x;
  spacecraft->acceleration_y = new_acc_y;

  bool falling_over_max = !spacecraft->boost && spacecraft->speed_y < max_speed;  
  bool boosting_over_max = spacecraft->boost && (spacecraft->speed_x > max_speed || spacecraft->speed_y > max_speed);

  // Updates the speed of the spacecraft
  if(!boosting_over_max || !falling_over_max) {
    spacecraft->speed_x += spacecraft->acceleration_x;
    spacecraft->speed_y += spacecraft->acceleration_y;
  }

  // Applies air friction in the x direction
  if(spacecraft->speed_x > 0) 
    spacecraft->speed_x -= air_friction;
  else if(spacecraft->speed_x < 0) 
    spacecraft->speed_x += air_friction;

  // Resets speed if it's getting out of bounds
  if(spacecraft->x < 0) {
    spacecraft->speed_x = 0;
    spacecraft->speed_y = 0;
    spacecraft->x = 0;
  } 
  if(spacecraft->y < 0) {
    spacecraft->speed_x = 0;
    spacecraft->speed_y = 0;
    spacecraft->y = 0;
  } 
  if(spacecraft->x + spacecraft->sprite->img.width > (double)get_hres()) {
    spacecraft->speed_x = 0;
    spacecraft->speed_y = 0;
    spacecraft->x = (double)(get_hres() - spacecraft->sprite->img.width);
  } 
  if(spacecraft->y + spacecraft->sprite->img.height > (double)get_vres()) {
    spacecraft->speed_x = 0;
    spacecraft->speed_y = 0;
    spacecraft->y = (double)(get_vres() - spacecraft->sprite->img.height);    
  } 

  
  // Updates the position of the spacecraft
  spacecraft->x += spacecraft->speed_x;
  spacecraft->y += spacecraft->speed_y;

  // Sets the sprite's coordinates
  spacecraft->sprite->x = (int)spacecraft->x;
  spacecraft->sprite->y = (int)spacecraft->y;

  return 0;
}


void rotate_right(Spacecraft* spacecraft) {
  spacecraft->rotate = 2;
}


void rotate_left(Spacecraft* spacecraft) {
  spacecraft->rotate = 1;
}


void stop_rotating(Spacecraft* spacecraft) {
  spacecraft->rotate = 0;
}


void start_boost(Spacecraft* spacecraft) {
  spacecraft->boost = true;
}


void stop_boost(Spacecraft* spacecraft) {
  spacecraft->boost = false;
}


int draw_spacecraft(Spacecraft* spacecraft) {
  return draw_sprite_with_rotation(spacecraft->sprite, spacecraft->angle);
}


int clear_spacecraft(Spacecraft* spacecraft, Sprite* background) {
  //return clear_sprite_with_cover(spacecraft->sprite, background);
  return clear_sprite_with_rotation(spacecraft->sprite, spacecraft->angle, background);
}


bool has_landed(Spacecraft* spacecraft, Sprite* level) {
  if(check_collision_with_background(spacecraft->sprite, level, 0x00FF00))
    return true;

  return false;
}


bool detect_colision_with_terrain(Spacecraft* spacecraft, Sprite* level, uint32_t ground_color){
  if(check_collision_with_background(spacecraft->sprite, level, ground_color))
    return true;

  return false;
}


//TODO: implement this function
bool detect_colision_with_projectiles(Spacecraft* spacecraft, Sprite* projectiles[], uint8_t num_enemies) {
  printf("Not completed\n");
  return false;
}
