#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  val = val >> 8;
  *msb = val;
  return 0;

}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t valor;                // variable to store value from int_sys_inb int 32 bit

  if (sys_inb(port, &valor) != OK){
    printf("Eror on function util_sys_inb on sys_inb");
    return 1; 
  }

  *value = valor;              // convert value from 32bit to 8 bit
  return 0;
}
