SH4PATH=../sh-elf

CC=$(SH4PATH)/bin/sh-elf-gcc -ml -m4-single-only -ffast-math
AS=$(SH4PATH)/bin/sh-elf-as -little
OBJCOPY=$(SH4PATH)/bin/sh-elf-objcopy

CFLAGS=-I$(SH4PATH)/sh-elf/include -Wall -Werror -O9 -fomit-frame-pointer -fno-ident -freorder-blocks -funsigned-char

#########################
#
# BUILD NTSC MASTER 
#
########################
ifeq ($(NTSC),1)
CFLAGS+=-DNTSC
endif
#########################
#
# OR PAL MASTER 
#
########################
ifeq ($(PAL),1)
CFLAGS+=-DPAL
endif


LFLAGS=-Wl,-T,dc.x,-Map,linkmap -nostdlib
LIBS=-lc -lgcc

all: ../NTSC/1ST_READ.BIN

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

../NTSC/1ST_READ.BIN: 1ST_READ.elf
	$(OBJCOPY) -S -R .stack --gap-fill 0 -O binary $< $@
	pad32 ../NTSC/1ST_READ.BIN

../PAL/1ST_READ.BIN: 1ST_READ.elf
	$(OBJCOPY) -S -R .stack --gap-fill 0 -O binary $< $@
	pad32 ../PAL/1ST_READ.BIN

clean:
	rm -f *.o *.elf *.cs linkmap ../NTSC/1ST_READ.BIN ../PAL/1ST_READ.BIN
