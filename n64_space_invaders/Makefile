BUILD_DIR      = build
SOURCE_DIR     = src
ASSET_DIR      = assets
FILESYSTEM_DIR = filesystem

# Fallback in case N64_INST is not provided by the environment
N64_INST ?= /pyrite64-sdk
T3D_INST ?= $(N64_INST)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/include/t3d.mk

N64_ROM_TITLE = "Space Invaders 64"
N64_ROM_SAVETYPE = none

SRCS = $(wildcard $(SOURCE_DIR)/*.c)
OBJS = $(SRCS:$(SOURCE_DIR)/%.c=$(BUILD_DIR)/%.o)

# assets
MODELS  = $(wildcard $(ASSET_DIR)/*.glb)
SPRITES = $(wildcard $(ASSET_DIR)/*.png)
TTF = $(wildcard $(ASSET_DIR)/*.ttf)
WAV = $(wildcard $(ASSET_DIR)/*.wav)
ASSETS  = $(MODELS:$(ASSET_DIR)/%.glb=$(FILESYSTEM_DIR)/%.t3dm) \
          $(SPRITES:$(ASSET_DIR)/%.png=$(FILESYSTEM_DIR)/%.sprite) \
          $(TTF:$(ASSET_DIR)/%.ttf=$(FILESYSTEM_DIR)/%.font64) \
          $(WAV:$(ASSET_DIR)/%.wav=$(FILESYSTEM_DIR)/%.wav64)

MKSPRITE_FLAGS ?=
# The four full-screen images as CI8 instead of RGBA16. space3 alone would be
# 960 KB and each UI screen 600 KB, which together is more RAM than the N64
# has. rdpq_sprite_blit uploads the palette itself and activates TLUT.
$(FILESYSTEM_DIR)/space3.sprite:      MKSPRITE_FLAGS = -f CI8
$(FILESYSTEM_DIR)/fmg_splash.sprite:  MKSPRITE_FLAGS = -f CI8
$(FILESYSTEM_DIR)/gameover_ui.sprite: MKSPRITE_FLAGS = -f CI8
$(FILESYSTEM_DIR)/win_ui.sprite:      MKSPRITE_FLAGS = -f CI8

# The explosion sheet is 2048 px wide, which is past the 10 bit texture
# coordinate limit of the RDP, so main.c addresses each frame as a sub-surface
# via rdpq_tex_blit. That function does not upload palettes, so the sheet must
# not be palettized. Left on AUTO, mksprite picks CI4 and the frames render
# through whatever palette was last active. RGBA16 also buys the explosion
# more than the 16 colors CI4 would allow.
$(FILESYSTEM_DIR)/explode.sprite:     MKSPRITE_FLAGS = -f RGBA16

# Left on AUTO, mksprite picks CI4 for these, which quantizes them down to 16
# colors and visibly dirties the sprites the player looks at most. Together
# they are only about 10 KB as RGBA16, so the quality is nearly free.
$(FILESYSTEM_DIR)/player.sprite:          MKSPRITE_FLAGS = -f RGBA16
$(FILESYSTEM_DIR)/bullet.sprite:          MKSPRITE_FLAGS = -f RGBA16
$(FILESYSTEM_DIR)/enemy-bullet.sprite:    MKSPRITE_FLAGS = -f RGBA16
$(FILESYSTEM_DIR)/invader32x32x4.sprite:  MKSPRITE_FLAGS = -f RGBA16

# Rendering happens at 640x480, the same as the SDL original, so use the same
# point size it did.
MKFONT_FLAGS ?= -s 24
# Default for short SFX: mono + VADPCM, loaded fully into RAM.
# Mono is not optional here: a stereo wave occupies two consecutive mixer
# channels and would clobber the next channel.
AUDIOCONV_FLAGS ?= --wav-mono

all: space_invaders_64.z64

# Tiny3D model: glTF -> .t3dm, then compress with mkasset (level 2)
$(FILESYSTEM_DIR)/%.t3dm: $(ASSET_DIR)/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@
	$(N64_BINDIR)/mkasset -c 2 -o $(FILESYSTEM_DIR) $@

$(FILESYSTEM_DIR)/%.sprite: $(ASSET_DIR)/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o $(FILESYSTEM_DIR) "$<"

$(FILESYSTEM_DIR)/%.font64: $(ASSET_DIR)/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	$(N64_MKFONT) $(MKFONT_FLAGS) -o $(FILESYSTEM_DIR) "$<"

# Background music: 31 MB of stereo PCM would not fit into the DFS. Mono +
# Opus, plus loop and seek points so it can be streamed at runtime.
# Opus always outputs 48 kHz no matter what is set here, but --wav-resample
# 32000 still has an effect: it limits the bandwidth before the encoder and
# shrinks the file from 1.11 MB to 823 KB. Add it back if size matters.
$(FILESYSTEM_DIR)/bodenstaendig.wav64: AUDIOCONV_FLAGS = --wav-mono --wav-compress 3 \
                                                        --wav-loop true --wav-seek 1.0

$(FILESYSTEM_DIR)/%.wav64: $(ASSET_DIR)/%.wav
	@mkdir -p $(dir $@)
	@echo "    [AUDIO] $@"
	$(N64_AUDIOCONV) $(AUDIOCONV_FLAGS) -o $(FILESYSTEM_DIR) "$<"

$(BUILD_DIR)/space_invaders_64.dfs: $(ASSETS)
$(BUILD_DIR)/space_invaders_64.elf: $(OBJS)

space_invaders_64.z64: $(BUILD_DIR)/space_invaders_64.dfs

clean:
	rm -rf $(BUILD_DIR) $(FILESYSTEM_DIR) space_invaders_64.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
