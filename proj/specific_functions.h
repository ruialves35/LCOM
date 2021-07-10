#ifndef specific_functions
#define specific_functions

#include <stdbool.h>
#include <stdint.h>

int(kbc_sys_inb)(int port, uint8_t *value);

void (kbc_ih)();

void (kbc_poll_status)();

int(mouse_sys_inb)(int port, uint8_t *value);

#endif
