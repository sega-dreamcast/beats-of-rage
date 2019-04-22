/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"

/////////////////////////////////////////////////////////////////////////////

extern int vbr_syscall_errno;

/////////////////////////////////////////////////////////////////////////////

int dcloadsyscall(int syscall, ...);

#define DCLOADMAGICVALUE 0xDEADBEEF
#define DCLOADMAGICADDR (unsigned*)0x8C004004

static int dcload_read(int file, char *ptr, int len) {
  if(*DCLOADMAGICADDR == DCLOADMAGICVALUE) return dcloadsyscall(0, file, ptr, len);
  return -1;
}

static int dcload_lseek(int file, int ptr, int dir) {
  if(*DCLOADMAGICADDR == DCLOADMAGICVALUE) return dcloadsyscall(9, file, ptr, dir);
  return -1;
}

int dcload_write(int file, const char *ptr, int len) {
  if(*DCLOADMAGICADDR == DCLOADMAGICVALUE) return dcloadsyscall(1, file, ptr, len);
  return -1;
}

static int dcload_close(int file) {
  if(*DCLOADMAGICADDR == DCLOADMAGICVALUE) return dcloadsyscall(3, file);
  return -1;
}

static int dcload_open(const char *path, int flags) {
  if(*DCLOADMAGICADDR == DCLOADMAGICVALUE) return dcloadsyscall(2, path, flags);
  return -1;
}

/////////////////////////////////////////////////////////////////////////////

int syscall_handler(int num, int arg0, int arg1, int arg2) {
  char *msg;
  switch(num) {
  case 1: // exit
    msg = "program exited (0x0000)\n";
    msg[18] = "0123456789ABCDEF"[(arg0>>12)&0xF];
    msg[19] = "0123456789ABCDEF"[(arg0>> 8)&0xF];
    msg[20] = "0123456789ABCDEF"[(arg0>> 4)&0xF];
    msg[21] = "0123456789ABCDEF"[(arg0>> 0)&0xF];
    dcload_write(1, msg, 24);
    for(;;);
  case 3:    return dcload_read(arg0, (char*)arg1, arg2);
  case 4:    return dcload_write(arg0, (const char*)arg1, arg2);
  case 5:    return dcload_open((const char*)arg0, arg1);
  case 6:    return dcload_close(arg0);
  case 19:   return dcload_lseek(arg0, arg1, arg2);
  default:
    msg = "unknown syscall 0x00\n";
    msg[18] = "0123456789ABCDEF"[(num>>4)&0xF];
    msg[19] = "0123456789ABCDEF"[(num>>0)&0xF];
    dcload_write(1, msg, 21);
    for(;;);
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////
