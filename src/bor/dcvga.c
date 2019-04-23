/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"

#include "dcvga.h"
#include "g2.h"

#include "filecache.h"

/////////////////////////////////////////////////////////////////////////////

#define VIDREG_ARRAY ((volatile unsigned*)(0xA05F8000))

#define VIDREG_ID             (VIDREG_ARRAY[0x000/4])
#define VIDREG_REVISION       (VIDREG_ARRAY[0x004/4])
#define VIDREG_RESET          (VIDREG_ARRAY[0x008/4])
  #define VIDREG_RESET_ta     (0x00000001)
  #define VIDREG_RESET_render (0x00000002)
  #define VIDREG_RESET_vram   (0x00000004)
  #define VIDREG_RESET_all    (0x00000007)
#define VIDREG_unknown0C      (VIDREG_ARRAY[0x00C/4])
#define VIDREG_unknown10      (VIDREG_ARRAY[0x010/4])
#define VIDREG_STARTRENDER    (VIDREG_ARRAY[0x014/4])
#define VIDREG_unknown18      (VIDREG_ARRAY[0x018/4])
#define VIDREG_unknown1C      (VIDREG_ARRAY[0x01C/4])
#define VIDREG_OB_ADDR        (VIDREG_ARRAY[0x020/4])
#define VIDREG_unknown24      (VIDREG_ARRAY[0x024/4])
#define VIDREG_unknown28      (VIDREG_ARRAY[0x028/4])
#define VIDREG_TILEBUF_ADDR   (VIDREG_ARRAY[0x02C/4])
#define VIDREG_SPANSORT_CFG   (VIDREG_ARRAY[0x030/4])
#define VIDREG_unknown34      (VIDREG_ARRAY[0x034/4])
#define VIDREG_unknown38      (VIDREG_ARRAY[0x038/4])
#define VIDREG_unknown3C      (VIDREG_ARRAY[0x03C/4])
#define VIDREG_BORDER_COL     (VIDREG_ARRAY[0x040/4])
#define VIDREG_FB_CFG1        (VIDREG_ARRAY[0x044/4])
  #define VIDREG_FB_CFG1_enable     (0x00000001)
  #define VIDREG_FB_CFG1_linedouble (0x00000002)
  #define VIDREG_FB_CFG1_pixelmode  (0x0000000C)
  #define VIDREG_FB_CFG1_clock      (0x00800000)
#define VIDREG_FB_CFG2        (VIDREG_ARRAY[0x048/4])
#define VIDREG_RENDER_MODULO  (VIDREG_ARRAY[0x04C/4])
#define VIDREG_DISPLAY_ADDR1  (VIDREG_ARRAY[0x050/4]) /* start for odd fields */
#define VIDREG_DISPLAY_ADDR2  (VIDREG_ARRAY[0x054/4]) /* start for even fields */
#define VIDREG_unknown58      (VIDREG_ARRAY[0x058/4])
#define VIDREG_DISPLAY_SIZE   (VIDREG_ARRAY[0x05C/4])
#define VIDREG_RENDER_ADDR1   (VIDREG_ARRAY[0x060/4])
#define VIDREG_RENDER_ADDR2   (VIDREG_ARRAY[0x064/4])
#define VIDREG_PCLIP_X        (VIDREG_ARRAY[0x068/4])
#define VIDREG_PCLIP_Y        (VIDREG_ARRAY[0x06C/4])
#define VIDREG_unknown70      (VIDREG_ARRAY[0x070/4])
#define VIDREG_SHADOW         (VIDREG_ARRAY[0x074/4])
#define VIDREG_OBJECT_CLIP    (*(vfloat32*)(&(VIDREG_ARRAY[0x078/4])))
#define VIDREG_OB_CFG         (VIDREG_ARRAY[0x07C/4])
#define VIDREG_unknown80      (VIDREG_ARRAY[0x080/4])
#define VIDREG_TSP_CLIP       (*(vfloat32*)(&(VIDREG_ARRAY[0x084/4])))
#define VIDREG_BGPLANE_Z      (*(vfloat32*)(&(VIDREG_ARRAY[0x088/4])))
#define VIDREG_BGPLANE_CFG    (VIDREG_ARRAY[0x08C/4])
#define VIDREG_unknown90      (VIDREG_ARRAY[0x090/4])
#define VIDREG_unknown94      (VIDREG_ARRAY[0x094/4])
#define VIDREG_unknown98      (VIDREG_ARRAY[0x098/4])
#define VIDREG_unknown9C      (VIDREG_ARRAY[0x09C/4])
#define VIDREG_VRAM_CFG1      (VIDREG_ARRAY[0x0A0/4])
#define VIDREG_VRAM_CFG2      (VIDREG_ARRAY[0x0A4/4])
#define VIDREG_VRAM_CFG3      (VIDREG_ARRAY[0x0A8/4])
#define VIDREG_unknownAC      (VIDREG_ARRAY[0x0AC/4])
#define VIDREG_FOG_TABLE_COL  (VIDREG_ARRAY[0x0B0/4])
#define VIDREG_FOG_VERTEX_COL (VIDREG_ARRAY[0x0B4/4])
#define VIDREG_FOG_DENSITY    (VIDREG_ARRAY[0x0B8/4])
#define VIDREG_CLAMP_MAX      (VIDREG_ARRAY[0x0BC/4])
#define VIDREG_CLAMP_MIN      (VIDREG_ARRAY[0x0C0/4])
#define VIDREG_GUN_POS        (VIDREG_ARRAY[0x0C4/4])
#define VIDREG_HPOS_IRQ       (VIDREG_ARRAY[0x0C8/4])
#define VIDREG_VPOS_IRQ       (VIDREG_ARRAY[0x0CC/4])
#define VIDREG_SYNC_CFG       (VIDREG_ARRAY[0x0D0/4])
  #define VIDREG_SYNC_CFG_enable    (0x00000100)
  #define VIDREG_SYNC_CFG_flag_pal  (0x00000080)
  #define VIDREG_SYNC_CFG_flag_ntsc (0x00000040)
  #define VIDREG_SYNC_CFG_interlace (0x00000010)
  #define VIDREG_SYNC_CFG_hp        (0x00000004)
  #define VIDREG_SYNC_CFG_vp        (0x00000002)
#define VIDREG_HBORDER        (VIDREG_ARRAY[0x0D4/4])
#define VIDREG_SYNC_LOAD      (VIDREG_ARRAY[0x0D8/4])
#define VIDREG_VBORDER        (VIDREG_ARRAY[0x0DC/4])
#define VIDREG_SYNC_WIDTH     (VIDREG_ARRAY[0x0E0/4])
#define VIDREG_TSP_CFG        (VIDREG_ARRAY[0x0E4/4])
#define VIDREG_VIDEO_CFG      (VIDREG_ARRAY[0x0E8/4])
  #define VIDREG_VIDEO_CFG_blank (0x00000008)
  #define VIDREG_VIDEO_CFG_lores (0x00000100)
#define VIDREG_HPOS           (VIDREG_ARRAY[0x0EC/4])
#define VIDREG_VPOS           (VIDREG_ARRAY[0x0F0/4])
#define VIDREG_SCALER_CFG     (VIDREG_ARRAY[0x0F4/4])
#define VIDREG_unknownF8      (VIDREG_ARRAY[0x0F8/4])
#define VIDREG_unknownFC      (VIDREG_ARRAY[0x0FC/4])
#define VIDREG_unknown100     (VIDREG_ARRAY[0x100/4])
#define VIDREG_unknown104     (VIDREG_ARRAY[0x104/4])
#define VIDREG_PALETTE_CFG    (VIDREG_ARRAY[0x108/4])
#define VIDREG_SYNC_STAT      (VIDREG_ARRAY[0x10C/4])
  #define VIDREG_SYNC_STAT_vpos   (0x000003FF)
  #define VIDREG_SYNC_STAT_field  (0x00000400)
  #define VIDREG_SYNC_STAT_hblank (0x00001000)
  #define VIDREG_SYNC_STAT_vblank (0x00002000)
#define VIDREG_unknown110     (VIDREG_ARRAY[0x110/4])
#define VIDREG_unknown114     (VIDREG_ARRAY[0x114/4])
#define VIDREG_TA_LUMINANCE   (VIDREG_ARRAY[0x118/4])
#define VIDREG_unknown11C     (VIDREG_ARRAY[0x11C/4])
#define VIDREG_unknown120     (VIDREG_ARRAY[0x120/4])
#define VIDREG_TA_OPB_START   (VIDREG_ARRAY[0x124/4])
#define VIDREG_TA_OB_START    (VIDREG_ARRAY[0x128/4])
#define VIDREG_TA_OPB_END     (VIDREG_ARRAY[0x12C/4])
#define VIDREG_TA_OB_END      (VIDREG_ARRAY[0x130/4])
#define VIDREG_TA_OPB_POS     (VIDREG_ARRAY[0x134/4])
#define VIDREG_TA_OB_POS      (VIDREG_ARRAY[0x138/4])
#define VIDREG_TILEBUF_SIZE   (VIDREG_ARRAY[0x13C/4])
#define VIDREG_TA_OPB_CFG     (VIDREG_ARRAY[0x140/4])
#define VIDREG_TA_INIT        (VIDREG_ARRAY[0x144/4])
#define VIDREG_YUV_ADDR       (VIDREG_ARRAY[0x148/4])
#define VIDREG_YUV_CFG1       (VIDREG_ARRAY[0x14C/4])
#define VIDREG_YUV_CFG2       (VIDREG_ARRAY[0x150/4])
#define VIDREG_unknown154     (VIDREG_ARRAY[0x154/4])
#define VIDREG_unknown158     (VIDREG_ARRAY[0x158/4])
#define VIDREG_unknown15C     (VIDREG_ARRAY[0x15C/4])
#define VIDREG_unknown160     (VIDREG_ARRAY[0x160/4])
#define VIDREG_TA_OPL_INIT    (VIDREG_ARRAY[0x164/4])
#define VIDREG_unknown168     (VIDREG_ARRAY[0x168/4])
#define VIDREG_unknown16C     (VIDREG_ARRAY[0x16C/4])
#define VIDREG_unknown170     (VIDREG_ARRAY[0x170/4])
#define VIDREG_unknown174     (VIDREG_ARRAY[0x174/4])
#define VIDREG_unknown178     (VIDREG_ARRAY[0x178/4])
#define VIDREG_unknown17C     (VIDREG_ARRAY[0x17C/4])

#define VIDREG_FOG_TABLE      ((volatile double*)(&(VIDREG_ARRAY[0x200/4])))
#define VIDREG_OPL_TABLE      (&(VIDREG_ARRAY[0x600/4]))
#define VIDREG_PALETTE_TABLE  (&(VIDREG_ARRAY[0x1000/4]))

#define VIDREG_DMA_DST         (*((volatile unsigned*)(0xA05F6800)))
#define VIDREG_DMA_LEN         (*((volatile unsigned*)(0xA05F6804)))
#define VIDREG_DMA_STATE       (*((volatile unsigned*)(0xA05F6808)))
#define VIDREG_DMA_LMMODE0     (*((volatile unsigned*)(0xA05F6884)))
#define VIDREG_DMA_LMMODE1     (*((volatile unsigned*)(0xA05F6888)))

/////////////////////////////////////////////////////////////////////////////

#define VIDEO_CABLE_VGA       (0)
#define VIDEO_CABLE_RGB       (2)
#define VIDEO_CABLE_COMPOSITE (3)

#define VIDEO_PIXELMODE_ARGB1555 (0)
#define VIDEO_PIXELMODE_RGB565   (1)
#define VIDEO_PIXELMODE_RGB888   (2)
#define VIDEO_PIXELMODE_ARGB8888 (3)

/////////////////////////////////////////////////////////////////////////////

unsigned dcvga_framecount = 0;

/////////////////////////////////////////////////////////////////////////////

static int video_check_cable(void) {
  #define CCN_PCTRA (*((volatile unsigned int  *)(0xFF80002C)))
  #define CCN_PDTRA (*((volatile unsigned short*)(0xFF800030)))
  CCN_PCTRA = (CCN_PCTRA & 0xFFF0FFFF) | 0x000A0000;
  return (CCN_PDTRA >> 8) & 3;
}

static void video_set_cable(int cabletype) {
  unsigned c = g2_readword((void*)0xA0702C00);
  if(cabletype == VIDEO_CABLE_COMPOSITE) {
    c |=  0x00000300;
  } else {
    c &= ~0x00000300;
  }
  g2_writeword((void*)0xA0702C00, c);
}

/////////////////////////////////////////////////////////////////////////////
//
// Force blank/unblank
//
void dcvga_force_blank(void) {
  VIDREG_VIDEO_CFG |=  VIDREG_VIDEO_CFG_blank;
  VIDREG_FB_CFG1   &= ~VIDREG_FB_CFG1_enable;
}

void dcvga_force_enable(void) {
  VIDREG_VIDEO_CFG &= ~VIDREG_VIDEO_CFG_blank;
  VIDREG_FB_CFG1   |=  VIDREG_FB_CFG1_enable;
}

/////////////////////////////////////////////////////////////////////////////
#ifndef PAL

/////////////////////////////////////////////////////////////////////////////
//
// set video mode NTSC (60Hz) 320x240x32bpp 
//
/////////////////////////////////////////////////////////////////////////////

static void video_setmode(void) {
  int cabletype = video_check_cable();
  int vga = (cabletype == VIDEO_CABLE_VGA);

  { unsigned data = (VIDEO_PIXELMODE_RGB888 << 2);
    if(vga) {
      data |= VIDREG_FB_CFG1_clock;
      data |= VIDREG_FB_CFG1_linedouble;
    }
    VIDREG_FB_CFG1 = data;
  }

  VIDREG_DISPLAY_SIZE =
    ((240 - 1) <<  0) |
    ((240 - 1) << 10) |
    (1 << 20);

  VIDREG_SYNC_CFG = VIDREG_SYNC_CFG_enable;

  VIDREG_VPOS_IRQ = ((21 << 16) | 260) << (vga?1:0);

  VIDREG_HBORDER   = (141 << 16) | 843;
  VIDREG_VBORDER   = ((24 << 16) | 263) << (vga?1:0);

  VIDREG_SYNC_LOAD = (262 << (vga?17:16)) | 857;

  VIDREG_HPOS = 164;
  VIDREG_VPOS = ((24<<16)|24) << (vga?1:0);

  VIDREG_VIDEO_CFG |=  VIDREG_VIDEO_CFG_lores;

  video_set_cable(cabletype);
}

#else
/////////////////////////////////////////////////////////////////////////////
//
// set video mode PAL(50Hz) 320x240x32bpp 
//
/////////////////////////////////////////////////////////////////////////////


static void video_setmode(void) {
	int cabletype = video_check_cable();
	int vga = (cabletype == VIDEO_CABLE_VGA);
	
	{ unsigned data = (VIDEO_PIXELMODE_RGB888 << 2);
    if(vga) {
		data |= VIDREG_FB_CFG1_clock;
		data |= VIDREG_FB_CFG1_linedouble;
    }
    VIDREG_FB_CFG1 = data;
	}
	
	VIDREG_DISPLAY_SIZE =
		((240 - 1) <<  0) |
		((240 - 1) << 10) |
		(1 << 20);
	
	VIDREG_SYNC_CFG = VIDREG_SYNC_CFG_enable;
	
	VIDREG_VPOS_IRQ = ((21 << 16) | 260) << (vga?1:0);
	
	VIDREG_HBORDER   = (141 << 16) | 843;
	VIDREG_VBORDER   = ((44 << 16) | 620) << (vga?1:0);
	
	VIDREG_SYNC_LOAD = (312 << (vga?17:16)) | 863;
	
	VIDREG_HPOS = 174;
	VIDREG_VPOS = ((46<<16)|45) << (vga?1:0);
	
	VIDREG_VIDEO_CFG |=  VIDREG_VIDEO_CFG_lores;
	
	video_set_cable(cabletype);
}
#endif // PAL or NTSC


/////////////////////////////////////////////////////////////////////////////

static unsigned dcvgapal[256];

/////////////////////////////////////////////////////////////////////////////

void dcvga_init(void) {
  VIDREG_DMA_STATE = 0;
  VIDREG_RESET     = 0x00000007;
  VIDREG_RESET     = 0x00000000;
  VIDREG_VRAM_CFG1 = 0x00000020; // magic
  VIDREG_VRAM_CFG3 = 0x15D1C951; // magic
  VIDREG_BORDER_COL = 0;
  video_setmode();
  dcvga_force_blank();
  VIDREG_DISPLAY_ADDR1 = 0;
  VIDREG_DISPLAY_ADDR2 = 0;
  memset(dcvgapal, 0, sizeof(dcvgapal));
//  dcvga_force_enable();
}

/////////////////////////////////////////////////////////////////////////////

void dcvga_setpalette(char *pal) {
  int i;
  for(i = 0; i < 256; i++) {
    unsigned r = ((unsigned)(*pal++)) & 0xFF;
    unsigned g = ((unsigned)(*pal++)) & 0xFF;
    unsigned b = ((unsigned)(*pal++)) & 0xFF;
    dcvgapal[i^0x80] = b | (g << 8) | (r << 16);
  }
}

/////////////////////////////////////////////////////////////////////////////

#define QACR0 (*((vuint32*)(0xFF000038)))
#define QACR1 (*((vuint32*)(0xFF00003C)))
#define SQDATA  ((vuint32*)(0xE0000000))
#define sq_set_area(dest) ({uint32 area=(((uint32)(dest))>>24)&0x1C;QACR0=area;QACR1=area;0;})
#define sq_get_queue_address(dest) ((uint32*)((((uint32)(dest))&0x03FFFFE0)|0xE0000000))
#define sq_commit_write(dest) ({asm volatile("pref @%r0"::"r"(dest));0;})

/////////////////////////////////////////////////////////////////////////////
//
// Copy 320x240x8bpp framebuffer to the screen, performing color palette
// lookups in software along the way.
//
// This eats about 17% of the frame, but since paletted textures have to be
// twiddled, I know of no better way to do it
//
static void dcvga_copy(unsigned addr, char *img) {
  register unsigned a,b,c,d;
  register unsigned xy;
  register unsigned *dest = sq_get_queue_address(addr);
  register unsigned *pal = dcvgapal+0x80;
  sq_set_area(addr);
  for(xy = 0; xy < (320*240); xy+=32) {
#define GETFOUR { \
    a = pal[(signed int)((signed char)(*img++))]; \
    b = pal[(signed int)((signed char)(*img++))]; \
    c = pal[(signed int)((signed char)(*img++))]; \
    d = pal[(signed int)((signed char)(*img++))]; }
#define PUT1(x) dest[(x)] = a|(b<<24);
#define PUT2(x) dest[(x)] = (b>>8)|(c<<16);
#define PUT3(x) dest[(x)] = (c>>16)|(d<<8);
#define COMMITSQ {sq_commit_write(dest);dest+=8;}
    GETFOUR;PUT1(0);PUT2(1);PUT3(2);
    GETFOUR;PUT1(3);PUT2(4);PUT3(5);
    GETFOUR;PUT1(6);PUT2(7); COMMITSQ; PUT3(0);
    GETFOUR;PUT1(1);PUT2(2);PUT3(3);
    GETFOUR;PUT1(4);PUT2(5);PUT3(6);
    GETFOUR;PUT1(7); COMMITSQ; PUT2(0);PUT3(1);
    GETFOUR;PUT1(2);PUT2(3);PUT3(4);
    GETFOUR;PUT1(5);PUT2(6);PUT3(7);
    COMMITSQ;
  }
}

/////////////////////////////////////////////////////////////////////////////

void dcvga_draw(char *img) {
  unsigned addr = (dcvga_framecount & 1) ? 0xA5400000 : 0xA5000000;
  dcvga_copy(addr, img);
//  while((VIDREG_SYNC_STAT & VIDREG_SYNC_STAT_vblank) == 0);
//  while((VIDREG_SYNC_STAT & VIDREG_SYNC_STAT_vblank) != 0);

  filecache_process();

  // we can relax this constraint since it'd be impossible to have two
  // frames coinciding within vblank (dcvga_copy uses 17% of the frame)
  while((VIDREG_SYNC_STAT & VIDREG_SYNC_STAT_vblank) != 0);
  addr &= 0x7FFFFF;
  VIDREG_DISPLAY_ADDR1 = addr;
  VIDREG_DISPLAY_ADDR2 = addr;
  dcvga_framecount++;
}

/////////////////////////////////////////////////////////////////////////////
