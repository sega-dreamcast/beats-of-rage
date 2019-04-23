/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"

#include "dcirq.h"
#include "dcvga.h"
#include "timer.h"
#include "dcsound.h"
#include "dcpad.h"
#include "gdrom.h"
#include "dcpak.h"

#include "borstartup.h"

/////////////////////////////////////////////////////////////////////////////

unsigned readmsb32(const unsigned char *src) {
  return
    ((((unsigned)(src[0])) & 0xFF) << 24) |
    ((((unsigned)(src[1])) & 0xFF) << 16) |
    ((((unsigned)(src[2])) & 0xFF) <<  8) |
    ((((unsigned)(src[3])) & 0xFF) <<  0);
}

unsigned readlsb32(const unsigned char *src) {
  return
    ((((unsigned)(src[0])) & 0xFF) <<  0) |
    ((((unsigned)(src[1])) & 0xFF) <<  8) |
    ((((unsigned)(src[2])) & 0xFF) << 16) |
    ((((unsigned)(src[3])) & 0xFF) << 24);
}

/////////////////////////////////////////////////////////////////////////////

void bor_main(int argc, char **argv);

/////////////////////////////////////////////////////////////////////////////

int main(void) {
  int cd_lba;

  // it's quite important that we initialize this first, otherwise the
  // newlib syscalls won't work
  dcirq_init();
  dcsound_init();
  dcvga_init();
  timer_init();
  dcpad_init();

  borstartup_draw();

//  printf("testing testing la la la %08X %d %d '%s'\n",0xdeadbeef,-1,1,"ham ham haaaaaaaam");
  cd_lba = gdrom_init();
//  printf("cd_lba = %d\n", cd_lba);
  if(cd_lba <= 0) { printf("gdrom_init failed\n"); abort(); }
  dcpak_init(cd_lba);

  bor_main(0, 0);

  return 0;
}

/////////////////////////////////////////////////////////////////////////////
