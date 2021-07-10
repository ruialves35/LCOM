// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include <machine/int86.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "i8254.h"
#include "kbc.h"
#include "sprite.h"
#include "vbe.h"
#include "vcMacros.h"

extern uint8_t data;

//static void *video_mem; /* frame-buffer VM address  (static global variable*/

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  /* Initialize lower memory region !  NO NEED TO INVOKE ANYMORE */
  /*
  if (lm_init(true) == NULL) {  
    printf("(%s) Could not run lm_init\n", __func__);
    return 1;
  }   */

  /* Get information about vbe mode */
  /*
  vbe_mode_info_t vmi;
  if (vbe_get_mode_info(mode, &vmi) != 0) { // need to create my own function afterwards
    printf("Error on vbe_get_mode_info\n");
  } */

  /* change video mode */
  if (set_video_mode(mode) != 0) {
    printf("Error on set_video_mode\n");
  }

  /* Wait for delay seconds    */
  sleep(delay);

  /* returns to default Minix text mode   */
  vg_exit();

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (init_graphic_mode(mode) != 0) {
    printf("Error on init_graphic_mode\n");
    vg_exit();
    return 1;
  }

  if (vg_draw_rectangle(x, y, width, height, color) != 0) {
    printf("Error on draw_rectangle\n");
    vg_exit();
    return 1;
  }

  uint8_t bit_no;
  if (keyboard_subscribe_int(&bit_no) != 0) { // subscribe keyboard
    printf("Error on keyboard_subscribe_int\n");
    vg_exit();
    return 1;
  }

  // variables to hold results
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  while (data != KBC_ESC_BREAKCODE) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { //  subscribed interrupt
            kbc_ih();
          }
          break;
        default:
          break; // no other notifications expected
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0) { // unsubscribe keybaord interrupts
    printf("Error on keyboard_unsubscribe_int\n");
    vg_exit();
    return 1;
  }

  // returns to text Mode
  vg_exit();

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (init_graphic_mode(mode) != 0) {
    printf("Error on init_graphic_mode\n");
    vg_exit();
    return 1;
  }
  // Calculates the matrix and draws the rectangles
  uint16_t width = get_x_res() / no_rectangles, height = get_y_res() / no_rectangles;
  printf("width / height %u / %u", width, height);
  uint16_t y = 0, x;
  uint32_t color;

  if (get_memory_model() != DIRECT_COLOR_MODE && get_memory_model() != INDEXED_COLOR_MODE) { // Check color mode
    printf("(%s) Invalid color mode\n", __func__);
    return 1;
  }

  for (uint8_t yIdx = 0; yIdx < no_rectangles; yIdx++) {
    x = 0;
    for (uint8_t xIdx = 0; xIdx < no_rectangles; xIdx++) {
      color = get_pattern_color(first, yIdx, xIdx, step, no_rectangles);
      if (vg_draw_rectangle(x, y, width, height, color) != 0) {
        printf("Error on vg_draw_rectangle\n");
        return 1;
      }
      x += width;
    }
    y += height;
  }

  uint8_t bit_no;
  if (keyboard_subscribe_int(&bit_no) != 0) { // subscribe keyboard
    printf("Error on keyboard_subscribe_int\n");
    vg_exit();
    return 1;
  }

  // variables to hold results
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  while (data != KBC_ESC_BREAKCODE) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { //  subscribed interrupt
            kbc_ih();
          }
          break;
        default:
          break; // no other notifications expected
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0) { // unsubscribe keybaord interrupts
    printf("Error on keyboard_unsubscribe_int\n");
    vg_exit();
    return 1;
  }

  // returns to text Mode
  vg_exit();

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (init_graphic_mode(R1024x768_INDEXED) != 0) {
    printf("Error on init_graphic_mode\n");
    vg_exit();
    return 1;
  }

  enum xpm_image_type type = XPM_INDEXED;
  xpm_image_t img;
  uint8_t *map;

  //get the pixmap from the XPM
  if ((map = xpm_load(xpm, type, &img)) == NULL) { // returns the address to the allocated memory to where the image was read, updates img with the img info
    printf("Error on xpm_load\n");
    return 1;
  }

  if (draw_image(x, y, &img, type) != 0) {
    printf("Error on draw_image\n");
    return 1;
  }

  uint8_t bit_no;
  if (keyboard_subscribe_int(&bit_no) != 0) { // subscribe keyboard
    printf("Error on keyboard_subscribe_int\n");
    vg_exit();
    return 1;
  }

  // variables to hold results
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  while (data != KBC_ESC_BREAKCODE) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { //  subscribed interrupt
            kbc_ih();
          }
          break;
        default:
          break; // no other notifications expected
      }
    }
  }

  if (keyboard_unsubscribe_int() != 0) { // unsubscribe keybaord interrupts
    printf("Error on keyboard_unsubscribe_int\n");
    vg_exit();
    return 1;
  }

  // returns to text Mode
  vg_exit();

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  uint16_t negative_speed_displacement_counter = 0;

  bool reachedEnd = false;

  if (init_graphic_mode(R1024x768_INDEXED) != 0) {
    printf("Error on init_graphic_mode\n");
    vg_exit();
    return 1;
  }

  enum xpm_image_type type = XPM_INDEXED;

  // creates sprite and parses all the info such as the color map
  Sprite *sprite;
  if (xi == xf) { // sprite will move vertically
    sprite = create_sprite(xpm, xi, yi, 0, speed, type);
  }
  else { // sprite will move horizontally
    sprite = create_sprite(xpm, xi, yi, speed, 0, type);
  }

  //Change timer frequency to fr_rate(frame rate)
  /*
  if (timer_set_frequency(TIMER0_IRQ, (uint32_t) fr_rate) != 0) {
    printf("Error on timer_set_frequency\n");
    vg_exit();
    return 1;
  } */

  int counter = 0;
  int freq = sys_hz() / fr_rate; // (60)
  //printf("%d", freq);

  uint8_t timer_bit_no;
  if (timer_subscribe_int(&timer_bit_no) == 1) {
    printf("Error on timer_subscribe_int");
    vg_exit();
    return 1;
  }

  uint8_t kb_bit_no;
  if (keyboard_subscribe_int(&kb_bit_no) != 0) { // subscribe keyboard
    printf("Error on keyboard_subscribe_int\n");
    if (timer_unsubscribe_int() == 1) {
      printf("Error on timer_unsubscribe_int");
      vg_exit();
      return 1;
    }
    vg_exit();
    return 1;
  }

  // Allocate memory for a backbuffer, this is made to ensure no trace is left when drawing
  uint8_t *backbuffer = malloc(get_y_res() * get_x_res());
  if (backbuffer == NULL) {
    printf("Error allocating space for the backbuffer\n");
    return 1;
  }

  // Draw the pixmap on the initial position
  draw_sprite(sprite, get_x_res(), get_y_res());

  // variables to hold results
  int ipc_status;
  message msg;
  uint32_t kb_irq_set = BIT(kb_bit_no);       //kb
  uint32_t timer_irq_set = BIT(timer_bit_no); //timer
  while (data != KBC_ESC_BREAKCODE) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                //hardware interrupt notification
          if (msg.m_notify.interrupts & kb_irq_set) { //  interrupt from keybaord
            kbc_ih();
          }
          if (msg.m_notify.interrupts & timer_irq_set) { //  interrupt from timer
            if ((++counter) % freq == 0) {
              if (!reachedEnd) {
                if (sprite->yspeed != 0) { // vertical movement
                  if (sprite->yspeed > 0) {
                    sprite->y += sprite->yspeed;
                  }
                  else { // if yspeed < 0
                    if (negative_speed_displacement_counter >= abs(sprite->yspeed)) {
                      sprite->y++;
                      negative_speed_displacement_counter = 0;
                    }
                    else {
                      negative_speed_displacement_counter++;
                    }
                  }
                  if (sprite->y >= yf) {
                    sprite->y = yf;
                    reachedEnd = true; //reached the end of the animation
                  }
                }
                else if (sprite->xspeed != 0) { // horizontal movement
                  if (sprite->xspeed > 0) {
                    sprite->x += sprite->xspeed;
                  }
                  else { // xspeed < 0
                    if (negative_speed_displacement_counter >= abs(sprite->xspeed)) {
                      sprite->x++;
                      negative_speed_displacement_counter = 0;
                    }
                    else {
                      negative_speed_displacement_counter++;
                    }
                  }
                  if (sprite->x >= xf) {
                    sprite->x = xf;
                    reachedEnd = true; //reached the end of the animation
                  }
                }
                // Draw the pixmap

                clears_last_movement_on(sprite, get_x_res(), get_y_res(), backbuffer);
                //clear_buffer(backbuffer, 0);                                  // clears backbuffer
                draw_sprite_on(sprite, get_x_res(), get_y_res(), backbuffer); // draws the pixmap on the backbuffer
                swap_buffers(backbuffer);                                     // updates the VRAM
              }
              /*
            }
            counter++;
            */
              if (counter % freq == 0) { // to avoid having a a huge number on counter
                counter = 0;
              }
            }
            break;
            default:
              break; // no other notifications expected
          }
      }
    }
  }

  if (timer_unsubscribe_int() == 1) {
    printf("Error on timer_unsubscribe_int");
    vg_exit();
    return 1;
  }

  if (keyboard_unsubscribe_int() != 0) { // unsubscribe keybaord interrupts
    printf("Error on keyboard_unsubscribe_int\n");
    vg_exit();
    return 1;
  }

  // Free allocated mem for the backbuffer
  free(backbuffer);

  // returns to text Mode
  vg_exit();

  return 0;
}

int(video_test_controller)() {
  /*
  reg86_t reg86;
  mmap_t mmap; // variable that will be changed by sys_int86()
  vg_vbe_contr_info_t contr_info;
  VbeInfoBlock* info_block;

  memset(&reg86, 0, sizeof(reg86)); // zero the structure

  // Allocate memory block in low memory area
  if (retry_lm_alloc(sizeof(VbeInfoBlock), &mmap) == NULL) {
    printf("Error on retry_lm_alloc\n");
    return 1;
  }

  char *sig = "VBE2";
  memcpy(mmap.virt, sig, 4);

  // Build the struct
  reg86.intno = VBE_BIOS_CALL; // BIOS video services 
  reg86.ah = VBE_MODE;
  reg86.al = RETURN_VBE_CONTROLLER_INFO;
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

#define CONVERSOR(x) (void *) (((((uint32_t) x & 0xFFFF0000) >> 12) + (uint32_t)((uint32_t) x & 0x0000FFFF)) + (uint32_t) init)

  // Copy the requested info to vg_vbe_contr_info_t
  VbeInfoBlock *info_block = mmap.virt;
  memcpy(&contr_info.VBESignature, &info_block->VbeSignature, 4);
  memcpy(&contr_info.VBEVersion, &info_block->VbeVersion, 2);
  contr_info.OEMString = CONVERSOR(info_block->OemStringPtr);
  contr_info.VideoModeList = CONVERSOR(info_block->VideoModePtr);
  contr_info.TotalMemory = info_block->TotalMemory * 64;
  contr_info.OEMVendorNamePtr = CONVERSOR(info_block->OemVendorNamePtr);
  contr_info.OEMProductNamePtr = CONVERSOR(info_block->OemProductNamePtr);
  contr_info.OEMProductRevPtr = CONVERSOR(info_block->OemProductRevPtr);

  //free allocated memory
  lm_free(&mmap);

  // Display the information 
  if (vg_display_vbe_contr_info(&contr_info) != OK) {
    printf("(%s): vg_display_vbe_contr_info returned with an error\n", __func__);
    return 1;
  }
  */
  return 0;
}
