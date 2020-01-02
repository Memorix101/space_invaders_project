# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Copyright 2014 doctorxyz
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#

.SILENT:

EE_BIN = ps2spacegame.elf
EE_BIN_PKD = PS2SPACEGAME.ELF
EE_OBJS = main.o
EE_INCS := -I/usr/lib/ps2dev/ps2sdk/ports/include 
EE_CXXFLAGS += -D_EE -O2 -G0 -Wall -fno-exceptions 
EE_LIBS	+= -L/usr/lib/ps2dev/ps2sdk/ports/lib
EE_LIBS = -L$(PS2SDK)/ports/lib -lSDL_image -lsdlmixer -lSDL_ttf -lsdl -Lsdlmain -lm -ldma -lmad -lmikmod -lz -lfreetype -lpng -ljpeg -lstdc++ -lps2snd -lc -lkernel 

all:
	echo "Building..."
		$(MAKE) main.cpp $(EE_BIN)
	
	echo "Stripping..."
	ee-strip $(EE_BIN)

	echo "Compressing..."
	ps2-packer $(EE_BIN) $(EE_BIN_PKD) > /dev/null

#IRX Modules
	echo "Preaparing... irx"

freesio2.c:
	bin2c $(PS2SDK)/iop/irx/freesio2.irx freesio2_irx.c freesio2_irx

padman.c:
	bin2c $(PS2SDK)/iop/irx/padman.irx padman_irx.c padman_irx

usbd_irx.c:
	bin2c $(PS2SDK)/iop/irx/usbd.irx usbd_irx.c usbd_irx

usbhdfsd_irx.c:
	bin2c $(PS2SDK)/iop/irx/usbhdfsd.irx usbhdfsd_irx.c usbhdfsd_irx

usbmass_bd_irx.c:
	bin2c $(PS2SDK)/iop/irx/usbmass_bd.irx usbmass_bd.irx.c usbmass_bd_irx

# You can replace the mickey.raw texture below by yours.
# Just pick up a 512x512 24-Bit RGB BMP image, then convert it to PS2 BGR RAW, using bmp2raw tool
# (https://bitbucket.org/doctorxyz/bmp2raw)
#texture.c:
	#bin2c mickey.raw texture.c texture

clean:
	echo "Cleaning..."
	rm -f *.ELF *.elf *.o main.cpp

rebuild: clean all

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal