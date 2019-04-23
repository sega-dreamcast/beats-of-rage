#ifndef DCPAD_H
#define DCPAD_H

/////////////////////////////////////////////////////////////////////////////
#define RUMBLE_STRONG_VALUE 	0x3339f010
#define RUMBLE_SHUTDOWN_VALUE	0x011a7010
#define RUMBLE_STOP_VALUE	0x00000010
#define MAKE_RUMBLE_VALUE(freq, pow, len) ((unsigned) (0x11 << 24) | (freq << 16) | (pow << 8) | (len ))


extern volatile unsigned dcpad_buttonstate0[4];
extern volatile unsigned dcpad_buttonstate1[4];
extern volatile unsigned dcpad_rumblepower[4];

void dcpad_init(void);
void dcpad_poll(void);

/////////////////////////////////////////////////////////////////////////////

#endif
