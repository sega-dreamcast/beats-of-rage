#ifndef __TIMER_H
#define __TIMER_H

void timer_init(void);
void timer_exit(void);
unsigned timer_gettime(void);
unsigned timer_getinterval(unsigned freq);

#endif
