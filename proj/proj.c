// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "game.h"
#include "i8042.h"
#include "i8254.h"
#include "kbc.h"
#include "rtc.h"
#include "sprite.h"
#include "vbe.h"
#include "vcMacros.h"
#include "uart.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

/*
static int print_usage(uint16_t mode) {
  printf("Usage: %x\n", mode);

  return 1;
} */

int(proj_main_loop)(int argc, char *argv[]) {
  /*
  if (argc == 0){
    printf("Not enough args\n");
    return 1;
  }

  // parse mode info (it has some limitations for the sake of simplicity)
  if (sscanf(argv[0], "%hx", &mode) != 1) {
    printf("%s: invalid mode (%s)\n", __func__, argv[0]);

    return print_usage(mode);
  }

  return proj_demo(mode, minix3_logo, grayscale, delay);
  */

  
  uint16_t mode = R1152x864_DIRECT;
  if (init_graphic_mode(mode) != 0) {
    printf("Error initializing graphic mode\n");
    vg_exit();
    return 1;
  }

  Game game1;
  choose_your_destiny(&game1);

  //sleep(3);

  vg_exit();
  

  /*
  uart_setup_lcr(5, 2, PARITY_ODD);
  uart_set_bit_rate(DEFAULT_BIT_RATE);

  uint8_t uart_bit_no;
  if(uart_subscribe_int(&uart_bit_no) != 0){
    printf("Error on uart_subscribe_int\n");
  }
  printf("done\n");

  if(uart_unsubscribe_int() != 0){
    printf("error on uart_unsubscribe_int\n");
  }
  */  

  return 0;
}
