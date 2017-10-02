TARGET		= game
OBJS		= main.o ./common/callbacks.o

PSPBIN = $(PSPSDK)/../bin
CFLAGS += $(shell $(PSPBIN)/sdl-config --cflags)
LIBS += $(shell $(PSPBIN)/sdl-config --libs)

INCDIR		=
CFLAGS		= -G0 -Wall -O2
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)

LIBDIR =
LDFLAGS	=
LIBS = -lSDL_image -lSDL_mixer -lSDL_TTF -lSDL -LSDLmain -lmikmod \
        -lpspaudiolib -lvorbisidec -losl -lmpeg2 -lvorbisfile \
        -logg -lsmpeg -lpspaudio -lFLAC -lssp -lpspmp3 -lmmio \
        -lglut -lGLU -lGL -lpspirkeyb -lpspwlan -lpsppower -lGL -lfreetype -ljpeg -lpng -lz -lm -lpspgu -l psphprm -lpspaudio -lstdc++ -lpspvfpu -lpsprtc

BUILD_PRX = 1 

EXTRA_TARGETS	= EBOOT.PBP
PSP_EBOOT_TITLE= Space Invaders
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG
PSP_EBOOT_SND0 = NULL

PSPSDK	= $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak