#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

// IRQ 
#define RTC_IRQ 8

//DATES REGISTERS
#define SECONDS 0x00
#define SECONDS_ALARM 0x01
#define MINUTES 0x02
#define MINUTES_ALARM 0x03
#define HOURS 0x04
#define HOURS_ALARM 0x05
#define DAY_WEEK 0x06
#define DAY_MONTH 0x07
#define MONTH 0x08
#define YEAR 0x09

//REGISTERS 
#define REGISTER_A 10
#define REGISTER_B 11
#define REGISTER_C 12
#define REGISTER_D 13

#define UIP_BIT BIT(7)  //checks if the time is gonna be updated

// REGISTER B BITS
#define PERIODIC_INTERRUPT BIT(6)
#define ALARM_INTERRUPT BIT(5)
#define UPDATE_INTERRUPT BIT(4)
#define CLEAN_BITS 0x8F

// REGISTER C BITS
#define PERIODIC_INTERRUPT_PENDING BIT(6)
#define ALARM_INTERRUPT_PENDING BIT(5)
#define UPDATE_INTERRUPT_PENDING BIT(4)

/**
 * @struct Date
 * @brief Struct that stores a Date (hours / min / seconds)
 * @var seconds seconds 
 * @var minutes minutes
 * @var hours hours
 */
typedef struct{
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
} Date;

/**
 * @brief Enum that indicates the type of interruption the RTC is set on
 * 
 */
typedef enum{
  PERIODIC_INT,
  ALARM_INT,
  UPDATE_INT,
  CLEAN_INT,
} interrupts;

//int readRegister(uint8_t register, uint8_t* value);

/**
 * loads RTC address Register with the address of the RTC register to be accessed
 * Return 0 if success, 1 otherwise
 */
int writeRTC(uint8_t addr);

/**
 * Reads RTC_DATA_REG to get information 
 */
int readRTCdate(Date* date);

/**
 * Writes data to RTC data register
 */ 
int writeRtcData(uint8_t data);

/**
 * @brief Reads data from the rtc to the var data
 * 
 * @param data 
 * @return int 
 */
int readRtcData(uint8_t* data);

/**
 * Convert bcd to int
 */
int convert_bcd_to_int (uint8_t value);

/**
 * @brief Subscribes to RTC interrupts
 * @arg bit_no pointer to an 8 bit variable that will hold the bit mask of the rtc irq
 * @return returns 0 upon success, 1 otherwise
 */
int(rtc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes the rtc interruptions
 * @return returns 0 upon success, 1 otherwise
 */
int(rtc_unsubscribe_int)();

/**
 * @brief Updates Register B to enable interrupt given as argument
 * @arg intType type of interrupt to enable
 */
void setupUpdateInterrupts(interrupts intType);

/**
 * @brief handles rtc interrupt
 * 
 * @param date paramater that will be updated
 */
void rtc_ih(Date* date);

/**
 * @brief Calculate time between 2 dates
 * 
 * @param d1 startDate
 * @param d2 endDate
 * @return uint8_t time between the 2 dates
 */
uint8_t timeBetweenDates(Date* d1, Date* d2);

#endif
