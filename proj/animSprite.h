#ifndef ANIM_SPRITE_H
#define ANIM_SPRITE_H

#include <lcom/lcf.h>
#include <stdarg.h>     // va_ * macros are defined here
#include "sprite.h"

/**
 * @struct AnimSprite
 * @brief Struct that stores multiple sprites for animation purposes
 * @var sp base sprite that also stores the position
 * @var no. frames per pixmap (animation speed)
 * @var cur_aspeed no. frames left to next change   (current animation speed)
 * @var num_fig number of pixmaps
 * @var cur_fig current pixmap
 * @var map array of pointers to pixmaps
 */
typedef struct {
    Sprite* sp;     // standard sprite
    int aspeed;     // no. frames per pixmap (animation speed)
    int cur_aspeed; // no. frames left to next change   (current animation speed)
    int num_fig;    // number of pixmaps    
    int cur_fig;    // current pixmap
    uint8_t** map;  // array of pointers to pixmaps
} AnimSprite;

/**
 * @brief Create a animSprite object
 * 
 * @param x 
 * @param y 
 * @param xspeed 
 * @param yspeed 
 * @param aspeed 
 * @param no_pic 
 * @param type 
 * @param pic1 
 * @param ... 
 * @return AnimSprite* 
 */
AnimSprite* create_animSprite(int x, int y, int xspeed, int yspeed, int aspeed, uint8_t no_pic, enum xpm_image_type type, xpm_map_t pic1, ...);  // uint8_t* pic[1] <=> xpm_map_t

/**
 * @return 0 if it didn't reach the end of the animation, 1 otherwise
 */ 
int animate_animSprite(AnimSprite* asp);

/**
 * Draws the pixmap directly on the VRAM, by updating the mapped_mem
 */ 
void draw_animated_sprite(AnimSprite* asp);

/**
 * Draws the pixmap on the buffer specified in its argument
 */ 
void draw_animated_sprite_on(AnimSprite* asp, uint8_t* buffer);

/**
 * @brief Destroys an animSprite object
 * 
 * @param asp 
 */
void destroy_animSprite(AnimSprite* asp);

#endif
