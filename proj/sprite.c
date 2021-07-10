#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <machine/int86.h>

#include <stdint.h>
#include <stdio.h>

#include "game.h"
#include "sprite.h"
#include "vbe.h"
#include "vbe_utils.h"
#include "vcMacros.h"

extern uint8_t *mapped_mem, mapped_mem2;

Sprite *create_sprite(xpm_map_t xpm, int x, int y, int xspeed, int yspeed, enum xpm_image_type type) {
  //allocate space for the object
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
  xpm_image_t img;

  if (sp == NULL) {
    return NULL;
  }

  //read the sprite map
  sp->map = xpm_load(xpm, type, &img);

  /* POSSIBLY CHANGE TO THIS
  xpm_load(xpm, type, &img);
  sp->map = img.bytes;  */

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
  int nextX = sp->x + sp->xspeed;
  int nextY = sp->y + sp->yspeed;
  if ((nextX + sp->width < get_x_res()) && (nextX >= 0)) { // 20 is just a threshold to not go out
    sp->x = nextX;
  }
  else {
    return 1;
  }
  if ((nextY + sp->height < get_y_res() - 20) && (nextY >= 0)) {
    sp->y = nextY;
  }
  else {
    return 1;
  }
  // this is not moving if the move goes beyond the screen
  return 0;
}

void draw_sprite(Sprite *sp) {
  draw_sprite_on(sp, mapped_mem);
}

void draw_sprite_on(Sprite *sp, uint8_t *buffer) {
  if ((sp->x < 0) || (sp->y < 0)) {
    printf("Image coordinates are negative\n");
    return;
  }

  uint16_t windowWidth = get_x_res(), windowHeight = get_y_res();

  // parsing data of the img
  uint16_t width = sp->width, height = sp->height; // number of horizontal and vertical pixels
  //uint8_t *bytes = sp->map;   // array with the colors of each pixel

  int byte_color_counter = 0; // stores the index of the current byte

  uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel()); // bytes per pixel
  //printf("Pixel size: %d", pixel_size);

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

      uint32_t color;
      memcpy(&color, &sp->map[byte_color_counter], pixel_size);

      if (color != TRANSPARENCY_COLOR_8_8_8_8) {
        memcpy(buffer + y_coord + x_coord, &sp->map[byte_color_counter], pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
      }

      byte_color_counter += pixel_size;
    }
  }
}

void draw_sprite_without_transparency(Sprite *sp, uint8_t *buffer) {
  if ((sp->x < 0) || (sp->y < 0)) {
    printf("Image coordinates are negative\n");
    return;
  }

  uint16_t windowWidth = get_x_res(), windowHeight = get_y_res();

  // parsing data of the img
  uint16_t width = sp->width, height = sp->height; // number of horizontal and vertical pixels
  //uint8_t *bytes = sp->map;   // array with the colors of each pixel

  int byte_color_counter = 0; // stores the index of the current byte

  uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel()); // bytes per pixel
  //printf("Pixel size: %d", pixel_size);

  for (uint16_t yIdx = sp->y; yIdx < sp->y + height; yIdx++) {
    if (yIdx >= windowHeight) { // yIdx surpasses the bounds
      break;
    }
    uint32_t y_coord = yIdx * windowWidth * pixel_size; // number of lines to travel
    uint32_t x_coord = sp->x * pixel_size;
    memcpy(buffer + y_coord + x_coord, &sp->map[byte_color_counter], pixel_size * width);

    byte_color_counter += pixel_size * width;
  }
}

void draw_background(Sprite *sp, uint8_t *buffer) {
  uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel()); // bytes per pixel
  memcpy(buffer, sp->map, get_y_res() * get_x_res() * pixel_size);
}

void copyPosition(Sprite *sp1, Sprite *sp2) {
  sp2->x = sp1->x;
  sp2->y = sp1->y;
}

/*    // Cannot clear a single sprite because of page flipping
void clear_sprite_on(Sprite *sp1, Sprite *sp2, uint8_t *buffer, int prevX, int prevY) {
  if ((prevX < 0) || (prevY < 0)) {
    printf("Image coordinates are negative\n");
    return;
  }

  uint16_t windowWidth = get_x_res(), windowHeight = get_y_res();

  // parsing data of the img
  uint16_t width = sp1->width, height = sp1->height; // number of horizontal and vertical pixels
  //uint8_t *bytes = sp->map;   // array with the colors of each pixel


  uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel()); // bytes per pixel
  //printf("Pixel size: %d", pixel_size);

  for (uint16_t yIdx = prevY; yIdx < prevY + height; yIdx++) {
    if (yIdx >= windowHeight) { // yIdx surpasses the bounds
      break;
    }
    for (uint16_t xIdx = prevX; xIdx < prevX + width; xIdx++) {
      if (xIdx >= windowWidth) { // if xIdx surpasses the bounds
        break;
      }
      uint32_t y_coord = yIdx * windowWidth * pixel_size; // number of lines to travel
      uint32_t x_coord = xIdx * pixel_size;

      //uint32_t color;
      //memcpy(&color, &sp2->map[byte_color_counter], pixel_size);

      //if (color != TRANSPARENCY_COLOR_8_8_8_8) {
      memcpy(buffer + y_coord + x_coord, sp2->map + y_coord + x_coord, pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
      //}

    }
  }
}*/

/*
void clears_last_movement_on(Sprite *sp, uint16_t windowWidth, uint16_t windowHeight, uint8_t *buffer, Sprite *background) {

  // parsing data of the img
  // uint16_t width = sp->width, height = sp->height; // number of horizontal and vertical pixels
  //uint8_t black = 0x00;
  uint32_t color;
  uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel()); // bytes per pixel

  for (uint16_t yIdx = sp->y - abs(sp->yspeed); yIdx < sp->y; yIdx++) {
    if (yIdx < 0) {
      yIdx = -1;
      continue;
    }
    if (yIdx >= windowHeight) { // yIdx surpasses the bounds
      break;
    }
    for (uint16_t xIdx = sp->x - sp->xspeed; xIdx < sp->x; xIdx++) {
      if (xIdx < 0) {
        xIdx = -1;
        continue;
      }
      if (xIdx >= windowWidth) {
        break;
      }
      uint32_t y_coord = yIdx * windowWidth * pixel_size; // number of lines to travel
      uint32_t x_coord = xIdx * pixel_size;
      memcpy(&color, &background->map[y_coord + x_coord], pixel_size);
      memcpy(buffer + y_coord + x_coord, &color, pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
    }
  }
} */
