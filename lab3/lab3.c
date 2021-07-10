#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include "i8042.h"
#include "kbc.h"
#include <stdbool.h>
#include <stdint.h>
#include "specific_functions.h"


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  //int hook_id;

  if (keyboard_subscribe_int(&bit_no) == 1) {
    printf("Error on keybord_subscribe_int");
    return 1;
  };

  keyboard_handle_interruption(bit_no);

  if (keyboard_unsubscribe_int() == 1) {
    printf("Error on keyboard_unsubscribe_int");
    return 1;
  };
  /* To be completed by the students */
  //printf("%s is not yet implemented!\n", __func__);
  return 0;
}

int(kbd_test_poll)() {
  keyboard_polling();
  keyboard_enable_interruptions();

  /* To be completed by the students */
  //printf("%s is not yet implemented!\n", __func__);
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t kb_bit_no, timer_bit_no;

  if (keyboard_subscribe_int(&kb_bit_no) == 1) { //subscribe keyboard
    printf("Error on keyboard_subscribe_int");
    return 1;
  }

  if (timer_subscribe_int(&timer_bit_no) == 1) {
    printf("Error on timer_subscribe_int");
    return 1;
  }

  keyboard_and_timer_handle_interruption(kb_bit_no, timer_bit_no, n);

  if (keyboard_unsubscribe_int() == 1) {
    printf("Error on keyboard_unsubscribe_int");
    return 1;
  };

  if (timer_unsubscribe_int() == 1) {
    printf("Error on timer_unsubscribe_int");
    return 1;
  };

  /* To be completed by the students */
  //printf("%s is not yet implemented!\n", __func__);
  return 0;
}
