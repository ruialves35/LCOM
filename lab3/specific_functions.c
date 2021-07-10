#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"
#include "specific_functions.h"
#include <stdbool.h>
#include <stdint.h>

uint32_t kb_counter = 0;
uint32_t counter = 0;
uint8_t size, data;
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
    printf("Error on parity or timeout. Invalid data\n");
    return;
  }

  if ((status & KBC_OBF) == 0) {
    printf("Output buffer isn't full. Data is not available for reading\n");
    return;
  }

  //if status byte is alright
  if (kbc_sys_inb(KBC_OUT_BUF, &data) == 1) {
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
      printf("Error on parity or timeout. Invalid data");
      //return;
    }
    else if ((status & KBC_AUX_BIT) != 0) {
      printf("Error, mouse data is being read");
      //return;
    }
    else if ((status & KBC_OBF) == 0) {
      printf("Output buffer isn't full. Data is not available for reading");
      //return;
    }
    else {
      break;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  //if status byte is alright
  if (kbc_sys_inb(KBC_OUT_BUF, &data) == 1) {
    printf("Error on function kbc_sys_inb for out_buf");
  }
}

int(keyboard_and_timer_handle_interruption)(uint8_t kb_bit_no, uint8_t  timer_bit_no, uint8_t time) {
  message msg;
  int ipc_status;
  uint32_t kb_irq_set = BIT(kb_bit_no);     //kb
  uint32_t timer_irq_set = BIT(timer_bit_no); //timer
  uint8_t prevByte;                            //kb
  bool is2Bytes = false;                       //kb
  while (data != KBC_ESC_BREAKCODE && counter < time * 60) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & kb_irq_set) {
            kbc_ih();
            if (data == KBC_2BYTES_CODE) {
              prevByte = data;
              is2Bytes = true;
            }
            else {
              counter = 0;    // resets the counter to 0 on a keyboard press
              if (is2Bytes) {
                size = 2;
                uint8_t bytes[] = {prevByte, data};
                if ((data & BIT(7)) == 0) //check if it is a break or make code
                  make = true;
                else
                  make = false;
                if (kbd_print_scancode(make, size, bytes) != 0) {
                  printf("kbd_print_scancode failed");
                }
                is2Bytes = false;
              }
              else {
                size = 1;
                uint8_t bytes[] = {data};
                if ((data & BIT(7)) == 0) //check if it is a break or make code
                  make = true;
                else
                  make = false;
                if (kbd_print_scancode(make, size, bytes) != 0) {
                  printf("kbd_print_scancode failed");
                }
              }
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) { //  subscribed interrupt
            timer_int_handler();
            if (counter % 60 == 0) {
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break; // no other notifications expected
      }
    }
    else {
      //received a standard message, not a notification
    }
  }
  return 0;
}
