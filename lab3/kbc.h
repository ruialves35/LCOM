#include <stdbool.h>
#include <stdint.h>

/**
  *Subscribes keybaord for interrupts
  @param bit_no variable that will get the value for interrupts notifications
  @return 0 if success, 1 otherwise
*/
int (keyboard_subscribe_int)(uint8_t *bit_no);

/**
  *unsubscribes an interrupt
  @return 0 if success, 1 otherwise
*/
int (keyboard_unsubscribe_int)();

/**
 * Handles keyboard interruptions
 * @param 
 * @return 0 if success, 1 otherwise
 */ 
int (keyboard_handle_interruption)(uint8_t bit_no);

int (keyboard_polling)();

int (keyboard_enable_interruptions)();

int(kbc_read_cmd_byte)(uint8_t *status);

int(kbc_write_cmd_args)(uint8_t status);
