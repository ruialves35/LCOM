// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <minix/syslib.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "i8042.h"
#include "mouse.h"
#include "specific_functions.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt) { //cnt = no_packets
  uint8_t bit_no;

  /*
  if (mouse_enable_data_reporting() == 1) {
    printf("Error on mouse_enable_dr\n");
    return 1;
  }  */

  if (mouse_enable_dr() == 1) { //since it is implemented before subscribing, I dont need to disable IRQ
    printf("Error on mouse_enable_dr\n");
    return 1;
  }

  if (mouse_subscribe_int(&bit_no) == 1) { //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
    printf("Error on ps2_subscribe_int\n");
    return 1;
  };

  mouse_handle_interruption(bit_no, cnt);

  if (mouse_unsubscribe_int() == 1) {
    printf("Error on ps2_unsubscribe_int\n");
    return 1;
  }

  if (mouse_disable_dr() == 1) {
    printf("Error disabling data report\n");
    return 1;
  };

  /* To be completed */
  //printf("%s(%u): under construction\n", __func__, cnt);
  return 1;
}

int(mouse_test_async)(uint8_t idle_time) {
  uint8_t bit_no;
  uint8_t timer_bit_no;

  /*
  if (mouse_enable_data_reporting() == 1) {
    printf("Error on mouse_enable_dr\n");
    return 1;
  }  */

  if (mouse_enable_dr() == 1) { //since it is implemented before subscribing, I dont need to disable IRQ
    printf("Error on mouse_enable_dr\n");
    return 1;
  }

  if (mouse_subscribe_int(&bit_no) == 1) { //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
    printf("Error on ps2_subscribe_int\n");
    return 1;
  };

  /*
  if (mouse_enable_dr() == 1) {
    printf("Error on mouse_enable_dr\n");
    return 1;
  } */

  if (timer_subscribe_int(&timer_bit_no) == 1) { //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
    printf("Error on ps2_subscribe_int\n");
    return 1;
  };

  //timer_set_frequency(TIMER0_IRQ, sys_hz());

  mouse_and_timer_handle_interruption(bit_no, timer_bit_no, idle_time); //output buffer isn't full for some reason

  if (mouse_unsubscribe_int() == 1) {
    printf("Error on ps2_unsubscribe_int\n");
    return 1;
  }

  if (timer_unsubscribe_int() == 1) {
    printf("Error on timer_unsubscribe_int");
    return 1;
  };

  if (mouse_disable_dr() == 1) {
    printf("Error disabling data report\n");
    return 1;
  };

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  uint8_t bit_no;

  if (mouse_enable_dr() == 1) { //since it is implemented before subscribing, I dont need to disable IRQ
    printf("Error on mouse_enable_dr\n");
    return 1;
  }

  if (mouse_subscribe_int(&bit_no) == 1) { //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
    printf("Error on ps2_subscribe_int\n");
    return 1;
  };

  mouse_gesture_handle_interruption(bit_no, x_len, tolerance);

  if (mouse_unsubscribe_int() == 1) {
    printf("Error on ps2_unsubscribe_int\n");
    return 1;
  }

  if (mouse_disable_dr() == 1) {
    printf("Error disabling data report\n");
    return 1;
  };

  return 0;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
  uint8_t no_packets_read = 0;
  uint8_t curr_packet;
  int packetByteCounter = 0;
  struct packet packet;

  while (no_packets_read < cnt) {
    if (packetByteCounter == 0){
      if (kbc_write_to_mouse(MC_READ_DATA) != 0) {
        printf("Error on kbc_write_to_mouse\n");
      }
    }
    if (kbc_read_mouse_data_by_poll(&curr_packet) != 0) {
      printf("Error on kbc_read_cmd_return\n");
    }

    switch (packetByteCounter) {
      case 0:
        if ((curr_packet & MOUSE_BYTE0_MASK) == 0) {
          printf("Invalid byte 0 packet\n");
          break;
        }
        else {
          mouse_packets_parsing(&packet, curr_packet, packetByteCounter);
          packetByteCounter++;
          break;
        }
      case 1:
        mouse_packets_parsing(&packet, curr_packet, packetByteCounter);
        packetByteCounter++;
        break;
      case 2:
        mouse_packets_parsing(&packet, curr_packet, packetByteCounter);
        mouse_print_packet(&packet);
        //printf("State: %d %d %d", st, x_draw_len, y_draw_len);
        packetByteCounter = 0;
        printf("oioio %d \n", no_packets_read);
        no_packets_read++;
        break;
    }

    /* Wait 'period' miliseconds (must convert to microseconds, hence the *1000) */
    tickdelay(micros_to_ticks(period * 1000));
  }

  printf("5");
  if (kbc_write_to_mouse(MC_SET_STREAM_MODE) != 0) {
    printf("Error on kbc_write_to_mouse()\n");
  }
  printf("6");
  if (mouse_disable_dr() != 0) {
    printf("Error on mouse_disable_dr()\n");
  }

  printf("oi");
  if (kbc_write_cmd_byte(minix_get_dflt_kbc_cmd_byte()) != 0) {
    printf("Error on kbc_write_cmd_byte()\n");
  }
  printf("8");
  return 0;
}
