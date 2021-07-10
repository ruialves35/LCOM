#include <lcom/lcf.h>

#include "vbe_utils.h"

#define BYTE_SIZE 8

uint8_t calculate_size_in_bytes(uint8_t bits) {
	return (uint8_t)(bits / BYTE_SIZE ) +  ( (bits % BYTE_SIZE != 0) ? 1 : 0);
}

uint32_t set_bits_mask(uint8_t n) {
	if (n == 0) return 0;
	uint32_t mask = 1;
	for (uint8_t i = 1; i < n; i++) {
		mask <<= 1;
		mask |= 1;
	}
	return mask;
}
