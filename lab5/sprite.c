#include <lcom/lcf.h>
#include "sprite.h"
#include "video_card.h"

Sprite* create_sprite(xpm_map_t xpm, int x, int y,int xspeed, int yspeed) {
  // allocates space for the "object"
  Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));

  if(sp == NULL)
    return NULL;

  // reads the sprite's pixmap
  sp->map = xpm_load(xpm, XPM_INDEXED, &(sp->img));
  
  if(sp->map == NULL) {
    free(sp);
    return NULL;
  }

  // sets sprite parameters
  sp->x = x;
  sp->y = y;
  sp->xspeed = xspeed;
  sp->yspeed = yspeed;

  return sp;
}


void draw_sprite(Sprite* sp) {
  draw_pixmap(sp->map, sp->img, sp->x, sp->y);
}


void update_sprite(Sprite* sp) {
  sp->x += sp->xspeed;
  sp->y += sp->yspeed;
}


void clear_sprite(Sprite* sp) {
  vg_draw_rectangle(sp->x, sp->y, sp->img.width, sp->img.height, 0);
}


void destroy_sprite(Sprite *sp) {
  if(sp == NULL)
    return;

  if(sp->map)
    free(sp->map);
  
  free(sp);
}
