#ifndef SPRITE_H
#define SPRITE_H


/** @struct Sprite
 *  @brief Struct relativa à criação de sprites 
 *  @var x x coordinate of the sprite
 *  @var y y cooordinate of the sprite
 *  @var width width of the sprite
 *  @var height height of the sprite
 *  @var xspeed speed on the x axis
 *  @var yspeed speed on the y axis
 *  @var map array that saves the pixmap
 *  @var type color mode
 */
typedef struct {
  int x, y;           // current position
  int width, height;  // dimensions
  int xspeed, yspeed; // current speed
  uint8_t *map;          // the pixmap
  enum xpm_image_type type;
} Sprite;

/**
 * @brief Create a sprite object
 * 
 * @param xpm pixmap
 * @param x x coord
 * @param y y coord
 * @param xspeed speed on the x axis
 * @param yspeed speed on the y axis
 * @param type color mode
 * @return Sprite* 
 */
Sprite* create_sprite(xpm_map_t xpm, int x, int y, int xspeed, int yspeed, enum xpm_image_type type);

/**
 * @brief destroys a sprite object
 * 
 * @param sp sprite to be destroyed
 */
void destroy_sprite(Sprite* sp);

/**
 * @return 0 if it didn't reach the end of the animation, 1 otherwise
 */ 
int animate_sprite(Sprite* sp);

/**
 * Draws the pixmap directly on the VRAM, by updating the mapped_mem
 */ 
void draw_sprite(Sprite* sp);

/**
 * Draws the pixmap on the buffer specified in its argument
 */ 
void draw_sprite_on(Sprite *sp, uint8_t* buffer);

/**
 * Draws a pixmap faster, by using memcpy on each row (no need to check transparency)
 */ 
void draw_sprite_without_transparency(Sprite *sp, uint8_t *buffer);

/**
 * Draws a background by simply copying the whole buffer to VRAM 
 */ 
void draw_background(Sprite *sp, uint8_t *buffer);

/**
 * Clears the pixels that are not going to be used from the previous iteration, on the buffer specified in its argument
 */ 
void clears_last_movement_on(Sprite* sp, uint16_t windowWidth, uint16_t windowHeight, uint8_t* buffer, Sprite* background);

/**
 * Copies sp1 pos to sp2
 */
void copyPosition(Sprite* sp1, Sprite* sp2);

//void clear_sprite_on(Sprite *sp1, Sprite *sp2, uint8_t *buffer, float prevX, float prevY);


#endif 
