#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

extern uint64_t counter;  //usada no timer_handler();

int timer_hook_id;  //IRQ of timer0 (timer)

/**
 * Change the frequency of a timer
 @param timer timer that will change
 @param freq new timer's frequency
 @return 0 if succeed, 1 otherwise
 */ 
int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t controlWord, lsb, msb;
  uint32_t newControlWord;
  
  if( timer_get_conf(timer, &controlWord) != 0 ){       //reads the control word
    printf("ERROR: Invalid timer value\n");
    return 1;
  }

  uint8_t control_lsb = (BIT(0) | BIT(1) | BIT(2) | BIT(3)) & controlWord; //saves 4 lsb on control lsb

  uint32_t divisor = TIMER_FREQ / freq;

  if( util_get_LSB(divisor, &lsb) != 0){
    printf("Error in timer_set_frequency, util_get_LSB\n");
    return 1;
  };
  if( util_get_MSB(divisor, &msb) != 0){
    printf("Error in timer_set_frequency, util_get_MSB\n");
    return 1;
  }
  switch (timer) {
    case 0:
      newControlWord = control_lsb | TIMER_SEL0 | TIMER_LSB_MSB;
      if (sys_outb(TIMER_CTRL, newControlWord) != 0){
        printf("Error in timer_set_frequency, timer_0 ctrl sys_outb\n");
        return 1;
      } //writes the new control word on TIMER_CTRL
      if(sys_outb(TIMER_0, lsb)!=0){
        printf("Error in timer_set_frequency, timer_0 lsb sys_outb\n");
        return 1;
      };               //writes lsb of freq to TIMER_0
      if(sys_outb(TIMER_0, msb) != 0){ 
        printf("Error in timer_set_frequency, timer_0 msb sys_outb\n");
        return 1;
      }               //writes msb of freq to TIMER_0
      break;  
    case 1:
      newControlWord = control_lsb | TIMER_SEL1 | TIMER_LSB_MSB;
      if(sys_outb(TIMER_CTRL, newControlWord)!=0){
        printf("Error in timer_set_frequency, timer_1 ctrl sys_outb\n");
        return 1;
      } //writes the new control word
      if(sys_outb(TIMER_1, lsb) != 0){
        printf("Error in timer_set_frequency, timer_1 lsb sys_outb\n");
        return 1;
      }
      if(sys_outb(TIMER_1, msb) != 0){
        printf("Error in timer_set_frequency, timer_1 msb sys_outb\n");
        return 1;
      }
      break;
    case 2:
      newControlWord = control_lsb | TIMER_SEL2 | TIMER_LSB_MSB;
      if(sys_outb(TIMER_CTRL, newControlWord) != 0){
        printf("Error in timer_set_frequency, timer_2 ctrl sys_outb\n");
        return 1;
      } //writes the new control word
      if(sys_outb(TIMER_2, lsb) != 0){
        printf("Error in timer_set_frequency, timer_2 lsb sys_outb\n");
        return 1;
      }
      if(sys_outb(TIMER_2, msb) != 0){
        printf("Error in timer_set_frequency, timer_2 msb sys_outb\n");
        return 1;
      }
      break;
    default: 
      printf("No corresponding timer, wrong timer\n");
      return 1;
  }

  return 0;
}


/**
  *Subscribes timer0 for interrupts
  @param bit_no variable that will get the value for interrupts notifications
  @return 0 if success, 1 otherwise
*/
int(timer_subscribe_int)(uint8_t *bit_no) {  
  timer_hook_id = TIMER0_IRQ; //global variable
  *bit_no = (uint8_t) timer_hook_id;  

  if( sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK ){   //subscribe as interrupções
    printf("sys_irqsetpolicy() failed\n");
    return 1;
  };
  
  return 0;
}

/**
  *unsubscribes an interrupt
  @return 0 if success, 1 otherwise
*/
int(timer_unsubscribe_int)() {
  if ( sys_irqrmpolicy(&timer_hook_id) != OK){
    printf("Error in timer_unsubscribe_int, sys_irqrmpolicy\n");
    return 1;
  }

  return 0;
}

/**
 * Increments global counter variable
 */ 
void(timer_int_handler)() {
  counter++;
}

/**
 * reads a timer configuration into st by changing the control word to read back
 @param timer timer that will be read
 @param pointer variable that will get the config
 @return returns 0 if succeed, 1 otherwise
 */
int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  //CONTROL BACK REGISTER
  uint32_t rbControlByte;
  switch (timer) {
    case 0:
      rbControlByte = TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(1);
      if(sys_outb(TIMER_CTRL, rbControlByte) != OK){   // escreve o byte de controlo (controlByte) na port desejada
        printf("Error in timer_get_conf, sys_outb\n");
      } 
      util_sys_inb(TIMER_0, st);           // lê o byte da port 1º arg para st
      break;
    case 1:
      rbControlByte = TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(2);
      if(sys_outb(TIMER_CTRL, rbControlByte) != OK){
        printf("Error in timer_get_conf, sys_outb\n");
      }
      util_sys_inb(TIMER_1, st);
      break;
    case 2:
      rbControlByte = TIMER_RB_CMD | TIMER_RB_COUNT_ | BIT(3);
      if(sys_outb(TIMER_CTRL, rbControlByte) != OK){
        printf("Error in timer_get_conf, sys_outb\n");
      }
      util_sys_inb(TIMER_2, st);
      break;
    default:
      return 1;
  }
  //printf("value stored in st: %x", *st, "\n");
  /* To be implemented by the students */
  //printf("%s is not yet implemented!\n", __func__);

  return 0;
}

/**
 * Prints the config of a certain timer
 @param timer timer that will be read
 @param st byte returned from read back command
 @param field enum with the fields that will be printed
 @return 0 if succeed, 1 otherwise
 */ 
int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  uint8_t countMode = (BIT(1) | BIT(2) | BIT(3)) & st;

  bool ctBase = BIT(0) & st; //true if Binary coded decimal

  uint8_t initMode = (BIT(4) | BIT(5)) & st;

  countMode = countMode >> 1; // need to shift so the number start at bit 0
  initMode = initMode >> 4;

  enum timer_init in_mode; //enum for the init mode

  switch (initMode) {
    case 1:
      in_mode = LSB_only;
      break;
    case 2:
      in_mode = MSB_only;
      break;
    case 3:
      in_mode = MSB_after_LSB;
      break;
    case 0:
      in_mode = INVAL_val;
      break;
  }
  union timer_status_field_val conf;
  switch (field) {
    case tsf_all:
      conf.byte = st;
      timer_print_config(timer, field, conf);
      break;
    case tsf_initial:
      conf.in_mode = in_mode;
      timer_print_config(timer, field, conf);
      break;
    case tsf_mode:
      if (countMode == 2 || countMode == 6) {
        conf.count_mode = 2;
      }
      else if (countMode == 3 || countMode == 7) {
        conf.count_mode = 3;
      }
      else {
        conf.count_mode = countMode;
      }
      timer_print_config(timer, field, conf);
      break;
    case tsf_base:
      conf.bcd = ctBase;
      timer_print_config(timer, field, conf);
      break;
    default:
      return 1;
  }

  /* To be implemented by the students */
  //printf("%s is not yet implemented!\n", __func__);

  return 0;
}
