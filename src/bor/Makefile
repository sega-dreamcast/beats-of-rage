SDKPATH=../../sdk
SH4PATH=$(SDKPATH)/sh-elf/bin

CC=$(SH4PATH)/sh-elf-gcc -ml -m4-single-only -ffast-math
AS=$(SH4PATH)/sh-elf-as -little
OBJCOPY=$(SH4PATH)/sh-elf-objcopy

PAD32=$(SDKPATH)/opt/pad32
RM=$(SDKPATH)/bin/rm

CFLAGS=-Wall -Werror -O9 -fomit-frame-pointer -fno-ident -freorder-blocks -funsigned-char -I$(SH4PATH)/../include -I$(SH4PATH)/../sh-elf/include

ifdef PAL
  CFLAGS+=-DPAL
else
  CFLAGS+=-DNTSC
endif

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

bor.elf: $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $@ $(LIBS)

1ST_READ.BIN: bor.elf
	$(OBJCOPY) -S -R .stack --gap-fill 0 -O binary $< $@
	$(PAD32) 1ST_READ.BIN

clean:
	$(RM) -f *.o *.elf *.cs linkmap
