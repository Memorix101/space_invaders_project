XBE_TITLE = SpaceInvadersNXDK
GEN_XISO = $(XBE_TITLE).iso
SRCS = $(CURDIR)/main.cpp
NXDK_DIR ?= $(CURDIR)/../..
NXDK_SDL = y

include $(NXDK_DIR)/Makefile

TARGET += $(OUTPUT_DIR)/rd/space3.png
TARGET += $(OUTPUT_DIR)/rd/invader32x32x4.png
TARGET += $(OUTPUT_DIR)/rd/player.png
TARGET += $(OUTPUT_DIR)/rd/bullet.png
TARGET += $(OUTPUT_DIR)/rd/enemy-bullet.png
TARGET += $(OUTPUT_DIR)/rd/explode.png
TARGET += $(OUTPUT_DIR)/rd/fmg_splash.png
TARGET += $(OUTPUT_DIR)/rd/gameover_ui.png
TARGET += $(OUTPUT_DIR)/rd/win_ui.png
TARGET += $(OUTPUT_DIR)/rd/blaster.mp3
TARGET += $(OUTPUT_DIR)/rd/explode1.wav
TARGET += $(OUTPUT_DIR)/rd/pusher.wav
TARGET += $(OUTPUT_DIR)/rd/bodenstaendig.mp3
TARGET += $(OUTPUT_DIR)/rd/vermin_vibes_1989.ttf

$(GEN_XISO): $(OUTPUT_DIR)/rd/space3.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/invader32x32x4.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/player.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/bullet.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/enemy-bullet.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/explode.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/fmg_splash.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/gameover_ui.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/win_ui.png
$(GEN_XISO): $(OUTPUT_DIR)/rd/blaster.mp3
$(GEN_XISO): $(OUTPUT_DIR)/rd/explode1.wav
$(GEN_XISO): $(OUTPUT_DIR)/rd/pusher.wav
$(GEN_XISO): $(OUTPUT_DIR)/rd/bodenstaendig.mp3
$(GEN_XISO): $(OUTPUT_DIR)/rd/vermin_vibes_1989.ttf

$(OUTPUT_DIR)/rd/space3.png: $(CURDIR)/rd/space3.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/invader32x32x4.png: $(CURDIR)/rd/invader32x32x4.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/player.png: $(CURDIR)/rd/player.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/bullet.png: $(CURDIR)/rd/bullet.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/enemy-bullet.png: $(CURDIR)/rd/enemy-bullet.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/explode.png: $(CURDIR)/rd/explode.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/fmg_splash.png: $(CURDIR)/rd/fmg_splash.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/gameover_ui.png: $(CURDIR)/rd/gameover_ui.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/win_ui.png: $(CURDIR)/rd/win_ui.png $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/blaster.mp3: $(CURDIR)/rd/blaster.mp3 $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/explode1.wav: $(CURDIR)/rd/explode1.wav $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/pusher.wav: $(CURDIR)/rd/pusher.wav $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/bodenstaendig.mp3: $(CURDIR)/rd/bodenstaendig.mp3 $(OUTPUT_DIR)
$(OUTPUT_DIR)/rd/vermin_vibes_1989.ttf: $(CURDIR)/rd/vermin_vibes_1989.ttf $(OUTPUT_DIR)
	$(VE)cp '$<' '$@'
