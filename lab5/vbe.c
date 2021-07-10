#include <lcom/lab5.h>
#include <lcom/lcf.h>
#include <machine/int86.h>

#include <stdint.h>
#include <stdio.h>

#include "vbe.h"
#include "vbe_utils.h"
#include "vcMacros.h"

static vbe_mode_info_t vbe_mode_info;
uint8_t *mapped_mem;

int(set_video_mode)(uint16_t mode) {
  reg86_t reg86;

  memset(&reg86, 0, sizeof(reg86)); // zero the structure

  reg86.intno = VBE_BIOS_CALL; /*BIOS video services */
  reg86.ah = VBE_MODE;
  reg86.al = SET_VBE_MODE;
  //reg86.ax = COMPLETE_SET_VBE_MODE;           /* 4f followed by the function (set vbe mode)  */
  reg86.bx = SET_LINEAR_MODE | mode; /* set mode */

  /* Make the BIOS call  */
  if (sys_int86(&reg86) != OK) {
    printf("\tvideo_test_init(): sys_int86() failed \n");
    return 1;
  }

  return 0;
}

int(init_graphic_mode)(uint16_t mode) {
  // Initialize lower memory region / No need to invoke anymore
  /*
  if (lm_init(true) == NULL) {   
    printf("(%s) Could not run lm_init\n", __func__);
    return NULL;
  }    */

  // Get information about vbe mode
  if (vbe_get_mode_info2(mode, &vbe_mode_info) != 0) { // need to create my own function afterwards
    printf("Error on vbe_get_mode_info\n");
    return 1;
  }

  struct minix_mem_range mr;                                                                                   // physical mem. range
  unsigned int vram_base = vbe_mode_info.PhysBasePtr;                                                          // VRAM's physical addresss
  unsigned int vram_size = vbe_mode_info.XResolution * vbe_mode_info.YResolution * vbe_mode_info.BitsPerPixel; // VRAM's size, but you can use the frame-buffer size, instead

  void *video_mem; // frame buffer VM address

  // Allow memory mapping

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  int res = 0;
  if (OK != (res = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) { // get permission to map virtual mem
    panic("Error on sys_privctl\n");
    return 1;
  }

  // Map memory

  video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size); // Process (virtual) address to which VRAM is mapped
  if (video_mem == MAP_FAILED) {
    panic("Couldn't map video memory\n");
    return 1;
  }

  mapped_mem = video_mem; //store the mapped memory pointer in mapped_mem (convert from void* to uint8_t*)

  // Set video mode
  if (set_video_mode(mode) != 0) {
    printf("Error on set_video_mode\n");
    return 1;
  }

  return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  uint16_t currY = y;

  // Check if starting point is invalid
  if (x >= get_x_res() || y >= get_y_res() || x < 0 || y < 0) {
    printf("Rectangle starting point is out of bounds\n");
    return 1;
  }

  // Draw horizontal lines

  while ((currY < y + height) && (currY < get_y_res())) { // check if currY surpasses the limit, // need to use < instead of <= since it starts counting on 0
    if (vg_draw_hline(x, currY, width, color) != 0) {
      printf("Error on vg_draw_hline\n");
    }
    currY++;
  }

  return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  //printf("1");
  if (get_memory_model() == DIRECT_COLOR_MODE) { // Direct Color
    // calculate size in bytes each pixel occupies
    uint8_t pixel_size = calculate_size_in_bytes(get_bits_per_pixel());
    //printf("2");
    for (uint16_t currX = x; currX < x + len; currX++) { // need to use < instead of <= since it starts counting on 0
      if (currX >= get_x_res()) {                        // reached the end of the line, so it stops drawing
        return 0;
      }
      //printf("%u, %u\n", currX, y);
      uint32_t y_coord = y * get_x_res() * pixel_size; // number of lines to travel
      uint32_t x_coord = currX * pixel_size;
      memcpy(mapped_mem + y_coord + x_coord, &color, pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
    }
  }
  else if (get_memory_model() == INDEXED_COLOR_MODE) {
    //printf("3");
    memset(mapped_mem + y * get_x_res() + x, (uint8_t) color, ((len > (get_x_res() - x)) ? get_x_res() - x : len)); // paint until the end of the line
  }
  else {
    printf("Invalid color mode\n");
    return 1;
  }

  return 0;
}

int(vbe_get_mode_info2)(uint16_t mode, vbe_mode_info_t *vbe_mode_info) {
  reg86_t reg86;
  mmap_t mmap; // variable that will be changed by sys_int86()

  memset(&reg86, 0, sizeof(reg86)); // zero the structure

  // Allocate memory block in low memory area
  if ( retry_lm_alloc(sizeof(vbe_mode_info_t), &mmap) == NULL) {
    printf("Error on retry_lm_alloc\n");
    return 1;
  }

  // Build the struct
  reg86.intno = VBE_BIOS_CALL; /*BIOS video services */
  reg86.ah = VBE_MODE;
  reg86.al = RETURN_VBE_MODE_INFO;
  reg86.cx = mode;
  reg86.es = PB2BASE(mmap.phys); // set a segment base
  reg86.di = PB2OFF(mmap.phys);  // set the offset accordingly

  if (sys_int86(&reg86) != OK) {
    lm_free(&mmap);
    printf("Error on vbe_get_mode_info2 -> sys_int86()\n");
    return 1;
  }

  // Verify the return for errors
  if (reg86.ax != FUNC_RETURN_OK) {
    lm_free(&mmap);
    printf("Return from sys_int86 in ax was different from OK\n");
    return 1;
  }

  // Copy the requested info to vbe_mode_info
  memcpy(vbe_mode_info, mmap.virt, sizeof(vbe_mode_info_t));

  //free allocated memory
  lm_free(&mmap);

  return 0;
}

void *retry_lm_alloc(size_t size, mmap_t *mmap) {
  void *result = NULL;
  for (unsigned i = 0; i < 5; i++) {
    result = lm_alloc(size, mmap);
    if (result != NULL)
      break;
    sleep(1);
  }

  return result;
}

uint32_t get_pattern_color(uint32_t first, uint8_t row, uint8_t col, uint8_t step, uint8_t no_rectangles) {
  uint32_t color = 0;
  /* Direct color mode */
  if (get_memory_model() == DIRECT_COLOR_MODE) {

    /* Separate the original color components */
    uint32_t orig_red = (first >> get_red_field_position()) & set_bits_mask(get_red_mask_size());
    uint32_t orig_green = (first >> get_green_field_position()) & set_bits_mask(get_green_mask_size());
    uint32_t orig_blue = (first >> get_blue_field_position()) & set_bits_mask(get_blue_mask_size());

    /* Calculate new color components */
    uint32_t red = (orig_red + col * step) % (BIT(get_red_mask_size()));
    uint32_t green = (orig_green + row * step) % (BIT(get_green_mask_size()));
    uint32_t blue = (orig_blue + (col + row) * step) % (BIT(get_blue_mask_size()));

    /* Build the new color */
    color = (red << get_red_field_position()) | (green << get_green_field_position()) | (blue << get_blue_field_position());
  }
  /* Indexed color mode */
  else if (get_memory_model() == INDEXED_COLOR_MODE) {
    /* Calculate index color */
    color = (first + (row * no_rectangles + col) * step) % BIT(get_bits_per_pixel());
  }
  return color;
}

int(draw_image)(uint16_t x, uint16_t y, xpm_image_t *img, enum xpm_image_type type) {
  if(x < 0 || y < 0){
    printf("Image coordinates are negative\n");
    return 1;
  } 

  // parsing data of the img
  uint16_t width = (*img).width, height = (*img).height; // number of horizontal and vertical pixels
  //size_t size = (*img).size;                             // width * height * bytes_per_pixel
  uint8_t *bytes = (*img).bytes;                         // array with the colors of each pixel (1 byte per pixel)
  int byte_color_counter = 0;                            // stores the index of the current byte
  if (type == XPM_INDEXED) {                             // 1 byte per pixel
    uint8_t pixel_size = BPP_INDEXED;       // bytes per pixel
    for (uint16_t yIdx = y; yIdx < y + height; yIdx++) {
      if(yIdx >= get_y_res()){    // yIdx surpasses the bounds
        break;
      }
      for (uint16_t xIdx = x; xIdx < x + width; xIdx++) {
        if (xIdx >= get_x_res()) { // if xIdx surpasses the bounds
          break;
        }
        uint32_t y_coord = yIdx * get_x_res() * pixel_size ; // number of lines to travel
        uint32_t x_coord = xIdx * pixel_size;
        memcpy(mapped_mem + y_coord + x_coord, &bytes[byte_color_counter], pixel_size); // Copies "pixel_size" bytes from address "&color" to address "1st arg"
        byte_color_counter++;
      }
    }
  }
  return 0;
}

void swap_buffers(uint8_t *buffer){
  memcpy(mapped_mem, buffer, get_x_res() * get_y_res());
}

void clear_buffer(uint8_t* buffer, uint8_t color){
  memset(buffer, color, get_x_res() * get_y_res());   // copies color to all bytes from buffer
}

uint8_t get_bits_per_pixel() { return vbe_mode_info.BitsPerPixel; }
uint16_t get_x_res() { return vbe_mode_info.XResolution; }
uint16_t get_y_res() { return vbe_mode_info.YResolution; }
uint8_t get_memory_model() { return vbe_mode_info.MemoryModel; }
uint8_t get_red_mask_size() { return vbe_mode_info.RedMaskSize; }
uint8_t get_red_field_position() { return vbe_mode_info.RedFieldPosition; }
uint8_t get_blue_mask_size() { return vbe_mode_info.BlueMaskSize; }
uint8_t get_blue_field_position() { return vbe_mode_info.BlueFieldPosition; }
uint8_t get_green_mask_size() { return vbe_mode_info.GreenMaskSize; }
uint8_t get_green_field_position() { return vbe_mode_info.GreenFieldPosition; }
uint8_t get_rsvd_mask_size() { return vbe_mode_info.RsvdMaskSize; }
uint8_t get_rsvd_field_position() { return vbe_mode_info.RsvdFieldPosition; }
