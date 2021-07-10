#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

uint64_t counter = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  /* To be implemented by the students */
  uint8_t st;
  timer_get_conf(timer, &st);

  timer_display_conf(timer, st, field);

  //printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  return timer_set_frequency(timer, freq);

  /* To be implemented by the students */
  //printf("%s is not yet implemented!\n", __func__);
}

/**
 * Subscribes timer0 for interruptions and handles them, by incrementing a global variable and printing time each second
 * @param time run time of this functions
 * @return 0 if success, 1 otherwise
 */ 
int(timer_test_int)(uint8_t time) {
  uint8_t prevHook; 
  if (timer_subscribe_int(&prevHook) != 0){
    printf("timer_subscribe_int failed!");
    return 1;
  };

  //interrupt loop to receive notifications
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(prevHook);

  while(counter < time * 60){
    int r;
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ){    //get a request message
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if(is_ipc_notify(ipc_status)){    //received notification
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE:                //hardware interrupt notification
          if(msg.m_notify.interrupts & irq_set){  //  subscribed interrupt
            timer_int_handler();
            if (counter % 60 == 0) {
              timer_print_elapsed_time();
            }
          }
          break;
        default:
          break;    // no other notifications expected
      }
    }
    else{
      //received a standard message, not a notification
    }
  }

  if( timer_unsubscribe_int() != 0){
    printf("timer_unsubscribe_int failed!");
    return 1;
  };

  return 0;
}

