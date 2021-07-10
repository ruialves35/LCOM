#ifndef specific_functions
#define specific_functions

#include <stdbool.h>
#include <stdint.h>

int(kbc_sys_inb)(int port, uint8_t *value);

void (kbc_ih)();

void (kbc_poll_status)();

int(keyboard_and_timer_handle_interruption)(uint8_t kb_bit_no, uint8_t timer_bit_no, uint8_t time);

#endif
