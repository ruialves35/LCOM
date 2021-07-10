#include <lcom/lcf.h>
#include <minix/syslib.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "mouse.h"
#include "specific_functions.h"

uint8_t mouse_data;
int mouse_hook_id;
uint64_t counter = 0;
int x_draw_len = 0;
int y_draw_len = 0;

int(mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = KBC_MOUSE_IRQ; //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
  mouse_hook_id = *bit_no;

  if (sys_irqsetpolicy(KBC_MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK) { //subscribes the interruptions from the keybaord
    printf("sys_irqsetpolicy() failed\n");
    return 1;
  }

  return 0;
}

int(mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id) != OK) {
    printf("Error in timer_unsubscribe_int, sys_irqrmpolicy\n");
    return 1;
  }

  return 0;
}

int(kbc_read_mouse_ack)() {
  uint8_t status, mouse_response;
  uint8_t counter = 0;
  while (counter < 15) {
    if (mouse_sys_inb(KBC_ST_REG, &status) != 0) {
      printf("Error reading ST reg on mouse_sys_inb\n");
    }
    /*
    else if ((status & KBC_OBF) != 1) {                             // DUNNO WHY OUTPUT BUFFER DOES NOT BECOME FULL
      printf("Output buffer not full. Trying again.");
    } 
    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) { //check if there is any parity or timeout error
      printf("Error on parity or timeout. Invalid data\n");
    } */
    else if (mouse_sys_inb(KBC_OUT_BUF, &mouse_response) == 1) { //read command byte
      printf("Error reading value from output buffer\n");
    } 
    else if (mouse_response == MOUSE_ACK) {
      printf("Everything OK\n");
      /*
      if (mouse_irq_enable() == 1) {
        return 1;
      } */
      return 0;
    }
    counter++;
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int(kbc_read_cmd_return)(uint8_t *return_value) {
  uint8_t status;
  uint8_t counter = 0;
  while (counter < 15) {
    if (mouse_sys_inb(KBC_ST_REG, &status) != 0) {
      printf("Error reading ST reg on mouse_sys_inb\n");
    }
    else if ((status & KBC_OBF) != 1) {
      printf("Output buffer not full. Trying again.");
    }
    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) { //check if there is any parity or timeout error
      printf("Error on parity or timeout. Invalid data\n");
    }
    else if (mouse_sys_inb(KBC_OUT_BUF, return_value) == 1) { //read command byte
      printf("Error reading value from output buffer\n");
    }
    else {
      return 0;
    }
    counter++;
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int(kbc_read_mouse_data_by_poll)(uint8_t *return_value){
  uint8_t status;
  uint8_t counter = 0;
  while (counter < 10) {
    if (mouse_sys_inb(KBC_ST_REG, &status) != 0) {
      printf("Error reading ST reg on mouse_sys_inb\n");
    }
    else if ((status & KBC_OBF) != 1) {
      printf("Output buffer not full. Trying again.");
    }
    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) { //check if there is any parity or timeout error
      printf("Error on parity or timeout. Invalid data\n");
    }
    else if((status & KBC_AUX_BIT) == 0){
      printf("This data is not from mouse.\n");
    }
    else if (mouse_sys_inb(KBC_OUT_BUF, return_value) == 1) { //read command byte
      printf("Error reading value from output buffer\n");
    }
    else {
      return 0;
    }
    counter++;
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}

int(kbc_read_cmd_byte)(uint8_t *status) {
  if (kbc_write_cmd_command(KBC_READ_CMD) == 1) { //write in command register that you want to read the command byte
    printf("Error writing read command to CMD_REG\n");
    return 1;
  }

  if (kbc_read_cmd_return(status) == 1) { //read command byte
    printf("Error reading value from output buffer\n");
    return 1;
  }
  return 0;
}

int(kbc_write_cmd_args)(uint8_t arg) {
  uint8_t status;
  int tries = 0;
  while (tries < 15) {
    if (mouse_sys_inb(KBC_ST_REG, &status) == 1) {
      printf("Error reading status reg\n");
    }
    else if ((status & KBC_IBF_BIT) != 0) {
      printf("Input buffer full. Trying again.");
    }
    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) { //check if there is any parity or timeout error
      printf("Error on parity or timeout. Invalid data\n");
    }
    else if (sys_outb(KBC_ARGS_REG, arg) == 1) { //write new command byte on args reg
      printf("Error writing to ARGS REG");
      return 1;
    }
    else {
      return 0;
    }
    tries++;
    tickdelay(micros_to_ticks(DELAY_US));
  }

  return 1;
}

int(kbc_write_cmd_byte)(uint8_t status) {
  if (kbc_write_cmd_command(KBC_WRITE_CMD) == 1) { //write in command register that you want to write the command byte
    printf("Error writing write command to CMD_REG\n");
    return 1;
  }

  if (kbc_write_cmd_args(status) == 1) { //write new command byte on args reg
    printf("Error writing to ARGS REG");
    return 1;
  }

  return 0;
}

int(kbc_write_cmd_command)(uint8_t byte) {
  uint8_t status;
  int counter = 0;
  while (counter < 10) {
    if (mouse_sys_inb(KBC_ST_REG, &status) == 1) {
      printf("Error on function kbc_sys_inb for st_reg\n");
    }
    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) { //check if there is any parity or timeout error
      printf("Error on parity or timeout. Invalid data\n");
    }

    else if ((status & KBC_IBF_BIT) != 0) {
      printf("Error. Input buffer full\n");
    }
    else if (sys_outb(KBC_CMD_REG, byte) == 1) { //write in command register the commmand
      printf("Error writing write command to CMD_REG\n");
      return 1;
    }
    else {
      return 0;
    }
    counter++;
    tickdelay(micros_to_ticks(DELAY_US));
  }

  return 1;
}

int(mouse_handle_interruption)(uint8_t bit_no, uint32_t cnt) { //cnt is the no_packets to read
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);

  uint8_t present_no_packets_read = 0;
  int packetByteCounter = 0;

  struct packet packet;
  printf("Will enter loop\n");
  while (present_no_packets_read < cnt) {
    int r;
    //printf("0\n");
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { //received notification
      //printf("0.5\n");
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
                                                   //printf("1\n");
          if (msg.m_notify.interrupts & irq_set) { //  subscribed interrupt
            //printf("2\n");
            mouse_ih();
            //printf("Counter Value: %d", packetByteCounter, "\n");
            switch (packetByteCounter) {
              case 0:
                if ((mouse_data & MOUSE_BYTE0_MASK) == 0) {
                  printf("Invalid byte 0 packet\n");
                  continue;
                }
                else {
                  mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                  packetByteCounter++;
                  break;
                }
              case 1:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                packetByteCounter++;
                break;
              case 2:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                mouse_print_packet(&packet);
                packetByteCounter = 0;
                present_no_packets_read++;
                break;
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
  /*
  if (kbd_print_no_sysinb(kb_counter) != 0) {
    printf("Error in kbd_print_scancode");
  }
  */
  return 0;
}

void(mouse_packets_parsing)(struct packet *packet, uint8_t byte, int byte_no) {
  /*Sets the upper 8 bits of the int16_t to 1 in case its negative*/
  static const int16_t negative_MSB = BIT(15) | BIT(14) | BIT(13) | BIT(12) | BIT(11) | BIT(10) | BIT(9) | BIT(8);
  /*Only enables the lower 8 bits of the int16_t in case its positive*/
  static const int16_t positive_MSB = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0);

  switch (byte_no) {
    case 0:
      packet->lb = byte & MOUSE_LB;
      packet->rb = byte & MOUSE_RB;
      packet->mb = byte & MOUSE_MB;
      packet->delta_x = byte & MOUSE_MSB_X_DELTA; //check if this is correct
      packet->delta_y = byte & MOUSE_MSB_Y_DELTA; // the shift left should be to the bit 9
      packet->x_ov = byte & MOUSE_X_OVFL;
      packet->y_ov = byte & MOUSE_Y_OVFL;
      packet->bytes[0] = byte;
      break;
    case 1:
      packet->delta_x = ((packet->delta_x & MOUSE_MSB_X_DELTA) == 0) ? ((int16_t) byte & positive_MSB) : ((int16_t) byte | negative_MSB);
      //packet->delta_x = (packet->delta_x << 4) | byte;
      packet->bytes[1] = byte;
      break;
    case 2:
      packet->delta_y = ((packet->delta_y & MOUSE_MSB_Y_DELTA) == 0) ? ((int16_t) byte & positive_MSB) : ((int16_t) byte | negative_MSB);
      packet->bytes[2] = byte;
      break;
  }
}

void(mouse_ih)() {
  uint8_t status;
  int loopCounter = 0;

  while (loopCounter < 10) {
    if (mouse_sys_inb(KBC_ST_REG, &status) == 1) {
      printf("Error on function mouse_sys_inb for st_reg\n");
    }
    //printf("Status: %x", status);
    else if ((status & (KBC_PARITY_AND_TIMEOUT)) != 0) {
      printf("Error on parity or timeout. Invalid data\n");
      //return;
    }
    else if ( (status & KBC_AUX_BIT) == 0){
      printf("Data is not from mouse\n");
    }
    else if ((status & KBC_OBF) == 0) {
      printf("Output buffer isn't full. Data is not available for reading\n");
      //return;
    }

    //if status byte is alright
    else if (mouse_sys_inb(KBC_OUT_BUF, &mouse_data) == 1) {
      printf("Error on function mouse_sys_inb for out_buf\n");
    }
    else {
      return;
    }
    loopCounter++;
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return;
}

int(mouse_disable_dr)() {

  /*if (mouse_irq_disable() == 1) {
    return 1;
  } */
  if (kbc_write_to_mouse(MC_DIS_DATA_REP) == 1) {
    printf("Error on kbc_write_to_mouse\n");
    return 1;
  }
  return 0;
}

/**
 * Enables data reporting from the mouse
 */
int(mouse_enable_dr)() {
  /*
  if (mouse_irq_disable() == 1) {
    return 1;
  } */

  if (kbc_write_to_mouse(MC_EN_DATA_REP) == 1) {
    printf("Error on kbc_write_to_mouse\n");
    return 1;
  }

  return 0;
}

int(kbc_write_to_mouse)(uint8_t byte) {
  if (kbc_write_cmd_command(KBC_WRITE_MOUSE) == 1) {
    printf("Error sending write to mouse info to 0x64\n");
    return 1;
  }
  if (kbc_write_cmd_args(byte) == 1) {
    printf("Error writing to ARGS REG");
    return 1;
  }
  if (kbc_read_mouse_ack() != 0) { // checks if it can read, and then reads the acknowledge byte
    return 1;
  }

  return 0;
}

int(mouse_irq_disable)() {
  if (sys_irqdisable(&mouse_hook_id) != 0) {
    printf("Error disabling mouse irq\n");
    return 1;
  }
  return 0;
}

int(mouse_irq_enable)() {
  if (sys_irqenable(&mouse_hook_id) != 0) {
    printf("Error enabling mouse irq\n");
    return 1;
  }
  return 0;
}

int(mouse_and_timer_handle_interruption)(uint8_t bit_no, uint8_t timer_bit_no, uint8_t idle_time) {
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no); // mouse_irq
  uint32_t timer_irq_set = BIT(timer_bit_no);
  int packetByteCounter = 0;

  struct packet packet;
  while (counter < idle_time * 60) { // need to check this condition
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { //received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) { //  subscribed interrupt from the mouse
            mouse_ih();
            counter = 0; // reset counter to 0
            //printf("Counter Value: %d", packetByteCounter, "\n");
            switch (packetByteCounter) {
              case 0:
                if ((mouse_data & MOUSE_BYTE0_MASK) == 0) {
                  printf("Invalid byte 0 packet\n");
                  break;
                }
                else {
                  mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                  packetByteCounter++;
                  break;
                }
              case 1:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                packetByteCounter++;
                break;
              case 2:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                mouse_print_packet(&packet);
                packetByteCounter = 0;
                break;
            }
          }
          if (msg.m_notify.interrupts & timer_irq_set) { //in case it is an interruption from the timer
            //counter++;
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
  /*
  if (kbd_print_no_sysinb(kb_counter) != 0) {
    printf("Error in kbd_print_scancode");
  }
  */
  return 0;
}

enum state_machine st = INITIAL;

int(mouse_gesture_handle_interruption)(uint8_t bit_no, uint8_t x_len, uint8_t tolerance) {
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);

  int packetByteCounter = 0;

  struct packet packet;
  while (st != DONE) {
    int r;
    //printf("0\n");
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) { //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { //received notification
      //printf("0.5\n");
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             //hardware interrupt notification
                                                   //printf("1\n");
          if (msg.m_notify.interrupts & irq_set) { //  subscribed interrupt
            //printf("2\n");
            mouse_ih();
            //printf("Counter Value: %d", packetByteCounter, "\n");
            switch (packetByteCounter) {
              case 0:
                if ((mouse_data & MOUSE_BYTE0_MASK) == 0) {
                  printf("Invalid byte 0 packet\n");
                  continue;
                }
                else {
                  mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                  packetByteCounter++;
                  break;
                }
              case 1:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                packetByteCounter++;
                break;
              case 2:
                mouse_packets_parsing(&packet, mouse_data, packetByteCounter);
                validate_mouse_movement(&packet, x_len, tolerance);
                mouse_print_packet(&packet);
                //printf("State: %d %d %d", st, x_draw_len, y_draw_len);

                packetByteCounter = 0;
                break;
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

void(validate_mouse_movement)(struct packet *packet, uint8_t x_len, uint8_t tolerance) {
  switch (st) {
    case INITIAL:
      if (packet->lb && (!(packet->rb || packet->mb))) {
        st = LEFT_DRAW;
      }
      break;
    case LEFT_DRAW:
      if (!(packet->lb || packet->mb || packet->rb)) { //released the left button
        float slope = (float) y_draw_len / (float) x_draw_len;
        if (slope > 1) {
          if (x_draw_len >= x_len) { // me gusta this line
            st = VERTIX;
          }
          else {
            st = INITIAL;
          }
        }
        else {
          st = INITIAL;
        }
        x_draw_len = 0, y_draw_len = 0;
      }
      else {
        if (packet->mb || packet->rb) {
          st = INITIAL;
          x_draw_len = 0, y_draw_len = 0;
        }
        else if ((packet->delta_x >= -tolerance) && (packet->delta_y >= -tolerance)) { //check tolerance
          x_draw_len += packet->delta_x, y_draw_len += packet->delta_y;
        }
        else { // mouse movement not allowed. Will have to reset
          x_draw_len = 0, y_draw_len = 0;
          st = INITIAL;
        }
      }
      break;
    case VERTIX:
      if (packet->rb && (!(packet->lb || packet->mb))) { //if it detects a right click
        st = RIGHT_DRAW;
      }
      else if (packet->lb || packet->mb) { //back to beggining
        st = INITIAL;
      }

      if ((abs(packet->delta_x) > tolerance) || (abs(packet->delta_y) > tolerance)) {
        st = INITIAL;
      }
      break;
    case RIGHT_DRAW:
      if (!(packet->lb || packet->mb || packet->rb)) { //released the left button
        float slope = y_draw_len / x_draw_len;
        if (slope < -1) {
          if (x_draw_len >= x_len) { // me gusta this line
            st = DONE;
          }
          else {
            st = INITIAL;
          }
        }
        else {
          st = INITIAL;
        }
        x_draw_len = 0, y_draw_len = 0;
      }
      else {
        if (packet->lb || packet->mb) {
          st = INITIAL;
          x_draw_len = 0, y_draw_len = 0;
        }
        else if ((packet->delta_x >= -tolerance) && (packet->delta_y <= tolerance)) { //check tolerance
          x_draw_len += packet->delta_x, y_draw_len += packet->delta_y;
        }
        else { // mouse movement not allowed. Will have to reset
          x_draw_len = 0, y_draw_len = 0;
          st = INITIAL;
        }
      }
      break;
    case DONE:
      break;
  }
}

int(kbc_read_status)(uint8_t *status) {
  uint8_t counter = 0;
  while (counter < 10) {
    if (mouse_sys_inb(KBC_ST_REG, status) != 0) {
      printf("Error reading ST reg on mouse_sys_inb\n");
    }
    else if (((*status) & (KBC_PARITY_AND_TIMEOUT)) != 0) { //check if there is any parity or timeout error
      printf("Error on parity or timeout. Invalid data\n");
    }
    else{
      return 0;
    }
    counter++;
    tickdelay(micros_to_ticks(DELAY_US));
  }
  return 1;
}
