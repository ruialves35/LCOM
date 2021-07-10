#ifndef SPRITE_H
#define SPRITE_H


typedef struct {
  int x, y;           // current position
  int width, height;  // dimensions
  int xspeed, yspeed; // current speed
  uint8_t *map;          // the pixmap
  enum xpm_image_type type;
} Sprite;

Sprite* create_sprite(xpm_map_t xpm, int x, int y, int xspeed, int yspeed, enum xpm_image_type type);

void destroy_sprite(Sprite* sp);

/**
 * @return 0 if it didn't reach the end of the animation, 1 otherwise
 */ 
int animate_sprite(Sprite* sp);

/**
 * Draws the pixmap directly on the VRAM, by updating the mapped_mem
 */ 
void draw_sprite(Sprite* sp, uint16_t windowWidth, uint16_t windowHeight);

/**
 * Draws the pixmap on the buffer specified in its argument
 */ 
void draw_sprite_on(Sprite *sp, uint16_t windowWidth, uint16_t windowHeight, uint8_t* buffer);

/**
 * Clears the pixels that are not going to be used from the previous iteration, on the buffer specified in its argument
 */ 
void clears_last_movement_on(Sprite* sp, uint16_t windowWidth, uint16_t windowHeight, uint8_t* buffer);



#endif 
