/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2016, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/** @file image.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine Image definition and tools
 *  @bug No known bugs.
 */

#ifndef __JO_IMAGE_H__
# define __JO_IMAGE_H__

/** @brief Get tile count in table (see Tileset Demo)
 *  @param TILE_TABLE jo_tile[] TABLE
 */
# define JO_TILE_COUNT(TILE_TABLE)    (sizeof(TILE_TABLE) / sizeof(jo_tile))

/** @brief 15 bits image struct */
typedef struct
{
	unsigned short	width;
	unsigned short	height;
	jo_color        *data;
}					jo_img;

/** @brief 8 bits image struct */
typedef struct
{
	unsigned short	width;
	unsigned short	height;
	unsigned char   *data;
}					jo_img_8bits;

/** @brief Struct of one tile (pos and size) */
typedef struct
{
	unsigned short	x;
	unsigned short	y;
	unsigned short	width;
	unsigned short	height;
}					jo_tile;

/** @brief Add a sprite
 *  @param img Pointer to an image struct
 *  @param src_color Color to replace
 *  @param dest_color New color
 */
void	jo_replace_color(const jo_img * const img, const jo_color src_color, const jo_color dest_color);

/** @brief Load a BIN image
 *  @param img Image (set data to NULL for dynamic allocation)
 *  @param sub_dir Sub directory name (use JO_ROOT_DIR if the file is on the root directory)
 *  @param filename Filename (upper case and shorter as possible like "A.BIN")
 *  @param transparent_color Transparent color (see colors.h). Use JO_COLOR_Transparent by default
 *  @return true if succeeded otherwise false
 */
bool        jo_bin_loader(jo_img *img, const char * const sub_dir, const char *const filename, const jo_color transparent_color);

/** @brief Load a BIN image from stream
 *  @param img Image (set data to NULL for dynamic allocation)
 *  @param stream Raw bin file contents
 *  @param transparent_color Transparent color (see colors.h). Use JO_COLOR_Transparent by default
 *  @return true if succeeded otherwise false
 */
bool        jo_bin_loader_from_stream(jo_img *img, char *stream, const jo_color transparent_color);

/** @brief Add tileset sprites from a BIN file
 *  @param sub_dir Sub directory name (use JO_ROOT_DIR if the file is on the root directory)
 *  @param filename Filename (upper case and shorter as possible like "A.BIN")
 *  @param transparent_color Transparent color (see colors.h). Use JO_COLOR_Transparent by default
 *  @param tileset Table of tiles
 *  @param tile_count Number of tile in the entire image
 *  @return Sprite Id of the first image or -1 if failed
 */
int		jo_sprite_add_bin_tileset(const char * const sub_dir, const char * const filename, const jo_color transparent_color, const jo_tile * const tileset, const unsigned int tile_count);

/** @brief Add a sprite from a BIN file
 *  @param sub_dir Sub directory name (use JO_ROOT_DIR if the file is on the root directory)
 *  @param filename Filename (upper case and shorter as possible like "A.BIN")
 *  @param transparent_color Transparent color (see colors.h). Use JO_COLOR_Transparent by default
 *  @return Sprite Id or -1 if failed
 */
int		jo_sprite_add_bin(const char * const sub_dir, const char * const filename, const jo_color transparent_color);

/** @brief Add a sprite from a BIN file
 *  @param stream Raw bin file contents
 *  @param transparent_color Transparent color (see colors.h). Use JO_COLOR_Transparent by default
 *  @return Sprite Id or -1 if failed
 */
int     jo_sprite_add_bin_from_stream(char *stream, const jo_color transparent_color);

/** @brief Add a set of image from a TEX file format
 *  @param sub_dir Sub directory name (use JO_ROOT_DIR if the file is on the root directory)
 *  @param filename Filename (upper case and shorter as possible like "A.TEX")
 *  @param transparent_color Transparent color (see colors.h). Use JO_COLOR_Transparent by default
 *  @details  Sample file with 3 Textures (it's very similar to the MAP file format):
 *  @details  1.TGA
 *  @details  2.BIN
 *  @details  3.TGA
 *  @return Sprite Id of the first image or -1 if failed
 */
int		jo_sprite_add_image_pack(const char * const sub_dir, const char * const filename, const jo_color transparent_color);

/** @brief Free an image loaded from CD
 *  @param img Pointer to an image struct
 */
void	jo_free_img(jo_img * const img);

/** @brief Set palette index color
 *  @param palette_index palette Index from 1 to 255
 *  @param color Color example for green: JO_COLOR_RGB(0, 63, 63)
 */
static  __jo_force_inline void        jo_set_palette_register(const unsigned char palette_index, const jo_color color) {*(jo_color *)(VDP2_COLRAM + ((palette_index) << 1)) = color; }

/** @brief Get palette color from index
 *  @param palette_index palette Index from 1 to 255
 *  @return color Color
 */
static  __jo_force_inline jo_color    jo_get_palette_register(const unsigned char palette_index) { return *(jo_color *)(VDP2_COLRAM + ((palette_index) << 1)); }

#endif /* !__JO_IMAGE_H__ */

/*
** END OF FILE
*/
