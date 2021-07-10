#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"
#include "specific_functions.h"
#include <stdbool.h>
#include <stdint.h>

uint32_t kb_counter = 0;
uint8_t size, kb_data;
bool make;

int(kbc_sys_inb)(int port, uint8_t *value) {
  uint32_t valor; // variable to store value from int_sys_inb int 32 bit

  if (sys_inb(port, &valor) != OK) {
    printf("Error on function sys_inb");
    return 1;
  }

  *value = valor; // convert value from 32bit to 8 bit
  kb_counter++;
  return 0;
}

void(kbc_ih)() {
  uint8_t status;
  if (kbc_sys_inb(KBC_ST_REG, &status) == 1) {
    printf("Error on function kbc_sys_inb for st_reg\n");
  }
  //printf("Status: %x", status);
  if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) {
    printf("Error on parity or timeout. Invalid kb_data\n");
    return;
  }

  if ((status & KBC_OBF) == 0) {
    printf("Output buffer isn't full. kb_data is not available for reading\n");
    return;
  }

  //if status byte is alright
  if (kbc_sys_inb(KBC_OUT_BUF, &kb_data) == 1) {
    printf("Error on function kbc_sys_inb for out_buf\n");
  }
}

/**
 * while output buffer is empty, keep iterating until it has a value that will be stored in status
 * 
 */
void(kbc_poll_status)() {
  uint8_t status;
  while (1) {
    if (kbc_sys_inb(KBC_ST_REG, &status) == 1) {
      printf("Error on function kbc_sys_inb for st_reg");
    }
    //printf("Status: %x", status);
    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) {
      printf("Error on parity or timeout. Invalid kb_data");
      //return;
    }
    else if ((status & KBC_AUX_BIT) != 0) {
      printf("Error, mouse kb_data is being read");
      //return;
    }
    else if ((status & KBC_OBF) == 0) {
      printf("Output buffer isn't full. kb_data is not available for reading");
      //return;
    }
    else {
      break;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  //if status byte is alright
  if (kbc_sys_inb(KBC_OUT_BUF, &kb_data) == 1) {
    printf("Error on function kbc_sys_inb for out_buf");
  }
}

int(mouse_sys_inb)(int port, uint8_t *value) {
  uint32_t valor; // variable to store value from int_sys_inb int 32 bit

  if (sys_inb(port, &valor) != OK) {
    printf("Error on function sys_inb");
    return 1;
  }

  *value = valor; // convert value from 32bit to 8 bit
  //kb_counter++;
  return 0;
}
