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

#endif

