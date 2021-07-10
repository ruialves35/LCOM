#ifndef _LCOM_I8042H
#define _LCOM_I8042H

#include <lcom/lcf.h>

/* @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 Keyboard.
 */

#define TIMER0_IRQ 0
#define KBC_IRQ 1     /*/< @brief Keyboard Controller IRQ line-1 / */
#define KBC_MOUSE_IRQ 12
#define BREAK_CD 0x81 /*<@brief breakCode by "ESC" key/ */
#define DELAY_US 20000     //polling period

/* Keyboard port addresses */
#define KBC_OUT_BUF 0x60  /**<@brief Address of Output Buffer */
#define KBC_ARGS_REG 0x60 /*<@brief Address where we are going to write KBC's arguments */
#define KBC_ST_REG 0x64   /**<@brief Address of status register*/
#define KBC_CMD_REG 0x64  /*<@brief Address where we are going to write KB's command */

// Status Register Bits
#define KBC_OBF BIT(0)     /**<@brief To check if OBF Bit is 1, if it is we can't write!*/
#define KBC_IBF_BIT BIT(1) /**<@brief To check if IBF Bit is 1, if it is we can't write!*/
#define KBC_SYS_BIT BIT(2) /**<@brief System Flag */
#define KBC_A2_BIT BIT(3)  /**<@brief A2 input line */
#define KBC_INH_BIT BIT(4) /**<@brief Inhibit flag: 0 if keyboard is inhibited */
#define KBC_AUX_BIT BIT(5) /**<@brief Mouse Data */
#define KBC_TIMEOUT BIT(6) /**<@brief Check Time Out Error (1) */
#define KBC_PARITY BIT(7)  /**<@brief Check Parity Error */
#define KBC_PARITY_AND_TIMEOUT BIT(7) | BIT(6)

// Keyboard specific codes
#define KBC_ESC_BREAKCODE 0x81
#define KBC_2BYTES_CODE 0xe0

//KBC Commands
#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60
#define KBC_CHECK_CMD 0xAA
#define KBC_CHECK_KB 0xAB
#define KBC_DIS_KBD 0xAD
#define KBC_EN_KBD 0xAE
#define KBC_WRITE_MOUSE 0xD4


//mouse packet bits
#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_BYTE0_MASK BIT(3)
#define MOUSE_MSB_X_DELTA BIT(4)
#define MOUSE_MSB_Y_DELTA BIT(5)
#define MOUSE_X_OVFL BIT(6)
#define MOUSE_Y_OVFL BIT(7)

//PS/2 Mouse Commands
#define MC_RESET       0xFF    /*<@brief Mouse Reset*/
#define MC_RESEND      0xFE    /*<@brief For serial communications errors*/
#define MC_DEFAULTS    0xF6    /**<@brief Set default values*/
#define MC_DIS_DATA_REP    0xF5    /*<@brief In stream mode, Disable (Data Reporting) */
#define MC_EN_DATA_REP       0xF4    /*<@brief In stream mode Enable (Data Reporting) */
#define MC_SET_SR      0xF3    /**<@brief Set Sample Rate*/
#define MC_SET_REMOTE_MODE      0xF0    /*<@brief Set Remote mode */
#define MC_READ_DATA      0xEB    /*<@brief Read Data*/
#define MC_SET_STREAM_MODE      0xEA    /**<@brief Set Stream Mode*/
#define MC_ST_REQ      0xE9    /*<@brief Status Request Get mouse configuration (3 bytes)*/
#define MC_SET_RES     0xE8    /*<@brief Set Resolution*/
#define MC_SET_RES21   0xE7    /**<@brief Acceleration mode*/
#define MC_SET_RES11   0xE6    /*<@brief Linear mode*/

//Mouse response values

#define MOUSE_ACK 0xFA
#define MOUSE_NACK 0xFE
#define MOUSE_FC 0xFC

#endif
