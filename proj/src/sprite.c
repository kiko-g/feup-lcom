// Lcom's lcf header
#include <lcom/lcf.h>

// Useful header
#include <math.h>

// Group's created headers
#include "sprite.h"
#include "video_card.h"

Sprite* create_sprite(xpm_map_t xpm, int x, int y, int xspeed, int yspeed) {
  // Allocates space for the "object"
  Sprite *sp = (Sprite *) malloc (sizeof(Sprite));

  if(sp == NULL)
    return NULL;

  // Reads the sprite's pixmap
  sp->map = xpm_load(xpm, XPM_8_8_8, &(sp->img));
  
  if(sp->map == NULL) {
    free(sp);
    return NULL;
  }

  // Sets sprite parameters
  sp->x = x;
  sp->y = y;
  sp->xspeed = xspeed;
  sp->yspeed = yspeed;

  return sp;
}


int draw_sprite(Sprite* sp) {

  // Checks if the sprite is inside the screen
  if (sp->x >= (int)get_hres() || sp->y >= (int)get_vres())
    return 0;

  // Color variables
  uint8_t *map = sp->map;
  unsigned num_bytes = get_bytesperpixel();
  uint32_t color;

  // pos_img
  // pos_screen
  
  for (int y = 0; y < sp->img.height; y++) {
    
    for (int x = 0; x < sp->img.width; x++) {
      
      // Calculates the color, depending on the number of bytes per pixel
      color = 0;
      for (unsigned byte = 0; byte < num_bytes; byte++) {
        color += *(map + byte) << byte * 8;
      }
      
      if (draw_pixel(sp->x + x, sp->y + y, color))
        return 1;

      map += num_bytes;
      // pos_screen += num_bytes;
    }

    // pos_screen += hres;
  }

  return 0;
}

int draw_sprite_with_rotation(Sprite* sp, uint16_t angle) 
{
  if (sp->x >= (int)get_hres() || sp->y >= (int)get_vres())
    return 0;

  // Color variables
  uint8_t *map = sp->map;
  unsigned num_bytes = get_bytesperpixel();
  uint32_t color;

  // Angles
  const double angle_radians = degrees_to_radians(angle);
  const double cossine = cos(angle_radians);
  const double sine = sin(angle_radians);

  // Calculates the middle of the sprite
  double rotated_x, rotated_y;
  double translated_x, translated_y;
  double middle_x = (double)(sp->img.width/2.0);  // tirar sp->x
  double middle_y = (double)(sp->img.height/2.0); // tirar sp->y

  for (int y = 0; y < sp->img.height; y++) {
    
    for (int x = 0; x < sp->img.width; x++) {

      // Calculates the color, depending on the number of bytes per pixel
      memcpy(&color, map, num_bytes);

      // Translate point back to its place
      translated_x = (double)(x - middle_x);  // tirar sp->x
      translated_y = (double)(y - middle_y);  // tirar sp->y

      // Rotates in relation to the middle point
      rotated_x = translated_x*cossine - translated_y*sine;
      rotated_y = translated_x*sine + translated_y*cossine;

      // Translate point to the middle of the sprite
      translated_x = floor(rotated_x + middle_x + sp->x); // + sp->x
      translated_y = floor(rotated_y + middle_y + sp->y); // + sp->y

      if (draw_pixel((uint16_t)translated_x, (uint16_t)translated_y, color))
        return 1;

      map += num_bytes;
    }
  }

  return 0;
}


void update_sprite(Sprite* sp) {
  sp->x += sp->xspeed;
  sp->y += sp->yspeed;
}


void clear_sprite(Sprite* sp) {
  vg_draw_rectangle(sp->x, sp->y, sp->img.width, sp->img.height, 0);
}


int clear_sprite_with_cover(Sprite* sp, Sprite* background) {

  uint8_t *background_map = background->map;
  uint32_t background_color;

  unsigned num_bytes = get_bytesperpixel();
  
  for (int y = 0; y < sp->img.height; y++) {
    
    if(sp->y + y >= (int)get_vres())
      continue;

    background_map = background->map + ((sp->y + y)*get_hres()+sp->x)*num_bytes;
    
    for (int x = 0; x < sp->img.width; x++) {

      if(sp->x + x >= (int)get_hres())
        continue;

      memcpy(&background_color, background_map, num_bytes);
    
      if (draw_pixel(sp->x + x, sp->y + y, background_color))
        return 1;        

      background_map += num_bytes;
    }
  }  

  return 0;
}


int clear_sprite_with_rotation(Sprite* sp, uint16_t angle, Sprite* background) {
  if (sp->x >= (int)get_hres() || sp->y >= (int)get_vres())
    return 0;

  // Color variables
  unsigned num_bytes = get_bytesperpixel();
  uint32_t background_color;

  // Angles
  const double angle_radians = degrees_to_radians(angle);
  const double cossine = cos(angle_radians);
  const double sine = sin(angle_radians);

  // Calculates the middle of the sprite
  double rotated_x, rotated_y;
  double translated_x, translated_y;
  double middle_x = (double)(sp->img.width/2.0);  // tirar sp->x
  double middle_y = (double)(sp->img.height/2.0); // tirar sp->y

  for (int y = 0; y < sp->img.height; y++) {
    
    for (int x = 0; x < sp->img.width; x++) {

      // Translate point back to its place
      translated_x = (double)(x - middle_x);  // tirar sp->x
      translated_y = (double)(y - middle_y);  // tirar sp->y

      // Rotates in relation to the middle point
      rotated_x = translated_x*cossine - translated_y*sine;
      rotated_y = translated_x*sine + translated_y*cossine;

      // Translate point to the middle of the sprite
      translated_x = floor(rotated_x + middle_x + sp->x); // + sp->x
      translated_y = floor(rotated_y + middle_y + sp->y); // + sp->y

      if(translated_x >= (int)get_hres())
        continue;

      if(translated_y < 0 || translated_y >= (int)get_vres())
        continue;        

      uint8_t* background_map = background->map + ((uint16_t)translated_y*get_hres()+(uint16_t)translated_x)*num_bytes;  

      // Calculates the color, depending on the number of bytes per pixel
      memcpy(&background_color, background_map, num_bytes);

      if(background_color == xpm_transparency_color(XPM_8_8_8))
        continue;

      if (draw_pixel((uint16_t)translated_x, (uint16_t)translated_y, background_color))
        return 1;

    }
  }  

  return 0;
}


int check_collision_by_size(Sprite *sp, Sprite *obstacle) { //Checks collision with an enemy
  double sprite_center_x = sp->x + (double)sp->img.width / 2;
  double sprite_center_y = sp->y + (double)sp->img.height / 2;

  double obstacle_center_x = obstacle->x + (double)obstacle->img.width / 2;
  double obstacle_center_y = obstacle->y + (double)obstacle->img.height / 2;

  double distance_x = abs(obstacle_center_x - sprite_center_x);
  double distance_y = abs(obstacle_center_y - sprite_center_y);

  double admissible_x = abs( ((double)obstacle->img.width / 2) + ((double) sp->img.width / 2) );
  double admissible_y = abs( ((double)obstacle->img.height / 2) + ((double) sp->img.height / 2) );

  if (distance_x < admissible_x && distance_y < admissible_y) {
    return 1;
  }

  return 0;
}


int check_collision_with_background(Sprite *sp, Sprite *background, uint32_t forbidden_color) {

  // Checks if the sprite is inside the screen
  if (sp->x + sp->img.width > (int)get_hres() || sp->y + sp->img.height > (int)get_vres() || sp->x < 0 || sp->y < 0)
    return 0;

  uint8_t *sp_map = sp->map;
  uint8_t *background_map = background->map;

  uint32_t sp_color;
  uint32_t background_color;

  unsigned num_bytes = get_bytesperpixel();
  
  for (int y = 0; y < sp->img.height; y++) {

    background_map = background->map + ((sp->y + y)*get_hres()+sp->x)*num_bytes;
    
    for (int x = 0; x < sp->img.width; x++) {

      memcpy(&sp_color, sp_map, num_bytes);
      memcpy(&background_color, background_map, num_bytes);
    
      if(sp_color != CHROMA_KEY_GREEN_888 && background_color == forbidden_color)
        return 1;

      sp_map += num_bytes;
      background_map += num_bytes;
    }
  }  

  return 0;
}


void destroy_sprite(Sprite *sp) {
  if(sp == NULL)
    return;

  if(sp->map)
    free(sp->map);
  
  free(sp);
}


double degrees_to_radians(double degrees) {
  return (double)(degrees*M_PI/180);
}
