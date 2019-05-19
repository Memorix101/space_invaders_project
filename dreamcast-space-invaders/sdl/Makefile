TARGET = space_invaders.elf
OBJS = IMG.o IMG_png.o IMG_bmp.o main.o romdisk.o

KOS_ROMDISK_DIR = romdisk

all: clean $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS) romdisk.*

$(TARGET): $(OBJS)
	kos-cc -o $(TARGET) $(OBJS) $(DATAOBJS) $(OBJEXTRA) -lpng -lz -lmp3 -lm -lSDL -ltremor

run:
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	$(KOS_STRIP) $(TARGET)
