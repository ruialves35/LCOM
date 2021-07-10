#ifndef _LCOM_VC_MACROS_H
#define _LCOM_VC_MACROS_H

/* BIOS Services */
#define VBE_BIOS_CALL     0x10

/* VBE Functions */
#define RETURN_VBE_CONTROLLER_INFO 0x00
#define RETURN_VBE_MODE_INFO 0x01
#define SET_VBE_MODE 0x02
#define RETURN_CURRENT_MODE_INFO 0x03
#define SET_DISPLAY_START 0x07
#define SET_DISPLAY_START_VERTICAL_RETRACE 0x80

/* VBE function returns in AH */
#define FUNC_SUCCESS 0x00
#define FUNC_CALL_FAILED 0x01 
#define FUNC_SUPPORTED 0x4F
#define FUNC_NOT_SUPPORTED 0x02
#define FUNC_INVALID 0x03
#define FUNC_RETURN_OK 0x004F

/* VBE function return in AL */
#define VBE_FUNC 0x4F

/* AH register in VBE mode */
#define VBE_MODE 0x4F

/* Graphics Mode */
#define R1024x768_INDEXED 0x105
#define R40x480_DIRECT 0x110
#define R800x600_DIRECT 0x115 
#define R1280x1024_DIRECT 0x11A 
#define R1152x864_DIRECT 0x14C

/* Mode Attributes*/
#define MODE_SUPPORTED_HARDWARE BIT(0)
#define TTY_FUNCS_SUPPORTED_BIOS BIT(2)
#define COLOR_MODE BIT(3)
#define GRAPHICS_MODE BIT(4)

/* Set VBE Mode */
#define SET_LINEAR_MODE BIT(14)

/* Memory Model */
#define INDEXED_COLOR_MODE 0x04
#define DIRECT_COLOR_MODE 0x06

/* Bytes per pixel      */
#define BPP_INDEXED 1
#define BPP_0x14C 4

/* VBE + functions */
#define VBE               0x4F00
#define COMPLETE_SET_VBE_MODE       0x4F02    /**< @brief Set VBE mode, VBE call, function 02 */

/* COLORS         */
#define TRANSPARENCY_COLOR_8_8_8_8 0xFF000000


#endif // _LCOM_VC_MACROS_H
