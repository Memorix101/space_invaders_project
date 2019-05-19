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
#include <sega_tim.h>
#include <stdbool.h>
#include "jo/conf.h"
#include "jo/types.h"
#include "jo/core.h"
#include "jo/tools.h"
#include "jo/math.h"

/*
** GLOBALS
*/
# ifdef JO_COMPILE_WITH_PRINTF_SUPPORT
char			__jo_sprintf_buf[JO_PRINTF_BUF_SIZE];
#endif /* !JO_COMPILE_WITH_PRINTF_SUPPORT */

void                    jo_getdate(jo_datetime *now)
{
    SmpcDateTime        *time;
#ifdef JO_DEBUG
   if (now == JO_NULL)
   {
	 	jo_core_error("now is null");
	 	return;
   }
#endif
	slGetStatus();
    time = &(Smpc_Status->rtc);
    now->year = (unsigned short)slDec2Hex(time->year);
    now->month = (unsigned char)(time->month & 0x0f);
    now->week = (unsigned char)(time->month >> 4);
    now->day = (unsigned char)slDec2Hex(time->date);
    now->hour = (unsigned char)slDec2Hex(time->hour);
    now->minute = (unsigned char)slDec2Hex(time->minute);
    now->second = (unsigned char)slDec2Hex(time->second);
}

unsigned int                jo_get_ticks(void)
{
    static unsigned int     ticks = 0;

    ticks += TIM_FRT_CNT_TO_MCR(TIM_FRT_GET_16()) / 1000;
	TIM_FRT_SET_16(0);
	return (ticks);
}

jo_language     jo_get_current_language(void)
{
    switch (slGetLanguage())
    {
        case SMPC_ITALIANO:
            return Italiano;
        case SMPC_DEUTSCH:
            return Deutsch;
        case SMPC_JAPAN:
            return Japanese;
        case SMPC_FRANCAIS:
            return French;
        case SMPC_ESPANOL:
            return Espanol;
        default:
            return English;
    }
}

void                    jo_clear_screen(void)
{
    register int        x;
    register int        y;

    for (JO_ZERO(y); y < 30; ++y)
        for (JO_ZERO(x); x < 40; ++x)
            slPrint(" ", slLocate(x, y));
}

/*
** This is a fast unsafe(doesn't check JO_NULL param, etc.) implementation of atoi
*/
inline int				jo_tools_atoi(const char * str)
{
    int					val;
    bool		        is_negative;

    JO_ZERO(val);
    is_negative = false;
#ifdef JO_DEBUG
   if (str == JO_NULL)
   {
	 	jo_core_error("str is null");
	 	return (0);
   }
#endif
	if (*str == '-')
	{
		++str;
		is_negative = true;
	}
 	while (*str)
        val = val * 10 + (*str++ - '0');
	if (is_negative)
		return (-val);
  return (val);
}

inline int				jo_strlen(const char * str)
{
    char                *begin;

#ifdef JO_DEBUG
   if (str == JO_NULL)
   {
	 	jo_core_error("str is null");
	 	return (0);
   }
#endif
    for (begin = (char *)str; *str; ++str)
        ;
    return (str - begin);
}

inline bool             jo_endwith(const char *str, const char *end)
{
    int                 str_len;
    int                 end_len;
    register int        i;

#ifdef JO_DEBUG
   if (str == JO_NULL)
   {
	 	jo_core_error("str is null");
	 	return (0);
   }
   if (end == JO_NULL)
   {
	 	jo_core_error("end is null");
	 	return (0);
   }
#endif
    str_len = jo_strlen(str);
    end_len = jo_strlen(end);
    if (str_len < end_len)
        return (false);
    str += (str_len - end_len);
    for (JO_ZERO(i); i < end_len; ++i)
    {
        if (str[i] != end[i])
            return (false);
    }
    return (true);
}

/*
** END OF FILE
*/
