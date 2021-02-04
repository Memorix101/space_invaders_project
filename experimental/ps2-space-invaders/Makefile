EE_BIN = GAME.ELF
#EE_BIN_PKD = GAME.ELF
EE_OBJS =  main.o
EE_INCS = -I$(PS2SDK)/ports/include 
EE_CXXFLAGS += -D_EE -O1 -G0 -Wall -fno-exceptions -fpermissive
#EE_LIBS	+= -L$(PS2SDK)/ports/lib
EE_LIBS += -L$(PS2SDK)/ports/lib -lpad -lSDL_image -lsdlmixer -lSDL_ttf -lsdl -Lsdlmain -lm -lmad -lmikmod -lz -lfreetype -lpng -ljpeg -lstdc++ -lps2snd -lc -lmc

all: $(EE_BIN) sio2man.irx padman.irx usbd.irx usbmass_bd.irx audsrv.irx

sio2man.irx:
	cp $(PS2SDK)/iop/irx/sio2man.irx $@

padman.irx:
	cp $(PS2SDK)/iop/irx/padman.irx $@

usbd.irx:
	cp $(PS2SDK)/iop/irx/usbd.irx $@

usbhdfsd.irx:
	cp $(PS2SDK)/iop/irx/usbhdfsd.irx $@

audsrv.irx:
	cp $(PS2SDK)/iop/irx/audsrv.irx $@

install:

run: $(EE_BIN)
	ps2client -h 192.168.178.50 execee host:$(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp
