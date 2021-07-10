#ifndef VBE_UTILS_H
#define VBE_UTILS_H

#include <lcom/lcf.h>

/**
 * @brief calculates size in bytes, given the size in bits
 * 
 * @param bits 
 * @return uint8_t 
 */
uint8_t calculate_size_in_bytes(uint8_t bits);

/**
 * Creates a bit mask of the n LSB
 */ 
uint32_t set_bits_mask(uint8_t n);

#endif
