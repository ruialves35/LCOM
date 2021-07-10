#ifndef UART_H
#define UART_H

#include <lcom/lcf.h>
#include "queue.h"

#define UART_COM1_IRQ            4        /**< @brief UART COM1 IRQ Line  */
#define UART_COM2_IRQ            3        /**< @brief UART COM2 IRQ Line  */

#define UART_DL_BITRATE_DIVISOR  115200    /**< Divisor latch value must be set to 115200/bit_rate to set the bit_rate */
#define DEFAULT_BIT_RATE 9600

#define UART_BIT_RATE_LSB(bit_rate) (  (bit_rate) & 0x00FF)
#define UART_BIT_RATE_MSB(bit_rate) ( ((bit_rate) & 0xFF00) >> 8 )

//////////////////////////
//  I/O port addresses  //
//////////////////////////

#define UART_COM1_BASE_ADDR    0x3F8 /**< @brief UART's COM1 base address register */
#define UART_COM2_BASE_ADDR    0x2F8 /**< @brief UART's COM2 base address register */

///////////////////////////////////////////////////
//  UART's accessible 8-bit Registers (offsets)  //
///////////////////////////////////////////////////

#define UART_RBR  0    /**< @brief Receiver Buffer Register */
#define UART_THR  0    /**< @brief Transmitter Holding Register */
#define UART_IER  1    /**< @brief Interrupt Enable Register */
#define UART_IIR  2    /**< @brief Interrupt Identification Register */
#define UART_FCR  2    /**< @brief FIFO Control Register */
#define UART_LCR  3    /**< @brief Line Control Register */
#define UART_MCR  4    /**< @brief Modem Control Register */
#define UART_LSR  5    /**< @brief Line Status Register */
#define UART_MSR  6    /**< @brief Modem Status Register */
#define UART_SR   7    /**< @brief Scratchpad Register */

#define UART_DLL  0    /**< @brief Divisor Latch LSB */
#define UART_DLM  1    /**< @brief Divisor Latch MSB */


///////////////////////////////////
//  Line Control Register (LCR)  //
///////////////////////////////////

/* Number of Bits per Char */
#define UART_LCR_5BPC   0                           /**< @brief 5 bits per char */
#define UART_LCR_6BPC   BIT(0)                      /**< @brief 6 bits per char */
#define UART_LCR_7BPC   BIT(1)                      /**< @brief 7 bits per char */
#define UART_LCR_8BPC   (BIT(0) | BIT(1))           /**< @brief 8 bits per char */

/* Number of Stop Bits */
#define UART_LCR_1_STOPBIT  0                       /**< @brief 1 stop bit */
#define UART_LCR_2_STOPBIT  BIT(2)                  /**< @brief 2 stop bits (1 and 1/2 when 5 bits char) */

/* Parity Control */
#define UART_LCR_NO_PARITY    0                     /**< @brief No parity */
#define UART_LCR_ODD_PARITY   BIT(3)                /**< @brief Odd parity */
#define UART_LCR_EVEN_PARITY  (BIT(3) | BIT(4))     /**< @brief Even parity */

/* Break Control */
#define UART_LCR_BREAK_CTRL   BIT(6)                /**< @brief Break control: sets serial output to 0 (low) */

/* Divisor Latch Access */
#define UART_LCR_DLAB   BIT(7)                      /**< @brief 0 - Divisor Latch Access / 1 - RBR (read) or THR (write) */
#define UART_LCR_OPPOSITE_MASK BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6)


//////////////////////////////////
//  Line Status Register (LSR)  //
//////////////////////////////////

#define UART_LSR_RCV_DATA    BIT(0)    /**< @brief Set to 1 when there is data for receiving */
#define UART_LSR_OVR_ERROR   BIT(1)    /**< @brief Set to 1 when a characters received is overwritten by another one */
#define UART_LSR_PAR_ERROR   BIT(2)    /**< @brief Set to 1 when a character with a parity error is received */
#define UART_LSR_FRM_ERROR   BIT(3)    /**< @brief Set to 1 when a received character does not have a valid Stop bit */
#define UART_LSR_BRK_INT     BIT(4)    /**< @brief Set to 1 when the serial data input line is held in the low level for longer than a full “word” transmission */
#define UART_LSR_THRE        BIT(5)    /**< @brief When set, means that the UART is ready to accept a new character for transmitting */
#define UART_LSR_TER         BIT(6)    /**< @brief When set, means that both the THR and the Transmitter Shift Register are both empty */
#define UART_LSR_FIFO_ERROR  BIT(7)    /**< @brief Set to 1 when there is at least one parity error or framing error or break indication in the FIFO Reset to 0 when the LSR is read, if there are no subsequent errors in the FIFO */


///////////////////////////////////////
// Interrupt Enabler Register (IER)  //
///////////////////////////////////////

#define UART_IER_RCVD_DATA_INT   BIT(0)   /**< @brief Enables the Received Data Available Interrupt */
#define UART_IER_THRE_INT        BIT(1)   /**< @brief Enables the Transmitter Holding Register Empty Interrupt */
#define UART_IER_ERROR_INT       BIT(2)   /**< @brief Enables the Receiver Line Status Interrupt This event is generated when there is a change in the state of bits 1 to 4, i.e. the error bits, of the LSR */
#define UART_IER_MODEM_STAT_INT  BIT(3)   /**< @brief Enables the MODEM Status Interrupt */


//////////////////////////////////////////////
// Interrupt Identification Register (IIR)  //
//////////////////////////////////////////////

#define UART_IIR_NO_INT          BIT(0)              /**< @brief If set, no interrupt is pending */
#define UART_IIR_RLS             (BIT(1) | BIT(2))   /**< @brief Receiver Line Status */
#define UART_IIR_RDA             BIT(2)              /**< @brief Received Data Available */
#define UART_IIR_FIFO_C_TIMEOUT  (BIT(2) | BIT(3))   /**< @brief Character Timeout (FIFO) */
#define UART_IIR_THRE            BIT(1)              /**< @brief Transmitter Holding Register Empty */
#define UART_IIR_MODEM_STATUS    0                   /**< @brief Modem Status */
#define UART_INT_ID_MASK         (BIT(1) | BIT(2) | BIT(3))


//////////////////////////////////////////////
// FIFO CONTROL REGISTER (FCR)             //
//////////////////////////////////////////////

#define UART_FCR_EN BIT(0)
#define UART_FCR_CLEAR_RCVR BIT(1)
#define UART_FCR_CLEAR_XMIT BIT(2)
#define UART_FCR_TRIGGER_1 0
#define UART_FCR_TRIGGER_4 BIT(6)
#define UART_FCR_TRIGGER_8 BIT(7)
#define UART_FCR_TRIGGER_14 BIT(6) | BIT(7)

//////////////////////
// BIT MASKS  ////////
//////////////////////

#define UART_5LSB_MASK BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4)
#define UART_L6SB_MASK BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)


typedef enum {
  PARITY_NONE,
  PARITY_ODD,
  PARITY_EVEN,
  PARITY_1,
  PARITY_0,
} parityTypes;

///////////////////////////////////////////////////
//  CONSTANTS FOR COMMUNICATION BETWEEN PLAYERS  //
///////////////////////////////////////////////////

#define UART_LVL1_COMPLETE 3
#define UART_LVL2_COMPLETE 6
#define UART_WON 9
#define UART_LVL1_LOSE 12
#define UART_LVL2_LOSE 15
#define UART_LVL3_LOSE 18
#define UART_PLAYER_JOINED 7
#define UART_ON_SCOREBOARD 17

#define UART_UPDATE_TIME 90



/**
 * @brief Subscribes interrupts from the serial port
 * @arg bit_no variable that will hold the bit mask for the identifying interrupts
 */
int(uart_subscribe_int)(uint8_t* bit_no);

/**
 * @brief Unsubscribes interrupts from the serial port
 */
int(uart_unsubscribe_int)();

/**
 * @brief writes in the uart reg given as the arg the byte
 * 
 * @param regNum reg where we are going to write
 * @param byte byte to write
 * @return int 
 */
int uart_write_reg(uint8_t regNum, uint8_t byte);

/**
 * @brief Reads from the reg given as argument
 * 
 * @param regNum reg where we are going to read from
 * @param variable variable that will store the value
 * @return int 
 */
int uart_read_reg(uint8_t regNum, uint8_t* variable);

/**
 * @brief changes the line control register byte in accordance to the args (Except BIT 6 and 7)
 * 
 * @param wordLength bits per char
 * @param stopBits  nº stop bits
 * @param parity    type of parity
 */
void uart_setup_lcr(uint8_t wordLength, uint8_t stopBits, parityTypes parity);

/**
 * @brief Enables divisor latch access on the Line Control Reg. if dlb is true
 * 
 * @param dlb indicates whether to turn dlb on / off
 */
void uart_set_dlb(bool dlb);

/**
 * @brief Enables the desired interrupts 
 * 
 */
void uart_en_interrupts();

/**
 * @brief Set the bitrate to the one specified as the argument
 * 
 * @param bit_rate
 */
void uart_set_bit_rate(int bit_rate);

/**
 * @brief ENABLES FIFO, clears Transmitter and Receiver FIFO and sets trigger level
 * 
 * @param triggerLevel
 */
void uart_en_fifo(uint8_t triggerLevel);

/**
 * @brief Sends a message through the serial port
 * 
 * @param message 
 * @return int 0 if succesfull, 1 otherwise
 */
int uart_send_message(uint8_t message);

/**
 * @brief Sends a message through the serial port to the queue
 * 
 * @param message 
 * @return int 0 if succesfull, 1 otherwise
 */
int uart_fifo_send_message(Queue* queue);

/**
 * @brief Reads a message through the serial port and inserts on the queue
 * 
 * @param queue 
 * @return int 0 if succesfull, 1 otherwise
 */
int uart_fifo_read_message(Queue* queue);

/**
 * @brief Reads a message through the serial port
 * 
 * @param message 
 * @return int 0 if succesfull, 1 otherwise
 */
int uart_read_message(uint8_t* message);

/**
 * @brief Checks if the user can send a new message
 * 
 * @return true if available, false otherwise
 */
bool uart_check_send();

/**
 * 
 * @brief Checks if the user can read a new message
 * 
 * @return true if available, false otherwise
 */
bool uart_check_read();

/**
 * @brief handles the uart interrupts
 * 
 */
void uart_ih();


#endif
