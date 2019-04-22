#ifndef DCSOUND_H
#define DCSOUND_H

/////////////////////////////////////////////////////////////////////////////
//
// Initialization
//
void dcsound_init(void);

typedef void (*dcsound_fillstereosamples_t)(signed short *l, signed short *r, int n);

void dcsound_setfillfunction(dcsound_fillstereosamples_t fillfunction);

// 0 to 32767
void dcsound_setvolume(unsigned v);

void dcsound_aica_irq(unsigned which);

/////////////////////////////////////////////////////////////////////////////

#endif
