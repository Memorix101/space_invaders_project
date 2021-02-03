EE_BIN = GAME.ELF
#EE_BIN_PKD = GAME.ELF
EE_OBJS =  romfs.o romfs_io.o main.o
EE_INCS = -I$(PS2SDK)/ports/include 
#EE_CXXFLAGS += -D_EE -O1 -G0 -Wall -fno-exceptions 
#EE_LIBS	+= -L$(PS2SDK)/ports/lib
EE_CFLAGS += -DSYS_LITTLE_ENDIAN -DSYS_NEED_ALIGNMENT
EE_LIBS = -L$(PS2SDK)/ports/lib -lromfs -lSDL_image -lsdlmixer -lSDL_ttf -lsdl -Lsdlmain -lm -lmad -lmikmod -lz -lfreetype -lpng -ljpeg -lstdc++ -lps2snd

all: $(EE_BIN)

install:

clean:
	rm -f $(EE_BIN) $(EE_OBJS) romdisk.img

include Makefile.romfs
#include $(PS2SDK)/Defs.make
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp
