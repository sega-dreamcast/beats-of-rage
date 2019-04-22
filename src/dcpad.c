/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"
#include "dcpad.h"

/////////////////////////////////////////////////////////////////////////////

#define MAPLEREGBASE (0xA05F6C00)

#define MAPLEREG_DMAADDR (*((vuint32*)(MAPLEREGBASE + 0x04)))
#define MAPLEREG_RESET2  (*((vuint32*)(MAPLEREGBASE + 0x10)))
#define MAPLEREG_ENABLE  (*((vuint32*)(MAPLEREGBASE + 0x14)))
#define MAPLEREG_STATE   (*((vuint32*)(MAPLEREGBASE + 0x18)))
#define MAPLEREG_SPEED   (*((vuint32*)(MAPLEREGBASE + 0x80)))
#define MAPLEREG_RESET1  (*((vuint32*)(MAPLEREGBASE + 0x8C)))

/* Some register define values */
#define MAPLE_RESET1_MAGIC     (0x6155404F)
#define MAPLE_RESET2_MAGIC     (0)
#define MAPLE_SPEED_2MBPS      (0)
#define MAPLE_SPEED_TIMEOUT(n) ((n) << 16)

#define MAPLE_FUNC_CONTROLLER (0x01000000)

/////////////////////////////////////////////////////////////////////////////

static volatile char dcpad_controller_attached[4] = {0,0,0,0};
volatile unsigned dcpad_buttonstate0[4] = {0,0,0,0};
volatile unsigned dcpad_buttonstate1[4] = {0,0,0,0};

/////////////////////////////////////////////////////////////////////////////

static unsigned dcpad_dmabuffer[8+16+1024];

static volatile unsigned *dcpad_msend;
static volatile unsigned *dcpad_mrecv[4];

static int dcpad_ready_to_poll = 0;

/////////////////////////////////////////////////////////////////////////////
//
// Poll button state
// (quick, doesn't block, safe to call from interrupts, etc.)
//
void dcpad_poll(void) {
  int port, ofs;

  if(!dcpad_ready_to_poll) return;

  // if dma is still busy, skip this
  if(MAPLEREG_STATE & 1) return;

  // examine results from last poll, if any
  for(port = 0; port < 4; port++) {
    // examine response code
    switch(dcpad_mrecv[port][0] & 0xFF) {
    case 0x05: // device info
      dcpad_controller_attached[port] = 1;
      dcpad_buttonstate0[port] = 0x0000FFFF;
      dcpad_buttonstate1[port] = 0x80808080;
      break;
    case 0x08: // device condition
      if((dcpad_mrecv[port][1] & 0xFF000000) == 0x01000000) {
        dcpad_buttonstate0[port] = dcpad_mrecv[port][2];
        dcpad_buttonstate1[port] = dcpad_mrecv[port][3];
      }
      break;
    default: // dunno
      dcpad_controller_attached[port] = 0;
      dcpad_buttonstate0[port] = 0x0000FFFF;
      dcpad_buttonstate1[port] = 0x80808080;
      break;
    }
  }

  // now send a new poll

  // set up send/recv buffers
  ofs = 0;
  for(port = 0; port < 4; port++) {
    unsigned cmd;
    if(dcpad_controller_attached[port]) {
      cmd = 0x09; // get condition
    } else {
      cmd = 0x01; // get device info
    }
    dcpad_msend[ofs++] = 0x00000001 | (port << 16) | (port == 3 ? 0x80000000 : 0);
    dcpad_msend[ofs++] = ((unsigned)dcpad_mrecv[port]) & 0x1FFFFFFF;
    dcpad_msend[ofs++] = 0x01002000 | (port << 14) | cmd;
    dcpad_msend[ofs++] = MAPLE_FUNC_CONTROLLER;
    dcpad_mrecv[port][0] = 0xFFFFFFFF;
  }

  MAPLEREG_DMAADDR = ((unsigned)dcpad_msend) & 0x1FFFFFFF;
  MAPLEREG_STATE = 1;
}

/////////////////////////////////////////////////////////////////////////////

void dcpad_init(void) {
  unsigned *t;
  // init maple bus
  MAPLEREG_RESET1 = MAPLE_RESET1_MAGIC;
  MAPLEREG_RESET2 = MAPLE_RESET2_MAGIC;
  MAPLEREG_SPEED = MAPLE_SPEED_2MBPS | MAPLE_SPEED_TIMEOUT(50000);
  MAPLEREG_ENABLE = 1;
  // set up pointers
  t = (unsigned*)((((unsigned)dcpad_dmabuffer)|0xA000001F)+1);
  dcpad_msend = t; t += 16;
  dcpad_mrecv[0] = t; t += 256;
  dcpad_mrecv[1] = t; t += 256;
  dcpad_mrecv[2] = t; t += 256;
  dcpad_mrecv[3] = t; t += 256;
  // clear recv buffers
  dcpad_mrecv[0][0] = 0xFFFFFFFF;
  dcpad_mrecv[1][0] = 0xFFFFFFFF;
  dcpad_mrecv[2][0] = 0xFFFFFFFF;
  dcpad_mrecv[3][0] = 0xFFFFFFFF;
  // wait for dma
  while(MAPLEREG_STATE & 1);
  // perform initial poll
  dcpad_ready_to_poll = 1;
  dcpad_poll();
}

/////////////////////////////////////////////////////////////////////////////
