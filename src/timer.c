#include "dcport.h"
#include "timer.h"

#define  TIMER8(o) (*((vuint8 *)(0xFFD80000+(o))))
#define TIMER16(o) (*((vuint16*)(0xFFD80000+(o))))
#define TIMER32(o) (*((vuint32*)(0xFFD80000+(o))))
#define TOCR     TIMER8(0x00)
#define TSTR     TIMER8(0x04)
#define TCOR0   TIMER32(0x08)
#define TCNT0   TIMER32(0x0C)
#define TCR0    TIMER16(0x10)
#define TCOR1   TIMER32(0x14)
#define TCNT1   TIMER32(0x18)
#define TCR1    TIMER16(0x1C)
#define TCOR2   TIMER32(0x20)
#define TCNT2   TIMER32(0x24)
#define TCR2    TIMER16(0x28)
#define TCPR2   TIMER32(0x2C)

#define GETTIME_FREQ (12500000)

static unsigned lastinterval;

unsigned timer_gettime(void) {
  return ~(TCNT0);
}

void timer_init(void) {
  TOCR = 0;  /* Set to internal clock source */
  TSTR = 0;  /* Disable all timers */

  /* Set mode/underflow */
  TCR0 = 0x0000;
  TCOR0 = 0xFFFFFFFF;
  TCNT0 = 0xFFFFFFFF;

  /* Start channel 0 */
  TSTR |= 0x01;

  lastinterval = timer_gettime();
}

void timer_exit(void) { }

unsigned timer_getinterval(unsigned freq) {
  unsigned tickspassed,ebx,blocksize;
  ebx=timer_gettime()-lastinterval;
  blocksize=GETTIME_FREQ/freq;
  ebx+=GETTIME_FREQ%freq;
  tickspassed=ebx/blocksize;
  ebx-=ebx%blocksize;
  lastinterval+=ebx;
  return tickspassed;
}
