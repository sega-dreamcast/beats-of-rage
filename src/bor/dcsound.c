/////////////////////////////////////////////////////////////////////////////
//
// Dreamcast sound stuff
//
// And IRQ stuff, but only because the sound code is all that cares about
// interrupts
//
/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"

#include "dcsound.h"
#include "g2.h"
#include "timer.h"

/////////////////////////////////////////////////////////////////////////////

#define SOUNDDRV_ARMPTR32(n) ((vuint32*)((0x00000000)+(n)))
#define SOUNDDRV_SH4PTR32(n) ((vuint32*)((0xA0800000)+(n)))
#define STR_TO_UINT32(s) ( \
  ((((uint32)((s)[0])) & 0xFF) <<  0) |  \
  ((((uint32)((s)[1])) & 0xFF) <<  8) |  \
  ((((uint32)((s)[2])) & 0xFF) << 16) |  \
  ((((uint32)((s)[3])) & 0xFF) << 24)    \
)
#define AICA_REGS (0xA0700000)
#define AICA_MEM  (0xA0800000)
#define SNDREGADDR(x) ((void*)(AICA_REGS+(x)))
#define CHNREGADDR(chn,x) SNDREGADDR(0x80*(chn)+(x))

/////////////////////////////////////////////////////////////////////////////

static dcsound_fillstereosamples_t thefillfunction = NULL;

void dcsound_setfillfunction(dcsound_fillstereosamples_t fillfunction) {
  thefillfunction = fillfunction;
}

/////////////////////////////////////////////////////////////////////////////

static int refillbuffer[256];

//volatile int dcsound_irqticks = 0;

static volatile int aica_irq_ok = 0;

void dcsound_aica_irq(unsigned which) {
  unsigned playpos;
  unsigned dest_bytes = 0;
//  int x;

  if(!aica_irq_ok) return;
  if(!(which & 0x40)) return;

//  x = timer_gettime();

  // get channel 0's play position
  playpos = (g2_readword(SNDREGADDR(0x2814)) + 64) & 0x1FF;

  if(playpos < 256) dest_bytes = 512;
  else dest_bytes = 0;

  if(thefillfunction) {
    thefillfunction(
      (signed short*)(refillbuffer),
      (signed short*)(refillbuffer+128),
      256
    );
  } else {
    memset(refillbuffer, 0, 1024);
  }

  // copy L and R buffers
  g2_copywords((uint32*)SOUNDDRV_SH4PTR32(   0+dest_bytes),refillbuffer+  0,128);
  g2_copywords((uint32*)SOUNDDRV_SH4PTR32(1024+dest_bytes),refillbuffer+128,128);

//  dcsound_irqticks = timer_gettime() - x;

}

/////////////////////////////////////////////////////////////////////////////
//
// using a 2048-byte (256-sample * 2byte * 2chan * 2block) buffer
//
static void dcsound_setup_sample(void) {
  int i;

  unsigned clearbuffer[2048/4];
  memset(clearbuffer, 0, 2048);

  // clear DSP MPRO
  g2_copywords(SNDREGADDR(0x3400), clearbuffer, 2048/4);

  // clear the sample area
//for(i=0;i<1024;i++){((signed short*)clearbuffer)[i]=(i&64)?(0x4000):(0x4000);}
  g2_copywords((uint32*)SOUNDDRV_SH4PTR32(0), clearbuffer, 2048/4);

  // turn all channels off
  for(i = 0; i < 64; i++) g2_writeword(CHNREGADDR(i,0x24), 0x0000);
  for(i = 0; i < 64; i++) g2_writeword(CHNREGADDR(i,0x00), 0x0000);
  for(i = 0; i < 64; i++) g2_writeword(CHNREGADDR(i,0x00), 0x8000);

  // disable all EFSDL
  for(i = 0; i < 0x48; i += 4) g2_writeword(SNDREGADDR(0x2000+i), 0);

  // set up the two stream channels
  for(i = 0; i < 2; i++) {
    g2_writeword(CHNREGADDR(i,0x00), 0x0200); // 16-bit, looping
    g2_writeword(CHNREGADDR(i,0x04), i*0x0400); // sample address
    g2_writeword(CHNREGADDR(i,0x08), 0x0000); // loop start
    g2_writeword(CHNREGADDR(i,0x0C), 0x0200); // loop end (512 samples)
    g2_writeword(CHNREGADDR(i,0x10), 0x001F); // full attack rate
    g2_writeword(CHNREGADDR(i,0x14), 0x0000);
    g2_writeword(CHNREGADDR(i,0x18), 0x0000); // octave/freq.
    g2_writeword(CHNREGADDR(i,0x1C), 0x0000); // no LFO
    g2_writeword(CHNREGADDR(i,0x20), 0x0000); // no TL attenuation
    g2_writeword(CHNREGADDR(i,0x28), 0x0020); // disable lowpass/Q
    // other regs don't matter
  }
  g2_writeword(CHNREGADDR(0,0x24), 0x0F1F); // direct send level
  g2_writeword(CHNREGADDR(1,0x24), 0x0F0F); // direct send level

  // key on both stream channels
  g2_writeword(CHNREGADDR(0,0), 0x4200);
  g2_writeword(CHNREGADDR(1,0), 0x4200);
  g2_writeword(CHNREGADDR(0,0), 0xC200);
  g2_writeword(CHNREGADDR(1,0), 0xC200);

  // we will be monitoring channel 0
  g2_writeword(SNDREGADDR(0x280C), 0);
}

/////////////////////////////////////////////////////////////////////////////

void dcsound_init(void) {
  // disable ARM CPU
  g2_writeword(SNDREGADDR(0x2C00), g2_readword(SNDREGADDR(0x2C00)) | 1);
  // volume off
  g2_writeword(SNDREGADDR(0x2800), 0);
  dcsound_setup_sample();

  // set timer A to interrupt every 256 samples
  g2_writeword(SNDREGADDR(0x2890), 0x000);
  // acknowledge all interrupts
  g2_writeword(SNDREGADDR(0x28BC), g2_readword(SNDREGADDR(0x28B8)));
  // set up MCIEB on the AICA side
  g2_writeword(SNDREGADDR(0x28B4), 0x40);
  aica_irq_ok = 1;

  // volume off
  g2_writeword(SNDREGADDR(0x2800), 0x0);
}

/////////////////////////////////////////////////////////////////////////////

// 0 to 32767
void dcsound_setvolume(unsigned v) {
//  static const int tltable[17] = {
//    0xFF,0x40,0x30,0x28,0x20,0x1C,0x18,0x14,0x10,0xE,0xC,0xA,0x8,0x6,0x4,0x2,0x0
//  };
//  v /= 0x7FF;
//  if(v > 0x10) v = 0x10;
  // use TL to set the volume
//  g2_writeword(CHNREGADDR(0,0x20), tltable[v] << 8);
//  g2_writeword(CHNREGADDR(1,0x20), tltable[v] << 8);
  v /= 0x888;
  if(v > 0xF) v = 0xF;
  g2_writeword(SNDREGADDR(0x2800), v);
}

/////////////////////////////////////////////////////////////////////////////
