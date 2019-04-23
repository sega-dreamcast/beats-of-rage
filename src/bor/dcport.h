#ifndef DCPORT_H
#define DCPORT_H

/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <errno.h>

/////////////////////////////////////////////////////////////////////////////

#define DCPORT

#undef USE_TRACEMALLOC

/////////////////////////////////////////////////////////////////////////////

#define uint8  unsigned char
#define uint16 unsigned short
#define uint32 unsigned int

#define sint8  signed char
#define sint16 signed short
#define sint32 signed int

#define float32  float

#define vuint8   volatile unsigned char
#define vuint16  volatile unsigned short
#define vuint32  volatile unsigned int
#define vfloat32 volatile float

#define gamelib_long int

/////////////////////////////////////////////////////////////////////////////

#define ARRAY_ENTRIES(a) (sizeof(a)/sizeof((a)[0]))

#define OBTAIN_LOCK(x) ({int r;asm volatile("tas.b @%1\n\tmovt %0":"=r"(r):"r"(&(x)):"memory","cc");r;})
#define CLEAR_LOCK(x) ({asm volatile("mov.b %1,@%0"::"r"(&(x)),"r"(0):"memory");0;})
#define TEST_LOCK(x) ({int r;asm volatile("mov.b @%1,%0\n\tshll16 %0\n\tshll8 %0\n\tshll %0\n\tmovt %0":"=r"(r):"r"(&(x)):"cc");r;})

#define CURRENT_FPSCR ({uint32 x;asm volatile("sts fpscr,%0\n\tnop":"=r"(x));x;})
#define CURRENT_SR ({uint32 x;asm volatile("stc sr,%0":"=r"(x));x;})
#define SET_CURRENT_SR(x) {asm volatile("ldc %0,sr\n\tnop"::"r"(x));}
#define CURRENT_VBR ({uint32 x;asm volatile("stc vbr,%0":"=r"(x));x;})
#define SET_CURRENT_VBR(x) {asm volatile("ldc %0,vbr\n\tnop"::"r"(x));}
#define CURRENT_SP ({uint32 x;asm volatile("mov r15,%0":"=r"(x));x;})

#define int_mask() ({uint32 m=CURRENT_SR;SET_CURRENT_SR(m|0xF0);m;})
#define int_restore(m) {SET_CURRENT_SR(m);}
#define int_enable() {SET_CURRENT_SR(0x40000000);}

#define TRAPA(n) ({asm volatile("trapa %0"::"i"(n));0;})

#define cache_pref(x) ({asm volatile("pref @%0"::"r"((uint32)(x)):"memory");0;})
#define cache_ocbwb(x) ({asm volatile("ocbwb @%0"::"r"((uint32)(x)):"memory");0;})
#define cache_ocbi(x) ({asm volatile("ocbi @%0"::"r"((uint32)(x)):"memory");0;})
#define cache_ocbp(x) ({asm volatile("ocbp @%0"::"r"((uint32)(x)):"memory");0;})

/////////////////////////////////////////////////////////////////////////////

unsigned readmsb32(const unsigned char *src);
unsigned readlsb32(const unsigned char *src);

/////////////////////////////////////////////////////////////////////////////

#ifdef USE_TRACEMALLOC

#include "tracemalloc.h"

#ifndef I_AM_TRACEMALLOC
#define malloc dont_use_malloc_dammit
#define free dont_use_free_dammit
#endif

#else

#define tracemalloc(a,b) malloc(b)
#define tracefree(b) free(b)

#endif

/////////////////////////////////////////////////////////////////////////////

#include "dcsound.h"
#define SB_setvolume(a,b) dcsound_setvolume(((unsigned)(b))*0x888)

/////////////////////////////////////////////////////////////////////////////

#endif
