#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"
#include "specific_functions.h"
#include <stdbool.h>
#include <stdint.h>

extern uint8_t size;
extern uint8_t data;
extern bool make;
extern uint32_t kb_counter;
int kb_hook_id, timer_hook_id;

int(keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBC_IRQ; //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
  kb_hook_id = *bit_no;

  if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kb_hook_id) != OK) { //subscribes the interruptions from the keybaord
    printf("sys_irqsetpolicy() failed\n");
    return 1;
  }

  return 0;
}

int(keyboard_unsubscribe_int)() {
  if (sys_irqrmpolicy(&kb_hook_id) != OK) {
    printf("Error in timer_unsubscribe_int, sys_irqrmpolicy\n");
    return 1;
  }

  return 0;
}

int(keyboard_handle_interruption)(uint8_t bit_no) {
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);

  uint8_t prevByte;
  bool is2Bytes = false;
  while (data != KBC_ESC_BREAKCODE) {
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { //  subscribed interrupt
            kbc_ih();
            if (data == KBC_2BYTES_CODE) {
              //kbc_ih();             // só posso chamar o handler 1 vez, esperar que ocorra outra interrupt para ler
              prevByte = data;
              is2Bytes = true;
            }
            else {
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
          break;
        default:
          break; // no other notifications expected
      }
    }
    else {
      //received a standard message, not a notification
    }
  }
  if (kbd_print_no_sysinb(kb_counter) != 0) {
    printf("Error in kbd_print_scancode");
  }
  return 0;
}

/**
 * Cycle that detects keyboard presses by using polling technique with a specified delay(DELAYUS), 
 * and prints the specific key. Cycle stops when you press 'ESC' key.
 */
int(keyboard_polling)() {
  uint8_t prevByte;
  bool is2Bytes = false;
  while (data != KBC_ESC_BREAKCODE) {
    kbc_poll_status();
    if (data == KBC_2BYTES_CODE) {
      prevByte = data;
      is2Bytes = true;
    }
    else {
      if (is2Bytes) {
        size = 2;
        uint8_t bytes[] = {prevByte, data};
        if ((data & BIT(7)) == 0) //check if it is a break or make code
          make = true;
        else
          make = false;
        if (kbd_print_scancode(make, size, bytes) != 0) {
          printf("kbd_print_scancode failed\n");
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
          printf("kbd_print_scancode failed\n");
        }
      }
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  if (kbd_print_no_sysinb(kb_counter) != 0) {
    printf("Error in kbd_print_scancode\n");
  }

  return 0;
}

/**
 * Enables keyboard interruptions
 */
int(keyboard_enable_interruptions)() {
  uint8_t status;
  while (1) {
    if (kbc_sys_inb(KBC_ST_REG, &status) == 1) {
      printf("Error on function kbc_sys_inb for st_reg\n");
    }

    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) { //check if there is any parity or timeout error
      printf("Error on parity or timeout. Invalid data\n");
    }

    else if ((status & KBC_IBF_BIT) != 0) {
      printf("Error. Input buffer full\n");
    }

    else if (kbc_read_cmd_byte(&status) == 1) { //says that we want to read the cmd byte, and stores it in status
      printf("Error on kbc_read_cmd_byte");
    }

    else {
      status = status | BIT(0); // activates keyboard interrupts

      if (kbc_write_cmd_args(status) == 1) {  //write the new command
        printf("Error on kbc_write_cmd_byte");
      }

      else {
        return 0;
      }
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
}

int(kbc_read_cmd_byte)(uint8_t *status) {
  if (sys_outb(KBC_CMD_REG, KBC_READ_CMD) == 1) { //write in command register that you want to read the command byte
    printf("Error writing read command to CMD_REG\n");
    return 1;
  }

  if (kbc_sys_inb(KBC_OUT_BUF, status) == 1) { //read command byte
    printf("Error reading value from output buffer\n");
    return 1;
  }
  return 0;
}

int(kbc_write_cmd_args)(uint8_t status) {
  if (sys_outb(KBC_CMD_REG, KBC_WRITE_CMD) == 1) { //write in command register that you want to read the command byte
    printf("Error writing write command to CMD_REG\n");
    return 1;
  }


  if (sys_outb(KBC_ARGS_REG, status) == 1) { //write new command byte on args reg
    printf("Error writing to ARGS REG");
    return 1;
  }

  return 0;
}
