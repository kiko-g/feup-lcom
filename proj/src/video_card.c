#include <lcom/lcf.h>
#include <math.h>
#include <machine/int86.h>
#include "macros.h"
#include "video_card.h"

static unsigned vram_base;          // VRAM’s physical addresss
static unsigned vram_size;          // VRAM's size
static unsigned hres;               // Horiziontal pixel resolution
static unsigned vres;               // Horiziontal pixel resolution
static unsigned bitsPerPixel;       // Amount of VRAM bits per pixel
static unsigned bytesPerPixel;      // Amount of VRAM bits per pixel
static unsigned colorMode;          // Color Mode

static uint8_t redMaskSize;
static uint8_t greenMaskSize;
static uint8_t blueMaskSize;
static uint8_t redFieldPosition;
static uint8_t greenFieldPosition;
static uint8_t blueFieldPosition;

static void *video_mem;             // Frame-buffer VM address
static void *buffer;
struct minix_mem_range mr;          // physical memory range

unsigned get_hres() { return hres; }
unsigned get_vres() { return vres; }
unsigned get_bitsperpixel() { return bitsPerPixel; }
unsigned get_bytesperpixel() { return bytesPerPixel; }
void* get_buffer() { return buffer; }
void destroy_buffer() { free(buffer); }

int set_graphics_mode(uint16_t mode)
{
  mr.mr_base = 0;	
  mr.mr_limit = 1 << 20;
  
  int r;
  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  // clears the struct to avoid unpredictability
  reg86_t r86;
  memset(&r86, 0, sizeof(r86));

  r86.ah = VBE_FUNCTION;
  r86.al = SET_VBE_MODE;
  r86.bx = mode | LINEAR_FRAME_BUFFER;
  r86.intno = BIOS_SERVICE;

  if(sys_int86(&r86) != OK) {
    printf("Error in sys_int86()\n");
    return 1;
  }

  if(r86.ah != VBE_OK) {
    printf("VBE return value is different from expected\n");
    return 1;
  }

  return 0;
}


int map_memory()
{
  /* Allow memory mapping */
  mr.mr_base = (phys_bytes) vram_base;  //set lowest memory address
  mr.mr_limit = mr.mr_base + vram_size; //set highest memory address

  int r;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED)
   panic("couldn't map video memory");

  return 0;
}


int sync_vbe_mode_info(uint16_t mode, vbe_mode_info_t *info)
{
  reg86_t r86;
  memset(&r86, 0, sizeof(r86));

  // allocate memory 
  mmap_t mem_map;
  if(lm_alloc(sizeof(vbe_mode_info_t), &mem_map) == NULL)
  {
    printf("lm_alloc() failed to allocate mem for vbe_mode_info_t\n");
    return 1;
  }

  r86.ah = VBE_FUNCTION;
  r86.al = GET_VBE_MODE;
  r86.cx = mode;
  r86.es = PB2BASE(mem_map.phys);
  r86.di = PB2OFF(mem_map.phys);
  r86.intno = BIOS_SERVICE;

  if(sys_int86(&r86) != OK) {
    printf("Error in sys_int86()\n");
    return 1;
  }

  if(r86.ah != VBE_OK) {
    switch(r86.ah) { 
      case 1:
        printf("Function call failed\n");
        break;
      case 2:
        printf("Function is not supported in current HW configuration\n");
        break;
      case 3: 
        printf("Function is invalid in current video mode\n");
        break;
      }
      lm_free(&mem_map);
      return 1;
  }
  
  *info = *(vbe_mode_info_t *)mem_map.virt;

  if(!lm_free(&mem_map)){
    printf("lm_free() failed to free memory\n");
    return 1;
  }

  return 0;
}


int sync_vram_info(uint16_t mode)
{
  vbe_mode_info_t info;
  if (sync_vbe_mode_info(mode, &info)) {
    printf("Error while getting info about vbe mode.\n");
    return 1;
  } 

  // Need to remove some of those, or find a more clever way
  hres = info.XResolution;
  vres = info.YResolution;
  bitsPerPixel = info.BitsPerPixel;
  bytesPerPixel = ceil(bitsPerPixel / 8.0);
  vram_base = info.PhysBasePtr;
  vram_size = hres * vres * bytesPerPixel;
  colorMode = info.DirectColorModeInfo;
  redMaskSize = info.RedMaskSize;
  greenMaskSize = info.GreenMaskSize;
  blueMaskSize = info.BlueMaskSize;
  redFieldPosition = info.RedFieldPosition;
  greenFieldPosition = info.GreenFieldPosition;
  blueFieldPosition = info.BlueFieldPosition;

  return 0;
}


void* (vg_init)(uint16_t mode)
{
    if(sync_vram_info(mode)) return NULL;
    if(map_memory()) return NULL;
    if(set_graphics_mode(mode)) return NULL; 

    buffer = (char*) malloc (vram_size);

    return video_mem;
}


void switch_buffers() {
  memcpy(video_mem, buffer, vram_size);
}


int draw_pixel(uint16_t x, uint16_t y, uint32_t color)
{
    // makes sure not to draw outside of memory
    if (x >= hres || y >= vres || x < 0 || y < 0)
      return 0;
    
    // doesn't draw if color is an alpha channel
    else if (color == CHROMA_KEY_GREEN_888)
      return 0;

    // places point on the correct line and column
    uint8_t *point = buffer;
    point += bytesPerPixel * (hres*y + x);

    // writes the color value in the given pixel
    for (unsigned j = 0; j < bytesPerPixel; j++)
    {
       *point = color;
        color >>= 8;
        point++;
    }

    return 0;
}


int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{
    for(unsigned i=x; i < (len+x); i++)
    {
      if(i >= hres) break;
      draw_pixel(i, y, color);
    }

    return 0;
}


int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    for(unsigned i=y; i < (height+y); i++)
    {
      if(i >= vres) break;
      vg_draw_hline(x, i, width, color);
    }

    return 0;
}


int(vg_draw_rectangle_matrix)(uint16_t mode, uint16_t no_rectangles, uint32_t first, uint8_t step) {
  
  // Calculating the rectangle's width
  unsigned width = hres/no_rectangles;

  // Calculating the rectangle's height
  unsigned height = vres/no_rectangles;

  // Declares color for later use, because its calculation depends on the mode
  uint32_t color; 

  for(size_t row = 0; row < no_rectangles; row++) {
    for(size_t col = 0; col < no_rectangles; col++) {
      
      if(mode == INDEXED_COLOR) {
        color = (first + (row * no_rectangles + col) * step) % (1 << bitsPerPixel);
      }
      else {
        uint8_t red_first = first >> redFieldPosition % BIT(redMaskSize);
        uint8_t green_first = first >> greenFieldPosition % BIT(greenMaskSize);
        uint8_t blue_first = first >> blueFieldPosition % BIT(blueMaskSize);

        uint8_t red = (red_first + col*step) % BIT(redMaskSize);
        uint8_t green = (green_first + row*step) % BIT(greenMaskSize);
        uint8_t blue = (blue_first + (col+row)*step) % BIT(blueMaskSize);

        color = (red << redFieldPosition) | 
                (green << greenFieldPosition) | 
                (blue << blueFieldPosition);
      }
      
      vg_draw_rectangle(col*width, row*height, width, height, color);
    }
  }

  return 0;
}


int draw_pixmap(uint8_t *map, xpm_image_t img, uint16_t x, uint16_t y)
{
  for(size_t i=0; i < img.height; i++)
  {
      //over vertical screen limit
      if ((y+i) >= vres) break;
      for(size_t j=0; j < img.width; j++)
      {
          //over horizontal screen limit
          if ((x+j) >= hres) break; 
          draw_pixel((x+j), (y+i), *(map + j + i*img.width));
      }
  }

  return 0;
}

uint32_t convert_far_ptr(phys_bytes p, void *base)
{
  return (((p >> 16) << 4) + PB2OFF(p) + (uint32_t) base);
}

int get_vbe_contr_info(vg_vbe_contr_info_t* c_info)
{
  vbe_info_block_t vbe_block_info;



  // allocate memory 
  mmap_t mem_map;
  if(lm_alloc(sizeof(vbe_mode_info_t), &mem_map) == NULL)
  {
    printf("lm_alloc() failed to allocate mem for vbe_mode_info_t\n");
    return 1;
  }

  // calculates base virtual pointer
  void* membase_ptr = (void*) ((phys_bytes)(mem_map.virt) - mem_map.phys);

  memcpy((vbe_info_block_t*)mem_map.virt, "VBE2", 4);

  reg86_t r86;
  memset(&r86, 0, sizeof(r86));

  r86.ah = VBE_FUNCTION;
  r86.al = GET_VBE_CTRL_INFO;
  r86.es = PB2BASE(mem_map.phys);
  r86.di = PB2OFF(mem_map.phys);
  r86.intno = BIOS_SERVICE;

  if(sys_int86(&r86) != OK) {
    printf("Error in sys_int86()\n");
    return 1;
  }

  if(r86.ah != VBE_OK) {
    switch(r86.ah) { 
      case 1:
        printf("Function call failed\n");
        break;
      case 2:
        printf("Function is not supported in current HW configuration\n");
        break;
      case 3: 
        printf("Function is invalid in current video mode\n");
        break;
      }
      lm_free(&mem_map);
      return 1;
  }

  vbe_block_info = *(vbe_info_block_t *) mem_map.virt;

  if(!lm_free(&mem_map)){
    printf("lm_free() failed to free memory\n");
    return 1;
  }

  memcpy(c_info->VBESignature, vbe_block_info.VBESignature, 4);

  c_info->VBEVersion[0] = vbe_block_info.VBEVersion[0];

  c_info->VBEVersion[1] = vbe_block_info.VBEVersion[1];

  c_info->TotalMemory = vbe_block_info.TotalMemory * 64; //number of 64kb blocks

  c_info->VideoModeList = (uint16_t*) convert_far_ptr(vbe_block_info.videoModePTR, membase_ptr);

  c_info->OEMString = (char*) convert_far_ptr(vbe_block_info.OEMStringPTR, membase_ptr);

  c_info->OEMVendorNamePtr = (char *) convert_far_ptr(vbe_block_info.OEMVendorNamePTR, membase_ptr);

  c_info->OEMProductNamePtr = (char*)convert_far_ptr(vbe_block_info.OEMProductNamePTR, membase_ptr);

  c_info->OEMProductRevPtr = (char *) convert_far_ptr(vbe_block_info.OEMProductRevPTR, membase_ptr);

  lm_free(&mem_map);
  return 0;
}
