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
/** @file sprites.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine Sprite definition and tools
 *  @bug No known bugs.
 */

#ifndef __JO_SPRITES_H__
# define __JO_SPRITES_H__

/*
** INCLUDES
*/
#include "image.h"
#include "tga.h"
#include "math.h"
#include "colors.h"

/** @brief Current displayed sprite attribute (internal engine usage)
 *  @warning MC Hammer: don't touch this
 */
extern SPR_ATTR __jo_sprite_attr_8bits;
extern SPR_ATTR __jo_sprite_attr;
extern SPR_ATTR *__jo_current_sprite_attr;
/** @brief Current displayed sprite position (internal engine usage)
 *  @warning MC Hammer: don't touch this
 */
extern FIXED	__jo_sprite_pos[XYZS];
/** @brief (internal engine usage)
 *  @warning MC Hammer: don't touch this
 */
extern TEXTURE	__jo_sprite_def[JO_MAX_SPRITE];
/** @brief (internal engine usage)
 *  @warning MC Hammer: don't touch this
 */
extern PICTURE  __jo_sprite_pic[JO_MAX_SPRITE];

/*
** INTERNAL
*/
/** @brief Internal usage don't use it */
static  __jo_force_inline void	__internal_jo_sprite_set_position3D(FIXED *location, const int x, const int y, const int z) { location[0] = JO_MULT_BY_65536(x); location[1] = JO_MULT_BY_65536(y); location[2] = JO_MULT_BY_65536(z); }

/** @brief Internal usage don't use it */
static  __jo_force_inline void	__internal_jo_sprite_draw(int sprite_id, FIXED *location) { __jo_sprite_attr.texno = sprite_id; slDispSprite(location, &__jo_sprite_attr, 0); }

/** @brief Get last Sprite Id
 *  @return Last Sprite Id
 */
int     jo_get_last_sprite_id(void);

/** @brief Add a sprite
 *  @param img Pointer to a 15 bits image struct
 *  @return Sprite Id or -1 if failed
 */
int		jo_sprite_add(const jo_img * const img);

/** @brief Add a 8 bits sprite
 *  @param img Pointer to a 8 bits bits image struct
 *  @return Sprite Id or -1 if failed
 */
int     jo_sprite_add_8bits_image(const jo_img_8bits * const img);

/** @brief Replace a sprite
 *  @param img Pointer to a image struct
 *  @param sprite_id Sprite ID to replace
 *  @return sprite_id
 */
int     jo_sprite_replace(const jo_img * const img, const int sprite_id);

/** @brief Free all sprites from the given sprite_id
 *  @param sprite_id Sprite ID to replace
 *  @warning Don't use freed sprites after this call or the game may crash unexpectedly
 */
void    jo_sprite_free_from(const int sprite_id);

/** @brief Free all sprites
 *  @warning Don't use any sprites after this call or the game may crash unexpectedly
 */
static  __jo_force_inline void	jo_sprite_free_all(void) { jo_sprite_free_from(0); }

/** @brief Retrive the Sprite Id from filename based on the four first character in the filename
 *  @param filename Filename (upper case and shorter as possible like "A.TEX")
 *  @warning Slow, use carefully
 *  @return Sprite Id of the first image or -1 if not found
 */
int		jo_sprite_name2id(const char *const filename);

/*
** Half transparency
*/
/** @brief Activate half transparency for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_enable_half_transparency(void) { __jo_sprite_attr.atrb |= CL_Trans; __jo_sprite_attr_8bits.atrb |= CL_Trans; }

/** @brief Disable half transparency for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_disable_half_transparency(void) { __jo_sprite_attr.atrb &= ~(CL_Trans); __jo_sprite_attr_8bits.atrb &= ~(CL_Trans); }

/*
** Dark filter
*/
/** @brief Activate dark filter for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_enable_dark_filter(void) { __jo_sprite_attr.atrb |= CL_Half; __jo_sprite_attr_8bits.atrb |= CL_Half; }

/** @brief Disable dark filter for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_disable_dark_filter(void) { __jo_sprite_attr.atrb &= ~(CL_Half); __jo_sprite_attr_8bits.atrb &= ~(CL_Half); }

/*
** Gouraud Shading
*/
/** @brief Value used to keep the original color for jo_set_gouraud_shading_colors() */
# define JO_NO_GOURAUD_COLOR            (0xc210)

/** @brief Change Gouraud Shading colors
 *  @param topleft_color Top left corner color in sprite
 *  @param topright_color Top right corner color in sprite
 *  @param bottomright_color bottom right corner color in sprite
 *  @param bottomleft_color bottom left corner color in sprite
 */
void	jo_set_gouraud_shading_colors(const jo_color topleft_color,
                                      const jo_color topright_color,
                                      const jo_color bottomright_color,
                                      const jo_color bottomleft_color);

/** @brief Activate Gouraud Shading for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_enable_gouraud_shading(void) { __jo_sprite_attr.atrb |= CL_Gouraud; __jo_sprite_attr_8bits.atrb |= CL_Gouraud; }

/** @brief Disable Gouraud Shading for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_disable_gouraud_shading(void) { __jo_sprite_attr.atrb &= ~(CL_Gouraud); __jo_sprite_attr_8bits.atrb &= ~(CL_Gouraud); }

/** @brief Default brightness for jo_set_gouraud_shading_brightness() */
# define JO_DEFAULT_BRIGHTNESS      (16)

/** @brief Maximum brightness value for jo_set_gouraud_shading_brightness() */
# define JO_MAX_BRIGHTNESS          (31)

/** @brief Change Gouraud Shading brightness
 *  @param brightness Brightness from 0 to 31
 */
void     jo_set_gouraud_shading_brightness(const unsigned char brightness);

/*
** Vertical flip
*/
/** @brief Activate vertical flip for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_enable_vertical_flip(void) { __jo_sprite_attr.dir |= 32; __jo_sprite_attr_8bits.dir |= 32; }

/** @brief Disable vertical flip for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_disable_vertical_flip(void) { __jo_sprite_attr.dir &= ~32; __jo_sprite_attr_8bits.dir &= ~32; }

/*
** Horizontal flip
*/
/** @brief Activate horizontal flip for every sprite displayed after this call */
static  __jo_force_inline void	jo_sprite_enable_horizontal_flip(void) { __jo_sprite_attr.dir |= 16; __jo_sprite_attr_8bits.dir |= 16; }

/** @brief Disable horizontal flip for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_disable_horizontal_flip(void) { __jo_sprite_attr.dir &= ~16; __jo_sprite_attr_8bits.dir &= ~16; }

/*
** Change sprite scale
*/
/** @brief Change scale for every sprite displayed after this call
 *  @param scale Sprite scale
 */
static  __jo_force_inline void	jo_sprite_change_sprite_scale(const float scale) { __jo_sprite_pos[3] = toFIXED(scale); }

/** @brief Restore default scale for every sprite displayed after this call
 */
static  __jo_force_inline void	jo_sprite_restore_sprite_scale(void) { __jo_sprite_pos[3] = JO_FIXED_1; }

/** @brief Display a sprite in 3 dimensional space
 *  @param sprite_id Sprite Id
 *  @param x Horizontal position from the center of the screen
 *  @param y Vertical position from the center of the screen
 *  @param z Z index (depth)
 *  @param angle Angle (Z axe)
 */
static  __jo_force_inline void	jo_sprite_draw3D_and_rotate(const int sprite_id, const int x, const int y, const int z, const int angle) { __internal_jo_sprite_set_position3D(__jo_sprite_pos, x, y, z); __jo_current_sprite_attr = ((__jo_sprite_pic[sprite_id].cmode & 1) ? &__jo_sprite_attr : &__jo_sprite_attr_8bits); __jo_current_sprite_attr->texno = sprite_id; slDispSprite((FIXED *)__jo_sprite_pos, __jo_current_sprite_attr, DEGtoANG(angle)); }

/** @brief Display a sprite in 3 dimensional space
 *  @param sprite_id Sprite Id
 *  @param x Horizontal position from the center of the screen
 *  @param y Vertical position from the center of the screen
 *  @param z Z index (depth))
 */
static  __jo_force_inline void	jo_sprite_draw3D(const int sprite_id, const int x, const int y, const int z) { __internal_jo_sprite_set_position3D(__jo_sprite_pos, x, y, z); __jo_current_sprite_attr = ((__jo_sprite_pic[sprite_id].cmode & 1) ? &__jo_sprite_attr : &__jo_sprite_attr_8bits); __jo_current_sprite_attr->texno = sprite_id; slDispSprite((FIXED *)__jo_sprite_pos, __jo_current_sprite_attr, 0); }

/** @brief Display a sprite in 3 dimensional space
 *  @param sprite_id Sprite Id returned by jo_sprite_add(), jo_sprite_add_tga() or jo_sprite_add_image_pack()
 *  @param x Horizontal position from the top left corner
 *  @param y Vertical position from the top left corner
 *  @param z Z index (depth)
 *  @param angle Angle (Z axe)
 *  @warning Slower than jo_sprite_draw3D_and_rotate(), but easier to use
 */
static  __jo_force_inline void	jo_sprite_draw3D_and_rotate2(const int sprite_id, const int x, const int y, const int z, const int angle) { jo_sprite_draw3D_and_rotate(sprite_id, x - JO_TV_WIDTH_2 + JO_DIV_BY_2(__jo_sprite_def[sprite_id].Hsize), y - JO_TV_HEIGHT_2 + JO_DIV_BY_2(__jo_sprite_def[sprite_id].Vsize), z, angle); }

/** @brief Display a sprite in 3 dimensional space
 *  @param sprite_id Sprite Id returned by jo_sprite_add(), jo_sprite_add_tga() or jo_sprite_add_image_pack()
 *  @param x Horizontal position from the top left corner
 *  @param y Vertical position from the top left corner
 *  @param z Z index (depth)
 *  @warning Slower than jo_sprite_draw3D_and_rotate(), but easier to use
 */
static  __jo_force_inline void	jo_sprite_draw3D2(const int sprite_id, const int x, const int y, const int z) { jo_sprite_draw3D(sprite_id, x - JO_TV_WIDTH_2 + JO_DIV_BY_2(__jo_sprite_def[sprite_id].Hsize), y - JO_TV_HEIGHT_2 + JO_DIV_BY_2(__jo_sprite_def[sprite_id].Vsize), z) ;}

/*
** Some helper to get pixel color in texture
*/
/** @brief Get a specific pixel color from image bytes
 *  @param data Bytes (raw image data)
 *  @param x Horizontal ccord in the image
 *  @param y Vertical ccord in the image
 *  @param image_width Image width (in pixels)
 */
static  __jo_force_inline jo_color        jo_sprite_get_pixel_color(const jo_color * const data, const unsigned int x, const unsigned int y, const unsigned int image_width) { return data[x + y * image_width]; }

/** @brief Get if a specific pixel is transparent (JO_COLOR_Transparent) in image bytes
 *  @param data Bytes (raw image data)
 *  @param x Horizontal ccord in the image
 *  @param y Vertical ccord in the image
 *  @return true if the pixel is transparent otherwise false
 *  @param image_width Image width (in pixels)
 */
static  __jo_force_inline bool        jo_sprite_is_pixel_transparent(const jo_color * const data, const unsigned int x, const unsigned int y, const unsigned int image_width) { return jo_sprite_get_pixel_color(data, x, y, image_width) == JO_COLOR_Transparent; }

/** @brief Get sprite width
 *  @param sprite_id Sprite Id returned by jo_sprite_add(), jo_sprite_add_tga() or jo_sprite_add_image_pack()
 *  @return Sprite width
 */
static  __jo_force_inline int         jo_sprite_get_width(const int sprite_id) { return (__jo_sprite_def[sprite_id].Hsize); }

/** @brief Get sprite height
 *  @param sprite_id Sprite Id returned by jo_sprite_add(), jo_sprite_add_tga() or jo_sprite_add_image_pack()
 *  @return Sprite height
 */
static  __jo_force_inline int         jo_sprite_get_height(const int sprite_id) { return (__jo_sprite_def[sprite_id].Vsize); }

/** @brief Get sprite count
 *  @return Count
 */
static  __jo_force_inline int         jo_sprite_count(void) { return (jo_get_last_sprite_id() + 1); }

/** @brief Get sprite memory usage
 *  @return Sprite memory usage percent
 *  @details jo_printf(0, 0, "Sprite memory usage: %d%%  ", jo_sprite_usage_percent());
 */
int                                   jo_sprite_usage_percent(void);

#endif /* !__JO_SPRITES_H__ */

/*
** END OF FILE
*/
