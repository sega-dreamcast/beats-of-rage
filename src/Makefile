SH4PATH=/usr/local/dc/sh-elf/bin

CC=$(SH4PATH)/sh-elf-gcc -ml -m4-single-only -ffast-math
AS=$(SH4PATH)/sh-elf-as -little
OBJCOPY=$(SH4PATH)/sh-elf-objcopy

CFLAGS=-Wall -Werror -O9 -fomit-frame-pointer -fno-ident -freorder-blocks -funsigned-char

#########################
#
# BUILD NTSC MASTER 
#
########################
CFLAGS+=-DNTSC

#########################
#
# OR PAL MASTER 
#
########################
# CFLAGS+=-DPAL

LFLAGS=-Wl,-T,dc.x,-Map,linkmap -nostdlib
LIBS=-lc -lgcc

all: 1ST_READ.BIN

OBJECTS=crt0.o dcport.o dcloadsc.o dcmath.o syscalls.o bios.o gdrom.o \
	vbr.o dcirq.o dcvga.o g2.o timer.o dcsound.o dcpad.o \
	dcpak.o filecache.o \
	adpcm.o anigif.o bitmap.o control.o draw.o font.o loadimg.o \
	palette.o rand32.o screen.o soundmix.o sprite.o spriteq.o \
	texture.o vga.o video.o \
	borstartup.o bor.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.o: %.s
	$(CC) $(CFLAGS) -c $<

%.cs: %.c
	$(CC) $(CFLAGS) -S -c $< -o $@

1ST_READ.elf: $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $@ $(LIBS)

1ST_READ.BIN: 1ST_READ.elf
	$(OBJCOPY) -S -R .stack --gap-fill 0 -O binary $< $@
	pad32 1ST_READ.BIN

clean:
	rm *.o *.elf *.cs linkmap 1ST_READ.BIN
