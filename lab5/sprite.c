#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <machine/int86.h>

#include <stdint.h>
#include <stdio.h>

#include "sprite.h"
#include "vbe.h"
#include "vcMacros.h"

extern uint8_t *mapped_mem;

Sprite *create_sprite(xpm_map_t xpm, int x, int y, int xspeed, int yspeed, enum xpm_image_type type) {
  //allocate space for the object
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));

  xpm_image_t img;

  if (sp == NULL) {
    return NULL;
  }

  //read the sprite map
  sp->map = xpm_load(xpm, type, &img);
  if (sp->map == NULL) {
    free(sp);
    return NULL;
  }

  sp->width = img.width, sp->height = img.height, sp->x = x, sp->y = y, sp->xspeed = xspeed, sp->yspeed = yspeed, sp->type = type;
  return sp;
}

void destroy_sprite(Sprite *sp) {
  if (sp == NULL) {
    return;
  }
  if (sp->map) {
    free(sp->map);
  }
  free(sp);
  sp = NULL;
}

int animate_sprite(Sprite *sp) {
  return 0;
}

void draw_sprite(Sprite *sp, uint16_t windowWidth, uint16_t windowHeight) {
  draw_sprite_on(sp, windowWidth, windowHeight, mapped_mem);
}

void draw_sprite_on(Sprite *sp, uint16_t windowWidth, uint16_t windowHeight, uint8_t *buffer) {
  if ((sp->x < 0) || (sp->y < 0)) {
    printf("Image coordinates are negative\n");
    return;
  }

  // parsing data of the img
  uint16_t width = sp->width, height = sp->height; // number of horizontal and vertical pixels
  //size_t size = (*img).size;                             // width * height * bytes_per_pixel
  uint8_t *bytes = sp->map;                                           // array with the colors of each pixel (1 byte per pixel)
  int byte_color_counter = 0;                                         // stores the index of the current byte

  uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel()); // bytes per pixel
  printf("Pixel size: %d", pixel_size);
  
  for (uint16_t yIdx = sp->y; yIdx < sp->y + height; yIdx++) {
    if (yIdx >= windowHeight) { // yIdx surpasses the bounds
      break;
    }
    for (uint16_t xIdx = sp->x; xIdx < sp->x + width; xIdx++) {
      if (xIdx >= windowWidth) { // if xIdx surpasses the bounds
        break;
      }
      uint32_t y_coord = yIdx * windowWidth * pixel_size; // number of lines to travel
      uint32_t x_coord = xIdx * pixel_size;
      memcpy(buffer + y_coord + x_coord, &bytes[byte_color_counter], pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
      byte_color_counter += pixel_size;
    }
  }
}

void clears_last_movement_on(Sprite *sp, uint16_t windowWidth, uint16_t windowHeight, uint8_t *buffer) {

  // parsing data of the img
  uint16_t width = sp->width, height = sp->height; // number of horizontal and vertical pixels
  uint8_t black = 0x00;
  if (sp->type == XPM_INDEXED) {      // 1 byte per pixel
    uint8_t pixel_size = BPP_INDEXED; // bytes per pixel
    if (sp->yspeed != 0) {            // vertical movement
      for (uint16_t yIdx = sp->y - abs(sp->yspeed); yIdx < sp->y; yIdx++) {
        if (yIdx < 0) {
          yIdx = -1;
          continue;
        }
        if (yIdx >= windowHeight) { // yIdx surpasses the bounds
          break;
        }
        for (uint16_t xIdx = sp->x; xIdx < sp->x + width; xIdx++) {
          if (xIdx < 0) {
            xIdx = -1;
            continue;
          }
          if (xIdx >= windowWidth) {
            break;
          }
          uint32_t y_coord = yIdx * windowWidth * pixel_size; // number of lines to travel
          uint32_t x_coord = xIdx * pixel_size;
          memcpy(buffer + y_coord + x_coord, &black, pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
        }
      }
    }
    else if (sp->xspeed != 0) { // horizontal movement
      for (uint16_t yIdx = sp->y; yIdx < sp->y + height; yIdx++) {
        if (yIdx >= windowHeight) { // yIdx surpasses the bounds
          break;
        }
        for (uint16_t xIdx = sp->x - abs(sp->xspeed); xIdx < sp->x; xIdx++) {
          if (xIdx < 0) {
            continue;
          }
          if (xIdx >= windowWidth) {
            break;
          }
          uint32_t y_coord = yIdx * windowWidth * pixel_size; // number of lines to travel
          uint32_t x_coord = xIdx * pixel_size;
          memcpy(buffer + y_coord + x_coord, &black, pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
        }
      }
    }
  }
}
