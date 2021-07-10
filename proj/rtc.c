#include "rtc.h"
#include "utils.h"
#include <lcom/lcf.h>
#include <machine/int86.h>

int writeRTC(uint8_t addr) {
  if (sys_outb(RTC_ADDR_REG, addr) != 0) {
    printf("Error on writeRTC, sys_outb\n");
    return 1;
  }
  return 0;
}

int convert_bcd_to_int(uint8_t value) {
  return (value & 0x0F) + (value >> 4) * 10;
}

int readRTCdate(Date *date) {
  uint8_t checkBit;
  writeRTC(REGISTER_A);
  while (1) {
    if (util_sys_inb(RTC_DATA_REG, &checkBit) != 0) {
      printf("Error on readRTC, util_sys_inb\n");
      return 1;
    }
    if (checkBit & UIP_BIT) { //can't read, will have an update
      continue;
    }
    else {
      break;
    }
  }
  uint8_t seconds, minutes, hours;

  //ler segundos
  writeRTC(SECONDS);
  if (util_sys_inb(RTC_DATA_REG, &seconds) != 0) { // read seconds from the data register
    printf("Error on readRTC, util_sys_inb hours\n");
    return 1;
  }

  //ler minutos
  writeRTC(MINUTES);
  if (util_sys_inb(RTC_DATA_REG, &minutes) != 0) {
    printf("Error on readRTC, util_sys_inb hours\n");
    return 1;
  }

  //ler horas
  writeRTC(HOURS);
  if (util_sys_inb(RTC_DATA_REG, &hours) != 0) {
    printf("Error on readRTC, util_sys_inb hours\n");
    return 1;
  }
  //parsing
  date->seconds = convert_bcd_to_int(seconds);
  date->minutes = convert_bcd_to_int(minutes);
  date->hours = convert_bcd_to_int(hours);
  return 0;
}

int writeRtcData(uint8_t data) {
  if (sys_outb(RTC_DATA_REG, data) != 0) {
    printf("Error on writeRtcData, sysoutb\n");
    return 1;
  }
  return 0;
}

int readRtcData(uint8_t *data) {
  if (util_sys_inb(RTC_DATA_REG, data) != 0) {
    printf("Error on readRTC, util_sys_inb\n");
    return 1;
  }
  return 0;
}

int rtc_hook_id;

int(rtc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = RTC_IRQ; //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
  rtc_hook_id = *bit_no;

  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &rtc_hook_id) != OK) { //subscribes the interruptions from the keybaord
    printf("sys_irqsetpolicy() failed\n");
    return 1;
  }

  return 0;
}

int(rtc_unsubscribe_int)(){
  if (sys_irqrmpolicy(&rtc_hook_id) != OK) {
    printf("Error in timer_unsubscribe_int, sys_irqrmpolicy\n");
    return 1;
  }

  return 0;
}

void setupUpdateInterrupts(interrupts intType){
  uint8_t regBData;

  if( writeRTC(REGISTER_B) != 0 ){
    printf("Error on writeRTC\n");
    return;
  }
  if( readRtcData(&regBData) != 0 ){
    printf("Error on readRtcData\n");
    return;
  }

  switch(intType){
    case PERIODIC_INT:
      regBData = regBData | PERIODIC_INTERRUPT;
      break;
    case ALARM_INT:
      regBData = regBData | ALARM_INTERRUPT;
      break;
    case UPDATE_INT:
      regBData = regBData | UPDATE_INTERRUPT;
      break;
    case CLEAN_INT:
      regBData = regBData & CLEAN_BITS;
      break;
  }

  if( writeRTC(REGISTER_B) != 0 ){
    printf("Error on writeRTC\n");
    return;
  }
  if( writeRtcData(regBData) != 0 ){
    printf("Error on writeRtcData\n");
    return;
  }
}

void rtc_ih(Date* date){
  uint8_t regCData;
  if( writeRTC(REGISTER_C) != 0 ){
    printf("Error on writeRtc\n");
    return;
  }
  if( readRtcData(&regCData) != 0 ){
    printf("Error on readRtcData\n");
    return;
  }
  if(regCData & PERIODIC_INTERRUPT_PENDING){
    printf("1\n");  
  }
  if(regCData & ALARM_INTERRUPT_PENDING){
    printf("2\n");
  }
  if(regCData & UPDATE_INTERRUPT_PENDING){
    //printf("3\n");
    readRTCdate(date);
  }

}

uint8_t timeBetweenDates(Date* d1, Date* d2){
  uint8_t totalSeconds = 0;
  totalSeconds += (d2->hours - d1->hours) * 3600;
  totalSeconds += (d2->minutes - d1->minutes) * 60;
  totalSeconds += (d2->seconds - d1->seconds);
  return totalSeconds;
}


