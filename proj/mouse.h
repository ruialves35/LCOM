#ifndef MOUSE_H
#define MOUSE_H

/**
 * Reads a byte from the output buffer after checking for errors
 */
void (mouse_ih)();

/**
 * @brief Subscribes mouse interrupts
 * 
 */
int(mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes mouse interrupts
 * 
 */
int(mouse_unsubscribe_int)();

/**
 * @brief Handles mouse interruptions
 * 
 */
int(mouse_handle_interruption)(uint8_t bit_no, uint32_t cnt);

/**
 * @brief Parses the packet received from the mouse
 * 
 */
void(mouse_packets_parsing)(struct packet* packet, uint8_t byte, int byte_no);

/**
 * Disables data report
 */ 
int(mouse_disable_dr)();

/**
 * Enables data report
 */ 
int(mouse_enable_dr)();

/**
 * Reads the command byte to status
 */ 
int(kbc_read_cmd_byte)(uint8_t *status);

/**
 * Write a new command byte
 */ 
int(kbc_write_cmd_byte)(uint8_t status);

/**
 * Writes an argument to the port 0x60
 */ 
int(kbc_write_cmd_args)(uint8_t arg);

/**
 * Writes a command on the port 0x64
 */ 
int(kbc_write_cmd_command)(uint8_t byte);

/**
 * Reads a return value from 0x60 port
 */ 
int(kbc_read_cmd_return)(uint8_t *return_value);

/**
 * Reads status from ST Reg(0x60) while checking if there is any parity or timeout error
 */ 
int(kbc_read_status)(uint8_t *status);

/**
 * Writes a command to the mouse
 */ 
int(kbc_write_to_mouse)(uint8_t byte);

/**
 * Receives the acknowledgement from the mouse
 */ 
int(kbc_read_mouse_ack)();

int(kbc_read_mouse_data_by_poll)(uint8_t *return_value);

/**
 * @brief Disables mouse's irq line
 * 
 */
int(mouse_irq_disable)();

/**
 * @brief Enables mouse's irq line
 * 
 */
int(mouse_irq_enable)();

int (mouse_and_timer_handle_interruption)(uint8_t bit_no, uint8_t timer_bit_no, uint8_t idle_time);

/**
 * Reads mouse interrupts and stops when you a draw an inverted V
 * @param bit_no number for the irq_set mask
 * @param x_len min. length of each line
 * @param tolerance allowed displacement
 */ 
int(mouse_gesture_handle_interruption)(uint8_t bit_no, uint8_t x_len, uint8_t tolerance);

/**
 * Checks if the mouse movement is valid in order to create the inverted V
 */ 
void(validate_mouse_movement)(struct packet* packet, uint8_t x_len, uint8_t tolerance);

enum state_machine{
    INITIAL,
    LEFT_DRAW,
    VERTIX,
    RIGHT_DRAW,
    DONE
};

#endif

