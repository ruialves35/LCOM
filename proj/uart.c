#include <lcom/lcf.h>
#include <machine/int86.h>

#include <stdint.h>
#include <stdio.h>

#include "queue.h"
#include "uart.h"
#include "utils.h"

//static Queue * transmitterQueue;
//static Queue * receiverQueue;

extern uint8_t uartSend;
extern uint8_t uartReceive;
extern uint8_t receivedScore;

int uart_hook_id;

int(uart_subscribe_int)(uint8_t *bit_no) {
  *bit_no = UART_COM1_IRQ; //bit_no contains the bit before being changed from sys_irqsetpolicy() and hookid contains the value afterwards
  uart_hook_id = *bit_no;

  if (sys_irqsetpolicy(UART_COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &uart_hook_id) != OK) { //subscribes the interruptions from the keybaord
    printf("sys_irqsetpolicy() failed\n");
    return 1;
  }

  return 0;
}

int(uart_unsubscribe_int)() {
  if (sys_irqrmpolicy(&uart_hook_id) != OK) {
    printf("Error in uart_unsubscribe_int, sys_irqrmpolicy\n");
    return 1;
  }

  return 0;
}

int uart_write_reg(uint8_t regNum, uint8_t byte) {
  if (regNum > 7) {
    return 1;
  }
  int addr = UART_COM1_BASE_ADDR + regNum; // since value is in hexadecimal there is no need to multiply by 8
  if (sys_outb(addr, byte) != 0) {
    printf("Error writing to the addr on uart_write_reg\n");
    return 1;
  }
  return 0;
}

int uart_read_reg(uint8_t regNum, uint8_t *variable) {
  if (regNum > 7) {
    return 1;
  }
  int addr = UART_COM1_BASE_ADDR + regNum;
  if (util_sys_inb(addr, variable) != 0) {
    printf("Error reading from the addr on uart_read_reg\n");
    return 1;
  }
  return 0;
}

void uart_setup_lcr(uint8_t wordLength, uint8_t stopBits, parityTypes parity) {
  uint8_t prevLcr;
  if (uart_read_reg(UART_LCR, &prevLcr) != 0) {
    return;
  }
  uint8_t wordLengthBits, stopBitsBits, parityBits;
  switch (wordLength) {
    case 5:
      wordLengthBits = UART_LCR_5BPC;
      break;
    case 6:
      wordLengthBits = UART_LCR_6BPC;
      break;
    case 7:
      wordLengthBits = UART_LCR_7BPC;
      break;
    case 8:
      wordLengthBits = UART_LCR_8BPC;
      break;
  }
  if (stopBits == 1) {
    stopBitsBits = UART_LCR_1_STOPBIT;
  }
  else {
    stopBitsBits = UART_LCR_2_STOPBIT;
  }
  switch (parity) {
    case PARITY_NONE:
      parityBits = UART_LCR_NO_PARITY;
      break;
    case PARITY_ODD:
      parityBits = UART_LCR_ODD_PARITY;
      break;
    case PARITY_EVEN:
      parityBits = UART_LCR_EVEN_PARITY;
      break;
    default:
      parityBits = UART_LCR_NO_PARITY;
      break;
  }
  uint8_t newLcr = (prevLcr & (UART_LCR_BREAK_CTRL | UART_LCR_DLAB)) | wordLengthBits | stopBitsBits | parityBits;
  if (uart_write_reg(UART_LCR, newLcr) != 0) {
    return;
  }
}

void uart_set_dlb(bool dlb) {
  uint8_t prevLcr;
  if (uart_read_reg(UART_LCR, &prevLcr) != 0) {
    return;
  }
  uint8_t dlbBit;
  if (dlb) {
    dlbBit = UART_LCR_DLAB;
  }
  else {
    dlbBit = 0;
  }
  uint8_t newLcr = (prevLcr & UART_LCR_OPPOSITE_MASK) | dlbBit;
  if (uart_write_reg(UART_LCR, newLcr) != 0) {
    return;
  }
}

void uart_en_interrupts() {
  uint8_t prevByte;
  if (util_sys_inb(UART_COM1_BASE_ADDR + UART_IER, &prevByte) != 0) {
    return;
  }
  prevByte = prevByte | UART_IER_RCVD_DATA_INT /*| UART_IER_THRE_INT*/ | UART_IER_ERROR_INT;

  if (sys_outb(UART_COM1_BASE_ADDR + UART_IER, prevByte) != 0) {
    return;
  }
}

void uart_set_bit_rate(int bit_rate) {
  uart_set_dlb(true);
  uint16_t quotient = UART_DL_BITRATE_DIVISOR / bit_rate;
  uint8_t lsb_quotient = UART_BIT_RATE_LSB(quotient);
  uint8_t msb_quotient = UART_BIT_RATE_MSB(quotient);
  if (uart_write_reg(UART_DLL, lsb_quotient) != 0) {
    return;
  }
  if (uart_write_reg(UART_DLM, msb_quotient) != 0) {
    return;
  }
  uart_set_dlb(false);
}

void uart_en_fifo(uint8_t triggerLevel) {
  uint8_t triggerLevelBits;
  switch (triggerLevel) {
    case 1:
      triggerLevelBits = UART_FCR_TRIGGER_1;
      break;
    case 4:
      triggerLevelBits = UART_FCR_TRIGGER_4;
      break;
    case 8:
      triggerLevelBits = UART_FCR_TRIGGER_8;
      break;
    case 14:
      triggerLevelBits = UART_FCR_TRIGGER_14;
      break;
    default:
      printf("Invalid trigger level\n");
      return;
  }

  uint8_t fcr_byte;
  if (uart_read_reg(UART_FCR, &fcr_byte) != 0) {
    return;
  }

  fcr_byte = (UART_FCR_EN | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | triggerLevelBits) | (fcr_byte & (UART_L6SB_MASK));
  if (uart_write_reg(UART_FCR, fcr_byte) != 0) {
    return;
  }
}

int uart_send_message(uint8_t message) {
  int sendCounter = 0;
  while (sendCounter < 10) {
    if (uart_check_send()) {
      if (uart_write_reg(UART_THR, message) != 0) {
        sendCounter++;
      }
      else {
        printf("Message sent\n");
        return 0;
      }
    }
    tickdelay(micros_to_ticks(100000));
  }
  // CHECK ACKNOWLEDGMENT BYTE THAT WILL BE SENT BY THE PC WHO RECEIVES THE MESSAGE

  return 1;
}

int uart_fifo_send_message(Queue *queue) {
  int sendCounter = 0;
  while (sendCounter < 10) {
    //printf("Sending message... ");
    if (queueIsEmpty(queue)) {
      printf("Cant send message since transmitterQueue is empty\n");
      return 1;
    }
    if (uart_write_reg(UART_THR, queueFront(queue)) != 0) {
      sendCounter++;
    }
    else {
      printf("Message sent\n");
      return 0;
    }
    tickdelay(micros_to_ticks(100000));
  }
  // CHECK ACKNOWLEDGMENT BYTE THAT WILL BE SENT BY THE PC WHO RECEIVES THE MESSAGE

  return 1;
}

int uart_read_message(uint8_t *message) {
  int readCounter = 0;
  while (readCounter < 10) {
    if (uart_read_reg(UART_RBR, message) != 0) {
      readCounter++;
    }
    else {
      if ((*message) % 10 == 0) {
        receivedScore = *message;
      }
      return 0;
    }
    tickdelay(micros_to_ticks(100000));
  }

  return 1;
}

int uart_fifo_read_message(Queue *queue) {
  //int readCounter = 0;
  uint8_t message = 0;
  while(true){
    uint8_t lsr;
    uart_read_reg(UART_LSR, &lsr);
    if(lsr & UART_LSR_RCV_DATA){
      uart_read_reg(UART_RBR, &message);
      addToQueue(queue, message);
      printf("Reading byte ... %u\n", message);
    }
    else{
      break;
    }
  }

  return 0;
}

bool uart_check_send() {
  uint8_t lsr;
  if (uart_read_reg(UART_LSR, &lsr) != 0) {
    return false;
  }
  if ((lsr & UART_LSR_THRE) && ~(lsr & UART_LSR_RCV_DATA)) {
    return true;
  }
  else {
    return false;
  }
}

bool uart_check_read() {
  uint8_t lsr;
  if (uart_read_reg(UART_LSR, &lsr) != 0) {
    return false;
  }
  if (lsr & UART_LSR_RCV_DATA) {
    return true;
  }
  return false;
}

void uart_ih(Queue* receiverBuffer) {
  uint8_t interruptType;
  if (uart_read_reg(UART_IIR, &interruptType) != 0) {
    printf("error reading reg on uart_ih\n");
    return;
  }

  if (interruptType & UART_IIR_NO_INT) {
    printf("There is no interrupt pending\n");
    return;
  }

  if (interruptType & UART_IIR_RDA) {
    //uart_read_message(&uartReceive);
    uart_fifo_read_message(receiverBuffer);
    //printf("Message read: %d\n", uartReceive);
  }
  /*
  else if (interruptType & UART_IIR_THRE) {
    printf("Available to write message...\n");
    //uart_send_message(uartSend);
  }*/
  else if (interruptType & UART_IIR_RLS) {
    uint8_t lsr;
    uart_read_reg(UART_LSR, &lsr);
    if (lsr & UART_LSR_OVR_ERROR) {
      printf("Character on uart was overwritten by another\n");
    }
    else if (lsr & UART_LSR_PAR_ERROR) {
      printf("uart parity error\n");
    }
    else if (lsr & UART_LSR_FRM_ERROR) {
      printf("uart framing error\n");
    }
    else if (lsr & UART_LSR_BRK_INT) {
      printf("uart break interrupt error\n");
    }
    else if (lsr & UART_LSR_FIFO_ERROR) {
      printf("uart fifo error\n");
    }
    // STILL HAVE TO CHECK THIS
  }
}
