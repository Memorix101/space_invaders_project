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

/*
** INCLUDES
*/
#include <sgl.h>
#include <stdbool.h>
#include "jo/conf.h"
#include "jo/types.h"
#include "jo/core.h"
#include "jo/tools.h"
#include "jo/colors.h"
#include "jo/malloc.h"
#include "jo/fs.h"
#include "jo/image.h"
#include "jo/sprites.h"
#include "jo/math.h"
#include "jo/list.h"
#include "jo/3d.h"

/*
** MACROS
*/
# define VDP1_USER_AREA_SIZE        (0x71D38)
# define VDP1_USER_AREA_END_ADDR    (0x25C7FEF8)

/*
** GLOBALS
*/
SPR_ATTR				__jo_sprite_attr = SPR_ATTRIBUTE(0, No_Palet, No_Gouraud, CL32KRGB | ECdis, sprNoflip | FUNC_Sprite);
SPR_ATTR				__jo_sprite_attr_8bits = SPR_ATTRIBUTE(0, (Uint16)VDP2_COLRAM, No_Gouraud, CL256Bnk | ECdis, sprNoflip | FUNC_Sprite);
SPR_ATTR                *__jo_current_sprite_attr = &__jo_sprite_attr;
FIXED					__jo_sprite_pos[XYZS] = {JO_FIXED_0, JO_FIXED_0, JO_FIXED_120, JO_FIXED_1};
TEXTURE					__jo_sprite_def[JO_MAX_SPRITE];
PICTURE                 __jo_sprite_pic[JO_MAX_SPRITE];

int                     __jo_gouraud_shading_runtime_index = -1;
int				        __jo_hash_table[JO_MAX_SPRITE];
static int				__jo_sprite_addr = 0;
static int				__jo_sprite_id = -1;

int                     jo_sprite_usage_percent(void)
{
    int                 mem_free;

    if (!__jo_sprite_addr)
       return (0);
    mem_free = VDP1_USER_AREA_END_ADDR - (SpriteVRAM + AdjCG(JO_MULT_BY_8(__jo_sprite_def[__jo_sprite_id].CGadr),
                             __jo_sprite_def[__jo_sprite_id].Hsize,
                             __jo_sprite_def[__jo_sprite_id].Vsize,
                             __jo_sprite_pic[__jo_sprite_id].cmode));
    return (int)(100.0f / (float)VDP1_USER_AREA_SIZE * (float)(VDP1_USER_AREA_SIZE - mem_free));
}

int                     jo_get_last_sprite_id(void)
{
    return (__jo_sprite_id);
}

void                    jo_sprite_init(void)
{
	register int		i;

	__jo_sprite_id = -1;
    for (JO_ZERO(i); i < JO_MAX_SPRITE; ++i)
        JO_ZERO(__jo_hash_table[i]);
}

inline int				jo_sprite_name2id(const char * const filename)
{
	register int		i;
	int					hash;

#ifdef JO_DEBUG
   if (filename == JO_NULL)
   {
	 	jo_core_error("filename is null");
	 	return (-1);
   }
#endif
	hash = jo_4_char_hash(filename);
	for (JO_ZERO(i); i <= __jo_sprite_id; ++i)
		if (__jo_hash_table[i] == hash)
			return (i);
	return (-1);
}

inline int				jo_sprite_replace(const jo_img * const img, const int sprite_id)
{
    TEXTURE             *txptr;

#ifdef JO_DEBUG
   if (img == JO_NULL)
   {
	 	jo_core_error("img is null");
	 	return (sprite_id);
   }
   if (sprite_id > __jo_sprite_id)
   {
	 	jo_core_error("Invalid sprite_id");
		return (sprite_id);
   }
   if (__jo_sprite_def[sprite_id].Hsize != img->width)
   {
        jo_core_error("Width doesn't match (%d != %d)", (int)__jo_sprite_def[sprite_id].Hsize, (int)img->width);
	 	return (sprite_id);
   }
   if (__jo_sprite_def[sprite_id].Vsize != img->height)
   {
	 	jo_core_error("Height doesn't match (%d != %d)", (int)__jo_sprite_def[sprite_id].Vsize, (int)img->height);
	 	return (sprite_id);
   }
#endif
	__jo_sprite_pic[sprite_id].pcsrc = img->data;
    txptr = __jo_sprite_def + __jo_sprite_pic[sprite_id].texno;
	slDMACopy(__jo_sprite_pic[sprite_id].pcsrc, (void *)(SpriteVRAM + JO_MULT_BY_8(txptr->CGadr)),
           (Uint32)((JO_MULT_BY_4(txptr->Hsize * txptr->Vsize)) >> (__jo_sprite_pic[sprite_id].cmode)));
    __jo_sprite_pic[sprite_id].pcsrc = (void *)(SpriteVRAM + JO_MULT_BY_8(txptr->CGadr));
#ifdef JO_COMPILE_WITH_3D_SUPPORT
    if (__jo_sprite_quad[sprite_id] != JO_NULL)
        jo_3d_free_sprite_quad(sprite_id);
#endif // JO_COMPILE_WITH_3D_SUPPORT
	return (sprite_id);
}

int				__internal_jo_sprite_add(void *data, unsigned short width, unsigned short height, unsigned short cmode)
{
    TEXTURE     *txptr;

#ifdef JO_DEBUG
    if (data == JO_NULL)
    {
        jo_core_error("data is null");
        return (-1);
    }
    if ((__jo_sprite_id + 1) >= JO_MAX_SPRITE)
    {
        jo_core_error("Too many sprites");
        return (-1);
    }
#endif
    if (!__jo_sprite_addr)
        __jo_sprite_addr = CGADDRESS;
    else
        __jo_sprite_addr = AdjCG(JO_MULT_BY_8(__jo_sprite_def[__jo_sprite_id].CGadr),
                                 __jo_sprite_def[__jo_sprite_id].Hsize,
                                 __jo_sprite_def[__jo_sprite_id].Vsize,
                                 __jo_sprite_pic[__jo_sprite_id].cmode);
	++__jo_sprite_id;
	__jo_sprite_def[__jo_sprite_id].Hsize = width;
	__jo_sprite_def[__jo_sprite_id].Vsize = height;
    __jo_sprite_def[__jo_sprite_id].CGadr = JO_DIV_BY_8(__jo_sprite_addr);
	__jo_sprite_def[__jo_sprite_id].HVsize = JO_MULT_BY_32(width & 0x1f8) | height;
    __jo_sprite_pic[__jo_sprite_id].cmode = cmode;
	__jo_sprite_pic[__jo_sprite_id].texno = __jo_sprite_id;
	__jo_sprite_pic[__jo_sprite_id].pcsrc = data;
	txptr = __jo_sprite_def + __jo_sprite_pic[__jo_sprite_id].texno;
	slDMACopy(__jo_sprite_pic[__jo_sprite_id].pcsrc,
            (void *)(SpriteVRAM + JO_MULT_BY_8(txptr->CGadr)),
            (Uint32)((JO_MULT_BY_4(txptr->Hsize * txptr->Vsize)) >> (cmode)));
    __jo_sprite_pic[__jo_sprite_id].pcsrc = (void *)(SpriteVRAM + JO_MULT_BY_8(txptr->CGadr));
	return (__jo_sprite_id);
}

int				jo_sprite_add(const jo_img * const img)
{
#ifdef JO_DEBUG
   if (img == JO_NULL)
   {
	 	jo_core_error("img is null");
	 	return (-1);
   }
#endif
    return __internal_jo_sprite_add(img->data, img->width, img->height, COL_32K);
}

int				jo_sprite_add_8bits_image(const jo_img_8bits * const img)
{
#ifdef JO_DEBUG
   if (img == JO_NULL)
   {
	 	jo_core_error("img is null");
	 	return (-1);
   }
#endif
    return __internal_jo_sprite_add(img->data, img->width, img->height, COL_256);
}

void             jo_sprite_free_from(const int sprite_id)
{
#ifdef JO_COMPILE_WITH_3D_SUPPORT
    register int    i;
#endif // JO_COMPILE_WITH_3D_SUPPORT
    if (sprite_id > __jo_sprite_id)
        return ;
#ifdef JO_COMPILE_WITH_3D_SUPPORT
    for (i = sprite_id; i < __jo_sprite_id; ++i)
    {
        if (__jo_sprite_quad[i] != JO_NULL)
            jo_3d_free_sprite_quad(i);
    }
#endif // JO_COMPILE_WITH_3D_SUPPORT
    if (sprite_id == 0)
        JO_ZERO(__jo_sprite_addr);
    else
        __jo_sprite_addr = AdjCG(JO_MULT_BY_8(__jo_sprite_def[sprite_id].CGadr),
                         __jo_sprite_def[sprite_id].Hsize,
                         __jo_sprite_def[sprite_id].Vsize,
                         __jo_sprite_pic[sprite_id].cmode);
    __jo_sprite_id = sprite_id - 1;
}

inline void	    jo_set_gouraud_shading_colors(const jo_color topleft_color,
                                              const jo_color topright_color,
                                              const jo_color bottomright_color,
                                              const jo_color bottomleft_color)
{
    ++__jo_gouraud_shading_runtime_index;
    __jo_sprite_attr.gstb = 0xe000 + __jo_gouraud_shading_runtime_index;
    *((unsigned short *)(void *)(SpriteVRAM + 0x70000) + JO_MULT_BY_4(__jo_gouraud_shading_runtime_index)) = topleft_color;
    *((unsigned short *)(void *)(SpriteVRAM + 0x70000) + 1 + JO_MULT_BY_4(__jo_gouraud_shading_runtime_index)) = topright_color;
    *((unsigned short *)(void *)(SpriteVRAM + 0x70000) + 2 + JO_MULT_BY_4(__jo_gouraud_shading_runtime_index)) = bottomright_color;
    *((unsigned short *)(void *)(SpriteVRAM + 0x70000) + 3 + JO_MULT_BY_4(__jo_gouraud_shading_runtime_index)) = bottomleft_color;
}

inline void     jo_set_gouraud_shading_brightness(const unsigned char brightness)
{
    jo_color    color;

    color = JO_COLOR_RGB(brightness, brightness, brightness);
    jo_set_gouraud_shading_colors(color, color, color, color);
}

/*
** END OF FILE
*/
