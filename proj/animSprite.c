#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <machine/int86.h>

#include <stdarg.h> // va_ * macros are defined here
#include <stdint.h>
#include <stdio.h>

#include "animSprite.h"
#include "sprite.h"
#include "vbe.h"
#include "vbe_utils.h"
#include "vcMacros.h"

extern uint8_t *mapped_mem;


AnimSprite *create_animSprite(int x, int y, int xspeed, int yspeed, int aspeed, uint8_t no_pic, enum xpm_image_type type, xpm_map_t pic1, ...) {
  AnimSprite *asp = malloc(sizeof(AnimSprite));

  // Create a standard sprite with the first pixmap
  asp->sp = create_sprite(pic1, x, y, xspeed, yspeed, type);
  // Allocate array of pointers to pixmaps
  asp->map = malloc((no_pic) * sizeof(uint8_t *));
  // Initialize the first pixmap
  asp->map[0] = asp->sp->map;

  // Initialize the remainder with the variable arguments
  // iterate over the list of args
  va_list ap;
  va_start(ap, pic1);
  for (int i = 1; i < no_pic; i++) {
    xpm_map_t tmp = va_arg(ap, xpm_map_t); // uint8_t ** <=> xpm_map_t <=> char**
    xpm_image_t img;
    asp->map[i] = xpm_load(tmp, type, &img);
    if ((asp->map[i] == NULL) || (img.width != asp->sp->width) || (img.height != asp->sp->height)) { // this forces all the pixmaps to have the same dimensions
      // Failure: release allocated memory
      for (int j = 1; j < i; j++) {
        free(asp->map[j]); // not sure if it is 'i' or 'j'
      }
      free(asp->map);
      destroy_sprite(asp->sp);
      free(asp);
      va_end(ap);
      printf("Error creating animSprite\n");
      return NULL;
    }
  }
  va_end(ap);
  asp->aspeed = aspeed, asp->cur_aspeed = asp->aspeed, asp->cur_fig = 0, asp->num_fig = no_pic;
  return asp;
}

int animate_animSprite(AnimSprite *asp) {
  // Animate the sprite selection
  if (asp->cur_aspeed == 0) {                         // change pixmap
    asp->cur_fig = (asp->cur_fig + 1) % asp->num_fig; // change current pixmap
    asp->cur_aspeed = asp->aspeed;                    // set current animation speed to max
  }
  else { // only update values
    asp->cur_aspeed -= 1;
  }

  // Move the sprite 
  return animate_sprite(asp->sp);
}

void draw_animated_sprite(AnimSprite *asp) {
  draw_animated_sprite_on(asp, mapped_mem);
}

void draw_animated_sprite_on(AnimSprite *asp, uint8_t *buffer) {
  if ((asp->sp->x < 0) || (asp->sp->y < 0)) {
    printf("Image coordinates are negative\n");
    return;
  }
  uint16_t windowWidth = get_x_res(), windowHeight = get_y_res();

  // parsing data of the img
  uint16_t width = asp->sp->width, height = asp->sp->height; // number of horizontal and vertical pixels
  //uint8_t *bytes = sp->map;   // array with the colors of each pixel

  int byte_color_counter = 0; // stores the index of the current byte

  uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel()); // bytes per pixel
                                                                      //printf("Pixel size: %d", pixel_size);
  for (uint16_t yIdx = asp->sp->y; yIdx < asp->sp->y + height; yIdx++) {
    if (yIdx >= windowHeight) { // yIdx surpasses the bounds
      break;
    }
    for (uint16_t xIdx = asp->sp->x; xIdx < asp->sp->x + width; xIdx++) {
      if (xIdx >= windowWidth) { // if xIdx surpasses the bounds
        break;
      }
      uint32_t y_coord = yIdx * windowWidth * pixel_size; // number of lines to travel
      uint32_t x_coord = xIdx * pixel_size;

      uint32_t color;
      memcpy(&color, &(asp->map[asp->cur_fig][byte_color_counter]), pixel_size); // &sp->map[byte_color_counter]

      if (color != TRANSPARENCY_COLOR_8_8_8_8) {
        memcpy(buffer + y_coord + x_coord, &(asp->map[asp->cur_fig][byte_color_counter]), pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
      }
      
      byte_color_counter += pixel_size;
    }
  }
}

void destroy_animSprite(AnimSprite *asp) {
  for (int i = 1; i < asp->num_fig; i++) {
    if (asp->map[i] != NULL) {
      free(asp->map[i]);
    }
  }
  free(asp->map);
  if (asp->sp != NULL) {
    destroy_sprite(asp->sp);
  }
  free(asp);
}
