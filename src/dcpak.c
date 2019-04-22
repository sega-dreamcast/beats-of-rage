/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"
#include "dcpak.h"
#include "packfile.h"
#include "filecache.h"
#include "gdrom.h"

/////////////////////////////////////////////////////////////////////////////
//
// CACHEBLOCKSIZE*CACHEBLOCKS is the size of the ever-present file cache
// cacheblocks must be 255 or less!
//
#define CACHEBLOCKSIZE (32768)
#define CACHEBLOCKS    (96)

/////////////////////////////////////////////////////////////////////////////

const char dcpakfilename[] = "bor.pak";

#define DCPAK_VFDS 8

static int dcpakfd;
static int dcpaksize;

static int dcpak_vfdexists[DCPAK_VFDS];
static int dcpak_vfdstart[DCPAK_VFDS];
static int dcpak_vfdsize[DCPAK_VFDS];
static int dcpak_vfdpos[DCPAK_VFDS];
static int dcpak_vfdreadahead[DCPAK_VFDS];

/////////////////////////////////////////////////////////////////////////////

char myfilenametolower(char c) {
  if(c >= 'A' && c <= 'Z') c += 'a' - 'A';
  if(c == '\\') c = '/';
  return c;
}

int myfilenamecmp(const char *a, const char *b) {
  for(;;) {
    char ca = *a++;
    char cb = *b++;
    if((!ca) && (!cb)) break;
    ca = myfilenametolower(ca);
    cb = myfilenametolower(cb);
    if(ca < cb) return -1;
    if(ca > cb) return 1;
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////

static int dcpak_headerstart;
static int dcpak_headersize;

static unsigned char *dcpak_cdheader;
static unsigned char *dcpak_header;

/////////////////////////////////////////////////////////////////////////////
//
// returns number of sectors read successfully
//
static int dcpak_getsectors(void *dest, int lba, int n) {
  if((lba+n) > ((dcpaksize+0x7FF)/0x800)) {
    n = ((dcpaksize+0x7FF)/0x800)-lba;
  }
//printf("dcpak_getsectors(dest,lba=%d,n=%d) dcpakfd=%d\n",lba,n,dcpakfd);
  if(dcpakfd >= 0) {
    lseek(dcpakfd, lba << 11, SEEK_SET);
    read(dcpakfd, dest, n << 11);
  } else {
//printf("lalala %d %d\n", (-dcpakfd)+lba, n);
    gdrom_readsectors(dest, (-dcpakfd)+lba, n);
    while(gdrom_poll());
  }
  return n;
}

/////////////////////////////////////////////////////////////////////////////
//
// returns 0 if they match
//
static int fncmp(const char *filename, const char *isofilename, int isolen) {
  for(; isolen > 0; isolen--) {
    char cf = *filename++;
    char ci = *isofilename++;
    if(!cf) {
      // allowed to omit the version on filename
      if(ci == ';' || ci == 0) return 0;
      return 1;
    }
    if(cf >= 'A' && cf <= 'Z') cf += 'a' - 'A';
    if(ci >= 'A' && ci <= 'Z') ci += 'a' - 'A';
    if(cf != ci) return 1;
  }
  // allowed to omit the version on isofilename too O_o
  if(*filename == ';' || *filename == 0) return 0;
  return 1;
}

/////////////////////////////////////////////////////////////////////////////
//
// input: starting lba of the track
// returns starting lba of the file, or 0 on failure
//
int dcpak_find_iso_file(const char *filename, int lba, int *bytes) {
  int dirlen;
  unsigned char sector[4096];
  int secofs;

  // read the root descriptor
  gdrom_readsectors(sector, lba+16, 1); while(gdrom_poll());
  // get the root directory extent and size
  lba    = 150 + readmsb32(sector+156+6);
  dirlen =       readmsb32(sector+156+14);

//printf("lba=%d dirlen=%d\n",lba,dirlen);

  // at this point, lba is the lba of the root dir
  secofs = 4096;
  while(dirlen > 0) {
    if(secofs >= 4096 || ((secofs + sector[secofs]) > 4096)) {
      memcpy(sector, sector+2048, 2048);
      gdrom_readsectors(sector+2048, lba, 1); while(gdrom_poll());
      lba++;
      secofs -= 2048;
    }
    if(!sector[secofs]) break;
//printf("filename length=%d\n",(int)(sector[secofs+32]));
    if(!fncmp(filename, sector+secofs+33, sector[secofs+32])) {
      lba = 150 + readmsb32(sector+secofs+6);
      if(bytes) *bytes = readmsb32(sector+secofs+14);
      return lba;
    }
    secofs += sector[secofs];
    dirlen -= sector[secofs];
  }
  // didn't find the file
  return 0;
}

/////////////////////////////////////////////////////////////////////////////

void dcpak_init(int use_cd_lba) {
  int i;
  unsigned char *sectors;

  if(!use_cd_lba) {
    dcpakfd = open(dcpakfilename, O_RDONLY);
    if(dcpakfd < 0) { printf("unable to open pak file\n"); abort(); }
    dcpaksize = lseek(dcpakfd, 0, SEEK_END);
  } else {
    dcpaksize = 0;
    dcpakfd = dcpak_find_iso_file(dcpakfilename, use_cd_lba, &dcpaksize);
    if(dcpakfd <= 0) { printf("unable to find pak file on cd\n"); abort(); }
//    printf("found pak file on cd at extent %d size %d\n", dcpakfd, dcpaksize);
//{char sector[2048];
//      gdrom_readsectors(sector, dcpakfd, 1); while(gdrom_poll());
//printf("pak contents = %c %c %c %c\n",sector[0],sector[1],sector[2],sector[3]);
//}
    dcpakfd = -dcpakfd;
  }

//printf("(dcpaksize = %d)\n", dcpaksize);

  sectors = tracemalloc("dcpak_init", 4096);
  if(!sectors) { printf("alloc failed\n"); for(;;); }

  { int getptrfrom = dcpaksize - 4;
    if(dcpak_getsectors(sectors, getptrfrom >> 11, 2) < 1) {
      printf("unable to read pak header pointer; halting\n");
      for(;;);
    }
    dcpak_headerstart = readlsb32(sectors + (getptrfrom & 0x7FF));
  }

//printf("headerstart=%d\n",dcpak_headerstart);

  tracefree(sectors);

  if(dcpak_headerstart >= dcpaksize) {
    printf("invalid pak header pointer; halting\n");
    for(;;);
  }

  dcpak_headersize = dcpaksize - dcpak_headerstart;

  {
    // let's cache it on CD sector boundaries
    int dcpak_cdheaderstart = dcpak_headerstart & (~0x7FF);
    int dcpak_cdheadersize = ((dcpaksize - dcpak_cdheaderstart) + 0x7FF) & (~0x7FF);

    if(dcpak_cdheadersize > 262144) {
      printf("pak header is too large; halting\n");
      for(;;);
    }

    dcpak_cdheader = tracemalloc("dcpak_init", dcpak_cdheadersize);
    if(!dcpak_cdheader) { printf("alloc failed\n"); for(;;); }
    if(dcpak_getsectors(dcpak_cdheader, dcpak_cdheaderstart >> 11, dcpak_cdheadersize >> 11) != (dcpak_cdheadersize >> 11)) {
      printf("unable to read pak header; halting\n");
      for(;;);
    }
    // ok, header is now cached

    dcpak_header = dcpak_cdheader + (dcpak_headerstart & 0x7FF);
  }

  // header does not include the last 4-byte stuff
  if(dcpak_headersize >= 4) {
    dcpak_headersize -= 4;
    // add a trailing null o/~
    dcpak_header[dcpak_headersize] = 0;
  }

//printf("dcpak cached header (%d bytes)\n", dcpak_headersize);

  // initialize vfd table
  for(i = 0; i < DCPAK_VFDS; i++) {
    dcpak_vfdexists[i] = 0;
  }

  // finally, initialize the filecache
  filecache_init(
    dcpakfd,
    (dcpaksize + 0x7FF) / 0x800,
    CACHEBLOCKSIZE,
    CACHEBLOCKS,
    DCPAK_VFDS
  );

}

/////////////////////////////////////////////////////////////////////////////

void update_filecache_vfd(int vfd) {
  if(dcpak_vfdexists[vfd]) {
    filecache_setvfd(vfd,
      dcpak_vfdstart[vfd],
      (dcpak_vfdstart[vfd] + dcpak_vfdpos[vfd]) / CACHEBLOCKSIZE,
      (dcpak_vfdreadahead[vfd] + (CACHEBLOCKSIZE-1)) / CACHEBLOCKSIZE
    );
  } else {
    filecache_setvfd(vfd, -1, -1, 0);
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// ONLY USE THIS AFTER THE INIT IS FINISHED
//
static int dcpak_rawread(int fd, unsigned char *dest, int len, int blocking) {
  int end;
  int r;
  int total = 0;

  int pos = dcpak_vfdstart[fd] + dcpak_vfdpos[fd];

  if(pos < 0) return 0;
  if(pos >= dcpaksize) return 0;
  if((pos + len) > dcpaksize) { len = dcpaksize - pos; }
  end = pos + len;

  update_filecache_vfd(fd);

  while(pos < end) {
    int b = pos / CACHEBLOCKSIZE;
    int startthisblock = pos % CACHEBLOCKSIZE;
    int sizethisblock = CACHEBLOCKSIZE - startthisblock;
    if(sizethisblock > (end-pos)) sizethisblock = (end-pos);
//printf("a");
    r = filecache_readpakblock(dest, b, startthisblock, sizethisblock, blocking);
//printf("b(%d)",r);
    if(r >= 0) {
      total += r;
      dcpak_vfdpos[fd] += r;
      update_filecache_vfd(fd);
    }
    if(r < sizethisblock) break;

    dest += sizethisblock;
    pos += sizethisblock;
  }

  return total;
}

/////////////////////////////////////////////////////////////////////////////

int openreadaheadpackfile(char *filename, char *packfilename, int readaheadsize, int prebuffersize) {
  int hpos;
  int vfd;

  if(myfilenamecmp(packfilename, dcpakfilename)) {
    printf("tried to open from unknown pack file (%s)\n", packfilename);
    for(;;);
  }

  // find a free vfd
  for(vfd = 0; vfd < DCPAK_VFDS; vfd++) {
    if(!dcpak_vfdexists[vfd]) break;
  }
  if(vfd >= DCPAK_VFDS) return -1;

  // look for filename in the header
  hpos = 0;
  for(;;) {
    if((hpos + 12) >= dcpak_headersize) return -1;
    if(myfilenamecmp(dcpak_header + hpos + 12, filename)) {
      hpos += readlsb32(dcpak_header + hpos);
      continue;
    }
    // found!
    dcpak_vfdstart[vfd] = readlsb32(dcpak_header + hpos + 4);
    dcpak_vfdsize[vfd] = readlsb32(dcpak_header + hpos + 8);
    break;
  }

  // have a little read-ahead
//  if(readaheadsize < 131072) readaheadsize = 131072;

  dcpak_vfdpos[vfd] = 0;
  dcpak_vfdexists[vfd] = 1;
  dcpak_vfdreadahead[vfd] = readaheadsize;

  // notify filecache that we have a new vfd
  update_filecache_vfd(vfd);

  // if we want prebuffering, wait for it
  if(prebuffersize > 0) {
    filecache_wait_for_prebuffer(vfd, (prebuffersize + ((CACHEBLOCKSIZE)-1)) / CACHEBLOCKSIZE);
  }

  return vfd;
}

int openpackfile(char *filename, char *packfilename) {
  return openreadaheadpackfile(filename, packfilename, 0, 0);
}

/////////////////////////////////////////////////////////////////////////////

int dcpak_isvalidfd(int fd) {
  if(fd < 0 || fd >= DCPAK_VFDS) return 0;
  if(!dcpak_vfdexists[fd]) return 0;
  return 1;
}

/////////////////////////////////////////////////////////////////////////////

int readpackfileblocking(int fd, void *buf, int len, int blocking) {
  int n;

  if(!dcpak_isvalidfd(fd)) return -1;

  if(dcpak_vfdpos[fd] < 0) return 0;
  if(dcpak_vfdpos[fd] > dcpak_vfdsize[fd]) dcpak_vfdpos[fd] = dcpak_vfdsize[fd];
  if((len + dcpak_vfdpos[fd]) > dcpak_vfdsize[fd]) { len = dcpak_vfdsize[fd] - dcpak_vfdpos[fd]; }
  if(len < 1) return 0;

  update_filecache_vfd(fd);

  n = dcpak_rawread(fd, buf, len, blocking);

  if(n < 0) n = 0;

  if(dcpak_vfdpos[fd] > dcpak_vfdsize[fd]) dcpak_vfdpos[fd] = dcpak_vfdsize[fd];

  update_filecache_vfd(fd);

  return n;
}

int readpackfile(int fd, void *buf, int len) {
  return readpackfileblocking(fd, buf, len, 1);
}

int readpackfile_noblock(int fd, void *buf, int len) {
  return readpackfileblocking(fd, buf, len, 0);
}

int packfileeof(int fd) {
  if(!dcpak_isvalidfd(fd)) return -1;
  return (dcpak_vfdpos[fd] >= dcpak_vfdsize[fd]);
}

/////////////////////////////////////////////////////////////////////////////

int closepackfile(int fd) {
  if(!dcpak_isvalidfd(fd)) return -1;
  dcpak_vfdexists[fd] = 0;
  update_filecache_vfd(fd);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////

int seekpackfile(int fd, int n, int whence) {
  if(!dcpak_isvalidfd(fd)) return -1;
  switch(whence) {
  case 0: dcpak_vfdpos[fd] = n; break; // set
  case 1: dcpak_vfdpos[fd] += n; break; // cur
  case 2: dcpak_vfdpos[fd] = dcpak_vfdsize[fd] + n; break; // end
  default: return -1;
  }
  // original code had this check too, so do it here
  if(dcpak_vfdpos[fd] < 0) dcpak_vfdpos[fd] = 0;
  if(dcpak_vfdpos[fd] > dcpak_vfdsize[fd]) dcpak_vfdpos[fd] = dcpak_vfdsize[fd];

  update_filecache_vfd(fd);

  return dcpak_vfdpos[fd];
}

/////////////////////////////////////////////////////////////////////////////

