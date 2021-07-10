#include <lcom/lcf.h>

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
