#ifndef UTILS_H
#define UTILS_H

#include <lcom/lcf.h>

/**
 * get lsb byte of a 16bit value
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * get msb byte of a 16bit value
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * Gets a value from a port to a variable
 */
int (util_sys_inb)(int port, uint8_t *value);

#endif
