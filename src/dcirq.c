/////////////////////////////////////////////////////////////////////////////
//
// Dreamcast IRQ stuff
//
/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"

#include "dcirq.h"
#include "g2.h"

#include "dcsound.h"
#include "dcpad.h"

/////////////////////////////////////////////////////////////////////////////

#define ASIC_ACK_A     (*(vuint32*)0xA05F6900)
#define ASIC_ACK_B     (*(vuint32*)0xA05F6904)
#define ASIC_ACK_C     (*(vuint32*)0xA05F6908)
#define ASIC_IRQD_A    (*(vuint32*)0xA05F6910)
#define ASIC_IRQD_B    (*(vuint32*)0xA05F6914)
#define ASIC_IRQD_C    (*(vuint32*)0xA05F6918)
#define ASIC_IRQB_A    (*(vuint32*)0xA05F6920)
#define ASIC_IRQB_B    (*(vuint32*)0xA05F6924)
#define ASIC_IRQB_C    (*(vuint32*)0xA05F6928)
#define ASIC_IRQ9_A    (*(vuint32*)0xA05F6930)
#define ASIC_IRQ9_B    (*(vuint32*)0xA05F6934)
#define ASIC_IRQ9_C    (*(vuint32*)0xA05F6938)

/////////////////////////////////////////////////////////////////////////////

void dcirq_c_handler(void) {
  uint32 a, b, c;
  // Acknowledge ASIC events
  a = ASIC_ACK_A;
  b = ASIC_ACK_B;
  c = ASIC_ACK_C;
  ASIC_ACK_A = a;
  ASIC_ACK_B = b;
  ASIC_ACK_C = c;
  // Acknowledge and handle AICA IRQ if there was one
  if(b & 0x00000002) {
    unsigned aicairq = g2_readword((void*)0xA07028B8);
    g2_writeword((void*)0xA07028BC, aicairq);
    dcsound_aica_irq(aicairq);
  }
  // Do controller polling, regardless of what kind of interrupt this was
  dcpad_poll();
}

/////////////////////////////////////////////////////////////////////////////

void vbr_exp_handler(void);
int old_vbr;

void dcirq_init(void) {
  int_mask();

  old_vbr = CURRENT_VBR;
  SET_CURRENT_VBR(((unsigned)vbr_exp_handler)-0x100);

  // disable, clear AICA interrupts
  g2_writeword((void*)0xA07028B4, 0);
  g2_writeword((void*)0xA07028BC, g2_readword((void*)0xA07028B8));

  // now route the interrupts on the ASIC side
  ASIC_IRQD_A = 0;
  ASIC_IRQD_B = 0;
  ASIC_IRQD_C = 0;
  ASIC_IRQB_A = 0;
  ASIC_IRQB_B = 0;
  ASIC_IRQB_C = 0;
  ASIC_IRQ9_A = 0x00001000; // MAPLE DMA IRQ
  ASIC_IRQ9_B = 0x00000002; // AICA IRQ
  ASIC_IRQ9_C = 0;
  ASIC_ACK_A = 0xFFFFFFFF;
  ASIC_ACK_B = 0xFFFFFFFF;
  ASIC_ACK_C = 0xFFFFFFFF;

  int_enable();
}

/////////////////////////////////////////////////////////////////////////////
