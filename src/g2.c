#include "dcport.h"
#include "g2.h"

#define G2_FIFO_WAIT() \
  { \
    for(;;) { \
      if(!((*((volatile int*)0xA05F688C)) & 0x31)) break; \
    } \
  }

#define G2_LOCK(i) \
  { i = int_mask(); \
    G2_FIFO_WAIT(); \
  }

#define G2_UNLOCK(i) \
  { G2_FIFO_WAIT(); \
    int_restore(i); \
  }

#define G2LVARS int g2lockvar;

#define G2L G2_LOCK(g2lockvar);
#define G2U G2_UNLOCK(g2lockvar);

char g2_readbyte(char *address) {
  G2LVARS;
  char r;
  G2L;
  r = *((volatile char*)address);
  G2U;
  return r;
}

void g2_writebyte(char *address, char data) {
  G2LVARS;
  G2L;
  *((volatile char*)address) = data;
  G2U;
}

int g2_readword(int *address) {
  G2LVARS;
  int r;
  G2L;
  r = *((volatile int*)address);
  G2U;
  return r;
}

void g2_writeword(int *address, int data) {
  G2LVARS;
  G2L;
  *((volatile int*)address) = data;
  G2U;
}

void g2_copybytes(char *dst, const char *src, int n) {
  G2LVARS;
  while(n) {
    int t = n;
    if(t > 8) t = 8;
    n -= t;
    G2L;
    do {
      *dst++ = *src++;
    } while(--t);
    G2U;
  }
}

/*
void g2_copywords(int *dst, const int *src, int n) {
  G2LVARS;
  while(n) {
    int t = n;
    if(t > 8) t = 8;
    n -= t;
    G2L;
    do {
      *dst++ = *src++;
    } while(--t);
    G2U;
  }
}
*/

void g2_copywords(int *dst, const int *src, int n) {
  G2LVARS;
  G2L;
  do { *dst++ = *src++; } while(--n);
  G2U;
}
