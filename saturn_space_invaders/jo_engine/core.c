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
#include <sl_def.h>
#include <sega_tim.h>
#include <stdbool.h>
#include "jo/conf.h"
#include "jo/types.h"
#include "jo/core.h"
#include "jo/tools.h"
#include "jo/audio.h"
#include "jo/colors.h"
#include "jo/sprites.h"
#include "jo/input.h"
#include "jo/math.h"
#include "jo/malloc.h"
#include "jo/video.h"
#include "jo/list.h"
#include "jo/background.h"

/*
** INTERNAL MACROS
*/
# define NBG1_CEL_ADR				        ((void *)(VDP2_VRAM_B1 + 0x02000))
# define NBG1_MAP_ADR				        ((void *)(VDP2_VRAM_B1 + 0x12000))

extern TEXTURE			                    __jo_sprite_def[];
extern int                                  __jo_gouraud_shading_runtime_index;
extern unsigned int                         __jo_fs_background_job_count;

#ifdef JO_COMPILE_WITH_VIDEO_SUPPORT
void                                        jo_video_init(void);
#endif
#ifdef JO_COMPILE_WITH_AUDIO_SUPPORT
void                                        jo_audio_init(void);
#endif
#ifdef JO_COMPILE_WITH_3D_SUPPORT
void                                        jo_3d_init(void);
#endif
int						                    jo_fs_init(void);
void                                        jo_fs_do_background_jobs(void);
void                                        jo_add_memory_zone(unsigned char *ptr, const unsigned int size_in_bytes);
void                                        jo_sprite_init(void);

#ifdef JO_DEBUG
char                                        __jo_last_error[JO_PRINTF_BUF_SIZE];
#endif

static jo_list                             __callbacks;

static  __jo_force_inline void              jo_enable_extended_ram_cartridge_support(void)
{
    *((int *)0x257efffe) = 1;
}

static  __jo_force_inline void              jo_set_a_bus_register(void)
{
    *((int *)0x25fe0080) = 0x23301ff0;
    *((int *)0x25fe0033) = 0x00000013;
}

static void                 jo_init_memory(void)
{
    static unsigned char    global_memory[JO_GLOBAL_MEMORY_SIZE_FOR_MALLOC];

    jo_add_memory_zone(global_memory, sizeof(global_memory));
    if (jo_get_extended_ram_cartridge_type() == CART_8MBits)
    {
        jo_enable_extended_ram_cartridge_support();
        jo_set_a_bus_register();
        jo_add_memory_zone((unsigned char *)0x2247ffff, 0x180000);
        jo_add_memory_zone((unsigned char *)0x2267ffff, 0x180000);
    }
    else if (jo_get_extended_ram_cartridge_type() == CART_32MBits)
    {
        jo_enable_extended_ram_cartridge_support();
        jo_set_a_bus_register();
        jo_add_memory_zone((unsigned char *)0x2247ffff, 0x180000);
        jo_add_memory_zone((unsigned char *)0x2267ffff, 0x180000);
        jo_add_memory_zone((unsigned char *)0x2287ffff, 0x180000);
        jo_add_memory_zone((unsigned char *)0x22a7ffff, 0x180000);
        jo_add_memory_zone((unsigned char *)0x22c7ffff, 0x180000);
        jo_add_memory_zone((unsigned char *)0x22e7ffff, 0x180000);
        jo_add_memory_zone((unsigned char *)0x2307ffff, 0x180000);
    }
}

#ifdef JO_COMPILE_WITH_PRINTF_SUPPORT

static void     jo_set_vdp2_4k_default_color_palette(void)
{
    jo_set_printf_palette_color(JO_COLOR_INDEX_Red, JO_COLOR_Red);
    jo_set_printf_palette_color(JO_COLOR_INDEX_Green, JO_COLOR_Green);
    jo_set_printf_palette_color(JO_COLOR_INDEX_Yellow, JO_COLOR_Yellow);
    jo_set_printf_palette_color(JO_COLOR_INDEX_Blue, JO_COLOR_Blue);
    jo_set_printf_palette_color(JO_COLOR_INDEX_Purple, JO_COLOR_Purple);
}

#endif

static	void	jo_core_init_vdp2(const jo_color back_color)
{
	slBack1ColSet((void *)BACK_CRAM, back_color);
	slPriorityNbg0(7);
	slPriorityNbg1(6);
	slCharNbg0(COL_TYPE_256, CHAR_SIZE_1x1);
	slCharNbg1(COL_TYPE_256, CHAR_SIZE_1x1);
#ifdef JO_COMPILE_WITH_PRINTF_SUPPORT
	jo_set_vdp2_4k_default_color_palette();
    jo_set_printf_color_index(0);
#endif
	slPageNbg1(NBG1_CEL_ADR, 0, PNB_1WORD | CN_12BIT);
	slPlaneNbg1(PL_SIZE_1x1);
	slMapNbg1(NBG1_MAP_ADR, NBG1_MAP_ADR, NBG1_MAP_ADR, NBG1_MAP_ADR);
	slBitMapNbg1(COL_TYPE_32768, JO_VDP2_SIZE, (void *)VDP2_VRAM_A0);
	slZoomNbg1(JO_FIXED_1, JO_FIXED_1);
	slScrAutoDisp(NBG0ON | NBG1ON);
}

void			jo_core_init(const jo_color back_color)
{
#ifdef JO_DEBUG
	JO_ZERO(__jo_last_error[0]);
#endif
	slInitSystem(JO_TV_RES, __jo_sprite_def, JO_FRAMERATE);
    slTVOff();
	jo_list_init(&__callbacks);
	jo_core_init_vdp2(back_color);
	jo_init_memory();
#if defined(JO_COMPILE_WITH_VIDEO_SUPPORT)
    jo_video_init();
#endif
#if defined(JO_COMPILE_WITH_AUDIO_SUPPORT)
	jo_audio_init();
	jo_audio_set_volume(JO_DEFAULT_AUDIO_VOLUME);
#endif /* !JO_COMPILE_WITH_AUDIO_SUPPORT */
#ifdef JO_COMPILE_WITH_3D_SUPPORT
    jo_3d_init();
#endif
	slTVOn();
	if (jo_fs_init() == 0)
	{
#ifdef JO_DEBUG
        jo_core_error("Fail to initialize filesystem");
#endif
	}
    jo_sprite_init();
}

inline int			jo_core_add_callback(const jo_event_callback callback)
{
    jo_node         *node;

#ifdef JO_DEBUG
   if (callback == JO_NULL)
   {
	 	jo_core_error("callback is null");
	 	return (0);
   }
#endif
    node = jo_list_add_ptr(&__callbacks, callback);
    if (node == JO_NULL)
        return (0);
    return (int)node;
}

inline void    jo_core_remove_callback(const int event_id)
{
#ifdef JO_DEBUG
	if (event_id == 0)
	{
		jo_core_error("Invalid event_id (%d)", event_id);
		return;
	}
#endif
    jo_list_remove(&__callbacks, (jo_node *)event_id);
}

void            jo_goto_boot_menu(void)
{
   	SYS_Exit(0);
}

#ifdef JO_DEBUG

void            __jo_core_error(char *message, const char *function)
{
    int         usage;

    jo_clear_background(JO_COLOR_Blue);
    jo_set_printf_color_index(0);
    jo_printf(0, 13, "   >>> Jo Engine error handler <<<");
    usage = jo_memory_usage_percent();
    if (usage > 75)
        jo_printf_with_color(0, 17, JO_COLOR_INDEX_Red, "Dynamic memory usage: %d%%  ", usage);
    else if (usage > 33)
        jo_printf_with_color(0, 17, JO_COLOR_INDEX_Yellow, "Dynamic memory usage: %d%%  ", usage);
    else
        jo_printf_with_color(0, 17, JO_COLOR_INDEX_Green, "Dynamic memory usage: %d%%  ", usage);

    usage = jo_sprite_usage_percent();
    if (usage > 75)
        jo_printf_with_color(0, 18, JO_COLOR_INDEX_Red, "Sprite memory usage:  %d%%  ", usage);
    else if (usage > 33)
        jo_printf_with_color(0, 18, JO_COLOR_INDEX_Yellow, "Sprite memory usage:  %d%%  ", usage);
    else
        jo_printf_with_color(0, 18, JO_COLOR_INDEX_Green, "Sprite memory usage:  %d%%  ", usage);
    jo_printf_with_color(0, 21, JO_COLOR_INDEX_Red, "In %s():", function);
    jo_printf_with_color(0, 23, JO_COLOR_INDEX_Red, message);
    jo_set_printf_color_index(0);
    jo_printf(2, 27, "Press [START] to continue...");
    jo_core_suspend();
    jo_clear_screen();
}

#endif

void            jo_core_suspend(void)
{
    int         wait_cursor;
    int         frame;

    for (JO_ZERO(frame), JO_ZERO(wait_cursor);; ++frame)
    {
        if (frame > 4)
        {
            switch (wait_cursor)
            {
                case 0: jo_printf(0, 27, "-"); break;
                case 1: jo_printf(0, 27, "/"); break;
                case 2: jo_printf(0, 27, "I"); break;
            }
            if (wait_cursor == 2)
                JO_ZERO(wait_cursor);
            else
                ++wait_cursor;
            JO_ZERO(frame);
        }
        slSynch();
        if ((jo_is_pad1_available() && jo_is_pad1_key_down(JO_KEY_START)) || (jo_is_pad2_available() && jo_is_pad2_key_down(JO_KEY_START)))
        {
            jo_printf(0, 27, " ");
            return ;
        }
    }
}

static __jo_force_inline void __jo_call_event(jo_node *node)
{
    ((jo_event_callback)node->data.ptr)();
}

void			jo_core_run(void)
{
    TIM_FRT_INIT(TIM_CKS_32);
    TIM_FRT_SET_16(0);
	for (;;)
	{
	    if (__jo_fs_background_job_count > 0)
            jo_fs_do_background_jobs();
#ifdef JO_COMPILE_WITH_3D_SUPPORT
	    slUnitMatrix(CURRENT);
#endif /* !JO_COMPILE_WITH_3D_SUPPORT */
		if (jo_is_pad1_available() && jo_is_pad1_key_pressed(JO_KEY_A) && jo_is_pad1_key_pressed(JO_KEY_B) &&
			jo_is_pad1_key_pressed(JO_KEY_C) && jo_is_pad1_key_pressed(JO_KEY_START))
			break;
        jo_list_foreach(&__callbacks, __jo_call_event);
		slSynch();
		__jo_gouraud_shading_runtime_index = -1;
	}
    jo_goto_boot_menu();
}

/*
** END OF FILE
*/
