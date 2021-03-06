#ifndef _LCOM_MACROS_H_
#define _LCOM_MACROS_H_

#include <lcom/lcf.h>
#include <minix/sysutil.h>

/* General macros */

#define LINEAR_FRAME_BUFFER   BIT(14)
#define MAX_LM_ALLOC_ATTEMPTS 5


/* VBE */

#define VBE_OK        0x00
#define BIOS_SERVICE  0x10

#define VBE_FUNCTION        0x4F
#define GET_VBE_CTRL_INFO   0x00
#define GET_VBE_MODE        0x01
#define SET_VBE_MODE        0x02


/* Color mode */
#define INDEXED_COLOR 0x105

#endif /* _LCOM_MACROS_H */
