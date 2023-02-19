BASEDIR	:= $(dir $(firstword $(MAKEFILE_LIST)))
VPATH	:= $(BASEDIR)

export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=/opt/devkitpro/devkitPPC
export WUT_ROOT=/opt/devkitpro/wut
export PORTLIBS=/opt/devkitpro/portlibs

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing header files
#---------------------------------------------------------------------------------
TARGET		:=	space-invaders-wiiu
SOURCES		:=	src
INCLUDES	:=	

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
CFLAGS		+=	$(INCLUDES)
CXXFLAGS	+=	$(CFLAGS) -I$(DEVKITPRO)/portlibs/wiiu/include -fpermissive
LDFLAGS		+=	$(WUT_NEWLIB_LDFLAGS) $(WUT_STDCPP_LDFLAGS) $(WUT_DEVOPTAB_LDFLAGS) \
				-L/opt/devkitpro/portlibs/ppc/lib  -L/opt/devkitpro/portlibs/wiiu/lib \
				-lSDL2_ttf -lfreetype -lpng -lSDL2_gfx -lSDL2_image -lSDL2 -ljpeg -lz -lbz2 \
				-lmad -logg -lmpg123 -lgcc -lvorbisidec -lSDL2_mixer \
				-lcoreinit -lvpad -lsndcore2 -lzlib125 -lsysapp -lnsysnet -lnlibcurl -lproc_ui -lgx2 -lgfd -lwhb
				#lmad -logg -lmpg123 -lgcc -lvorbisidec -lSDL2_mixer \
#---------------------------------------------------------------------------------
# get a list of objects
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))
SFILES		:=	$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.s))
OBJECTS		:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

#---------------------------------------------------------------------------------
# objectives
#---------------------------------------------------------------------------------
$(TARGET).rpx: $(OBJECTS)

clean:
	rm -rf $(TARGET).rpx $(OBJECTS) $(OBJECTS:.o=.d)

#---------------------------------------------------------------------------------
# wut
#---------------------------------------------------------------------------------
include $(WUT_ROOT)/share/wut.mk

#---------------------------------------------------------------------------------
# portlibs
#---------------------------------------------------------------------------------
PORTLIBS	:=	$(DEVKITPRO)/portlibs/ppc
LDFLAGS		+=	-L$(PORTLIBS)/lib
CFLAGS		+=	-I$(PORTLIBS)/wiiu/include -I$(PORTLIBS)/wiiu/include/SDL2
CXXFLAGS	+=	-I$(PORTLIBS)/wiiu/include -I$(PORTLIBS)/wiiu/include/SDL2
