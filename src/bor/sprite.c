/////////////////////////////////////////////////////////////////////////////

#include "dcport.h"

#include "types.h"

/////////////////////////////////////////////////////////////////////////////

#define TRANSPARENT_IDX (0x00)

/////////////////////////////////////////////////////////////////////////////

static void putsprite_(
  unsigned char *dest, int x, int *linetab, int h, int screenwidth
) {
  for(; h > 0; h--, dest += screenwidth) {
    register int lx = x;
    unsigned char *data = ((char*)linetab) + (*linetab); linetab++;
    while(lx < screenwidth) {
      register int count = *data++;
      if(count == 0xFF) break;
      lx += count;
      if(lx >= screenwidth) break;
      count = *data++;
      if(!count) continue;
      if((lx + count) <= 0) { lx += count; data += count; continue; }
      if(lx < 0) { count += lx; data -= lx; lx = 0; }
      if((lx + count) > screenwidth) { count = screenwidth - lx; }
      for(; count > 0; count--) dest[lx++] = *data++;
    }
  }
}

static void putsprite_flip_(
  unsigned char *dest, int x, int *linetab, int h, int screenwidth
) {
  for(; h > 0; h--, dest += screenwidth) {
    register int lx = x;
    unsigned char *data = ((char*)linetab) + (*linetab); linetab++;
    while(lx > 0) {
      register int count = *data++;
      if(count == 0xFF) break;
      lx -= count;
      if(lx <= 0) break;
      count = *data++;
      if(!count) continue;
      if((lx - count) >= screenwidth) { lx -= count; data += count; continue; }
      if(lx > screenwidth) { int diff = (lx - screenwidth); count -= diff; data += diff; lx = screenwidth; }
      if((lx - count) < 0) { count = lx; }
      for(; count > 0; count--) dest[--lx] = *data++;
    }
  }
}

static void putsprite_remap_(
  unsigned char *dest, int x, int *linetab, int h, int screenwidth,
  unsigned char *remap
) {
  for(; h > 0; h--, dest += screenwidth) {
    register int lx = x;
    unsigned char *data = ((char*)linetab) + (*linetab); linetab++;
    while(lx < screenwidth) {
      register int count = *data++;
      if(count == 0xFF) break;
      lx += count;
      if(lx >= screenwidth) break;
      count = *data++;
      if(!count) continue;
      if((lx + count) <= 0) { lx += count; data += count; continue; }
      if(lx < 0) { count += lx; data -= lx; lx = 0; }
      if((lx + count) > screenwidth) { count = screenwidth - lx; }
      for(; count > 0; count--) dest[lx++] = remap[((int)(*data++))&0xFF];
    }
  }
}

static void putsprite_remap_flip_(
  unsigned char *dest, int x, int *linetab, int h, int screenwidth,
  unsigned char *remap
) {
  for(; h > 0; h--, dest += screenwidth) {
    register int lx = x;
    unsigned char *data = ((char*)linetab) + (*linetab); linetab++;
    while(lx > 0) {
      register int count = *data++;
      if(count == 0xFF) break;
      lx -= count;
      if(lx <= 0) break;
      count = *data++;
      if(!count) continue;
      if((lx - count) >= screenwidth) { lx -= count; data += count; continue; }
      if(lx > screenwidth) { int diff = (lx - screenwidth); count -= diff; data += diff; lx = screenwidth; }
      if((lx - count) < 0) { count = lx; }
      for(; count > 0; count--) dest[--lx] = remap[((int)(*data++))&0xFF];
    }
  }
}

//src high dest low
static void putsprite_blend_(
  unsigned char *dest, int x, int *linetab, int h, int screenwidth,
  unsigned char *blend
) {
  for(; h > 0; h--, dest += screenwidth) {
    register int lx = x;
    unsigned char *data = ((char*)linetab) + (*linetab); linetab++;
    while(lx < screenwidth) {
      register int count = *data++;
      if(count == 0xFF) break;
      lx += count;
      if(lx >= screenwidth) break;
      count = *data++;
      if(!count) continue;
      if((lx + count) <= 0) { lx += count; data += count; continue; }
      if(lx < 0) { count += lx; data -= lx; lx = 0; }
      if((lx + count) > screenwidth) { count = screenwidth - lx; }
      for(; count > 0; count--) { dest[lx] = blend[((((int)(*data++))&0xFF)<<8)|dest[lx]]; lx++; }
    }
  }
}

static void putsprite_blend_flip_(
  unsigned char *dest, int x, int *linetab, int h, int screenwidth,
  unsigned char *blend
) {
  for(; h > 0; h--, dest += screenwidth) {
    register int lx = x;
    unsigned char *data = ((char*)linetab) + (*linetab); linetab++;
    while(lx > 0) {
      register int count = *data++;
      if(count == 0xFF) break;
      lx -= count;
      if(lx <= 0) break;
      count = *data++;
      if(!count) continue;
      if((lx - count) >= screenwidth) { lx -= count; data += count; continue; }
      if(lx > screenwidth) { int diff = (lx - screenwidth); count -= diff; data += diff; lx = screenwidth; }
      if((lx - count) < 0) { count = lx; }
      for(; count > 0; count--) { --lx; dest[lx] = blend[((((int)(*data++))&0xFF)<<8)|dest[lx]]; }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////

static void all_putsprite(
  int x, int y, s_sprite *sprite, s_screen *screen,
  unsigned char *remap, unsigned char *blend
) {
  int *linetab;
  int w, h;
  char *dest;
  // Get screen size
  int screenwidth = screen->width;
  int screenheight = screen->height;
  int is_flip = 0;
  // Adjust coords for centering
  x -= sprite->centerx;
  y -= sprite->centery;
  // see if this is a flip sprite
  if(sprite->is_flip_of) { is_flip = 1; sprite = sprite->is_flip_of; }
  // Get sprite dimensions
  w = sprite->width;
  h = sprite->height;
  // trivial clip all directions
  if(x >= screenwidth) return;
  if((x+w) <= 0) return;
  if(y >= screenheight) return;
  if((y+h) <= 0) return;
  // Init line table pointer
  linetab = (int*)(sprite->data);
  // clip top
  if(y < 0) {
    h += y; // subtract from height
    linetab -= y; // add to linetab
    y = 0; // add to y
  }
  // clip bottom
  if((y+h) > screenheight) {
    h = screenheight - y;
  }
  // calculate destination pointer
  dest = ((char*)(screen->data)) + y*screenwidth;

  if(blend) {
    if(is_flip) putsprite_blend_flip_(dest, x+w, linetab, h, screenwidth, blend);
    else        putsprite_blend_     (dest, x  , linetab, h, screenwidth, blend);
  } else if(remap) {
    if(is_flip) putsprite_remap_flip_(dest, x+w, linetab, h, screenwidth, remap);
    else        putsprite_remap_     (dest, x  , linetab, h, screenwidth, remap);
  } else {
    if(is_flip) putsprite_flip_      (dest, x+w, linetab, h, screenwidth);
    else        putsprite_           (dest, x  , linetab, h, screenwidth);
  }
}

/////////////////////////////////////////////////////////////////////////////

void putsprite(int x, int y, s_sprite *sprite, s_screen *screen) {
  all_putsprite(x,y,sprite,screen,NULL,NULL);
}

void putsprite_remap(int x, int y, s_sprite *sprite, s_screen *screen, char *remap) {
  all_putsprite(x,y,sprite,screen,remap,NULL);
}

void putsprite_blend(int x, int y, s_sprite *sprite, s_screen *screen, char *blend) {
  all_putsprite(x,y,sprite,screen,NULL,blend);
}

/////////////////////////////////////////////////////////////////////////////
//
// NULL for dest means do not actually encode
//
int encodesprite(
  int centerx, int centery,
  s_bitmap *bitmap, s_sprite *dest
) {
  int x, x0, y, w, h;
  unsigned char *data;
  int *linetab;
  unsigned char *src = bitmap->data;

  if(bitmap->width <= 0 || bitmap->height <= 0){
    // Image is empty (or bad), create an empty sprite
    if(dest) {
      dest->is_flip_of = NULL;
      dest->centerx = 0;
      dest->centery = 0;
      dest->width = 0;
      dest->height = 0;
    }
    return 4*5;
  }

  w = bitmap->width;
  h = bitmap->height;

  if(dest) {
    dest->is_flip_of = NULL;
    dest->centerx = centerx;
    dest->centery = centery;
    dest->width = w;
    dest->height = h;
  }
  linetab = (int*)(dest->data);
  data = (unsigned char*)(linetab+h);

  for(y = 0; y < h; y++, src += w) {
    if(dest) { linetab[y] = ((int)data)-((int)(linetab+y)); }
    x = 0;
    for(;;) {
      // search for the first visible pixel
      x0 = x;
      for(; (x < w) && ((x-x0)<0xFE); x++) { if(src[x]) break; }
      // handle EOL
      if(x >= w) { if(dest) { *data = 0xFF; } data++; break; }
      // encode clearcount
      if(dest) { *data = x-x0; } data++;
      // if we're still not visible, encode a null visible count and continue
      if(!src[x]) { if(dest) { *data = 0; } data++; continue; }
      // search for the first invisible pixel
      x0 = x;
      for(; (x < w) && ((x-x0)<0xFF); x++) { if(!src[x]) break; }
      // encode viscount and visible pixels
      if(dest) {
        *data++ = x-x0;
        memcpy(data, src+x0, x-x0);
        data += x-x0;
      } else {
        data += 1+(x-x0);
      }
    }
  }

  return ((int)data)-((int)dest);
}

/////////////////////////////////////////////////////////////////////////////

int fakey_encodesprite(s_bitmap *bitmap) {
  return encodesprite(0, 0, bitmap, NULL);
}

/////////////////////////////////////////////////////////////////////////////

int fakey_encodesprite_flip(s_bitmap *bitmap) {
  return 4 * 5;
}

int encodesprite_flip(
  int centerx, int centery,
  s_sprite *src, s_sprite *dest
) {
  if(src->width <= 0 || src->height <= 0) {
    // Image is empty (or bad), create an empty sprite
    if(dest) {
      dest->is_flip_of = NULL;
      dest->centerx = 0;
      dest->centery = 0;
      dest->width = 0;
      dest->height = 0;
    }
    return 4*5;
  }
  if(dest) {
    dest->is_flip_of = src;
    dest->centerx = centerx;
    dest->centery = centery;
    dest->width = src->width;
    dest->height = src->height;
  }
  return 4 * 5;
}

/////////////////////////////////////////////////////////////////////////////
