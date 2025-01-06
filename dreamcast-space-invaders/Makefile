TARGET = space-invaders-dc.elf
OBJS = font.o glfx.o timer.o main.o pvr-texture.o romdisk.o

KOS_ROMDISK_DIR = romdisk

all: clean $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS) romdisk.*  space-invaders-dreamcast.cdi space-invaders-dc.bin  ./cd_root/1ST_READ.BIN

$(TARGET): $(OBJS)
	kos-cc -o $(TARGET) $(OBJS) $(DATAOBJS) $(OBJEXTRA) -lz -lKGL -lpcx -lkosutils -lmp3 -lm -ltremor

run:
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	$(KOS_STRIP) $(TARGET)