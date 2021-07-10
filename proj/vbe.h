#ifndef VBE_H
#define VBE_H

#include <lcom/lcf.h>

int(set_video_mode)(uint16_t mode);

/**
 * Maps the physical memory to a virtual memory stored in mapped_mem. Initiates the mode passed as argument and grabs information relative to that mode
 */
int(init_graphic_mode)(uint16_t mode);

// vg_exit()  --> Returns to text mode

/**
 * Draws a rectangle
 */
int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/**
 * @brief Draws a rectangle in desired buffer
 * 
 */
int(vg_draw_rectangle_on)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, uint8_t* buffer);

/**
 * @brief Draws horizontal line in desired buffer
 * 
 */
int(vg_draw_hline_on)(uint16_t x, uint16_t y, uint16_t len, uint32_t color, uint8_t* buffer);

/**
 * Fills the vbe_mode_info_t structure with info from the mode
 */
int(vbe_get_mode_info2)(uint16_t mode, vbe_mode_info_t *vbe_mode_info);

/**
 * Tries to allocates a memory block in low memory area with the specified size 5 times and initializes the input mmap_t struct with the mapping information
 */
void *(retry_lm_alloc)(size_t size, mmap_t *mmap);

//int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/**
 * Calculate the color for the elem in a specific row and col
 */
uint32_t(get_pattern_color)(uint32_t first, uint8_t row, uint8_t col, uint8_t step, uint8_t no_rectangles);


int(draw_image)(uint16_t x, uint16_t y, xpm_image_t *img, enum xpm_image_type type);

/**
 * Updates the VRAM with the new pixmap, by copying the backbuffer to the mapped_mem
 */ 
void swap_buffers(uint8_t *buffer);

/**
 * Clears the specified buffer by setting all bytes to the specified color
 */ 
void clear_buffer(uint8_t* buffer, uint8_t color);

/**
 * @arg buffer_y_start line of the starting pixel in the VRAM (Y position)
 */ 
int (set_display_start)(uint16_t buffer_y_start);

uint8_t get_bits_per_pixel();
uint16_t get_x_res();
uint16_t get_y_res();
uint8_t get_memory_model();
uint8_t get_red_mask_size();
uint8_t get_red_field_position();
uint8_t get_blue_mask_size();
uint8_t get_blue_field_position();
uint8_t get_green_mask_size();
uint8_t get_green_field_position();
uint8_t get_rsvd_mask_size();
uint8_t get_rsvd_field_position();

#endif
