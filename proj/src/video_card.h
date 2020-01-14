#ifndef _LCOM_VIDEO_CARD_H_
#define _LCOM_VIDEO_CARD_H_

/** @defgroup videocard
 * @{
 *
 * Video card - Types, macros and functions for programming with the video card 
 */


#pragma pack(1)
typedef struct {
  uint8_t VBESignature[4];      /* VBE signature */
  BCD VBEVersion[2];            /* VBE Version */
  uint32_t OEMStringPTR;        /* Pointer to OEM String */
  uint8_t Capabilities[4];      /* Capabilities of graphics controller */
  uint32_t videoModePTR;        /* Pointer to VideoModeList */
  uint32_t TotalMemory;         /* Number of 64kb memory blocks: Added for VBE 2.0 */
  
  uint16_t OEMSoftwareRev;      /* VBE implemntation Software revision */
  uint32_t OEMVendorNamePTR;    /* Pointer to Vendor Name String */
  uint32_t OEMProductNamePTR;   /* Pointer to Product Name String */
  uint32_t OEMProductRevPTR;    /* Pointer to Product Revision String */
  uint8_t Reserved[222];        /* Reserved for VBE implementation scratch area */
  uint8_t OEMData[256];         /* Data Area for OEM Strings */
} vbe_info_block_t;
//PTR fields are far pointers

#pragma options align = reset

//functions below are used to get attributes from outside
//the file they are defined in (video_card.c)
unsigned get_hres();
unsigned get_vres();
unsigned get_bitsperpixel();
unsigned get_bytesperpixel();
void* get_buffer();
void destroy_buffer();

/** 
 * @brief Change the graphics mode to 'mode'
 * @param mode the intended 16bit graphics mode to set
 * @return 0 upon success, non-zero otherwise
 */
int set_graphics_mode(uint16_t mode);

/** 
 * @brief maps vram
 * @return 0 upon success
 */
int map_memory();

/** 
 * @brief extract vbe mode information
 * @param mode the intended 16bit graphics mode
 * @return 0 upon success, non-zero otherwise
 */
int sync_vram_info(uint16_t mode);

/** 
 * @brief our implementation of vg_init
 * @param mode the intended 16bit graphics mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void* (vg_init)(uint16_t mode);


/**
 * @brief Switches from the auxiliary buffer to the main buffer
 */
void switch_buffers();


/** 
 * @brief Our own implementation of vbe_get_mode_info
 * @brief Gets current graphics mode information 
 * @return 0 upon success, non-zero otherwise
 */
int sync_vbe_mode_info(uint16_t mode, vbe_mode_info_t *info);

/** 
 * @brief Parses vbe controller information
 * @param c_info controller info variable to be parsed
 * @return 0 upon success, non-zero otherwise
*/
int get_vbe_contr_info(vg_vbe_contr_info_t *c_info);

/** 
 * @brief Draws a pixel on x, y with color provided
 * @param x horizontal coordinate
 * @param y vertical coordinate
 * @param color hexadecimal color to draw on x,y
 * @return 0 upon success, non-zero otherwise
*/
int draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/** 
 * @brief Draws a horizontal line with length 'len' and color 'color' using draw_pixel, starting on (x,y) 
 * @param x horizontal coordinate
 * @param y vertical coordinate
 * @param len length of the line
 * @param color hexadecimal color of the line
 * @return 0 upon success
*/
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

/** 
 * @brief Draws a rectangle (using vg_draw_h_line) of color 'color', size (width * height) and starting on (x, y)
 * @param x horizontal coordinate
 * @param y vertical coordinate
 * @param width rectangle width
 * @param height rectangle height
 * @param color hexadecimal color of the rectangle
 * @return 0 upon success
*/
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

/** 
 * @brief Draws a rectangle (using vg_draw_h_line) of color 'color', size (width * height) and starting on (x, y)
 * @param mode intended graphics mode
 * @param no_rectangles number of rectangles
 * @param first first color
 * @param step color step (used to calculate next colors)
 * @return 0 upon success
*/
int (vg_draw_rectangle_matrix)(uint16_t mode, uint16_t no_rectangles, uint32_t first, uint8_t step);


/** 
 * @brief draws a xpm on the coords (x,y)
 * @param map pixmap obtained from a xpm using xpm_load()
 * @param img struct that carries info about image
 * @param x horizontal starting coordinate
 * @param y vertical starting coordinate
 * @return 0 upon succes
 */
int draw_pixmap(uint8_t *map, xpm_image_t img, uint16_t x, uint16_t y);



/**
 * @} 
 */


#endif /* _LCOM_VIDEO_CARD_H */
