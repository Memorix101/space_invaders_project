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
/** @file tools.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine Tools (printf, etc,)
 *  @bug No known bugs.
 */

#ifndef __JO_TOOLS_H__
# define __JO_TOOLS_H__

/*
** INCLUDES
*/
#include "math.h"

/** @brief Swap Unsigned short endian
  * @param value Little or Big endian unsigned short
  * @return Endian swapped unsigned short
  */
static  __jo_force_inline unsigned short        jo_swap_endian_ushort(unsigned short value) { return ((((value) >> 8) & 0xff) | (((value) & 0xff) << 8)); }

/** @brief Swap unsigned int endian
  * @param value Little or Big endian unsigned int
  * @return Endian swapped unsigned int
  */
static  __jo_force_inline unsigned int        jo_swap_endian_uint(unsigned int value) { return ((((value) & 0xff000000) >> 24) | (((value) & 0x00ff0000) >>  8) | (((value) & 0x0000ff00) <<  8) | (((value) & 0x000000ff) << 24)); }

/** @brief Swap int endian
  * @param value Little or Big endian int
  * @return Endian swapped int
  */
static  __jo_force_inline int        jo_swap_endian_int(int value) { return ((((value) & 0xff000000) >> 24) | (((value) & 0x00ff0000) >>  8) | (((value) & 0x0000ff00) <<  8) | (((value) & 0x000000ff) << 24)); }


/** @brief sprintf prototypes
 *  @param str String
 *  @param format Format string
 *  @param ... Arguments
 *  @return If successful, the total number of characters written is returned excluding the null-character appended at the end of the string, otherwise a negative number is returned in case of failure.
 */
int				sprintf(char* str, const char* format, ...);

# ifdef JO_COMPILE_WITH_PRINTF_SUPPORT

#ifdef JO_DEBUG

#define jo_printf_debug(X, Y, fmt, args...) \
        jo_printf(X, Y, "%s(%s)#%d : " fmt , \
                __FILE__, __FUNCTION__, __LINE__, ## args)

#endif

/** @brief Internal sprintf buffer
 *  @warning MC Hammer: don't touch this
 */
extern char		__jo_sprintf_buf[JO_PRINTF_BUF_SIZE];

/** @brief printf implementation
 *  @param X Horizontal position from top left screen corner
 *  @param Y Vertical position from top left screen corner
 *  @param ... Arguments
 */
# define jo_printf(X, Y, ...)		do {sprintf(__jo_sprintf_buf, __VA_ARGS__); slPrint(__jo_sprintf_buf, slLocate((X), (Y)));} while(0)

/** @brief printf implementation
 *  @param X Horizontal position from top left screen corner
 *  @param Y Vertical position from top left screen corner
 *  @param COLOR_INDEX Color palette index
 *  @param ... Arguments
 */
# define jo_printf_with_color(X, Y, COLOR_INDEX, ...)  do { jo_set_printf_color_index(COLOR_INDEX); jo_printf(X, Y, __VA_ARGS__); } while(0)

/** @brief Set foreground palette color for the specific index
  * @param index Palette index (0 and 1 are reserved)
  * @param color Color (Example: JO_COLOR_Red)
  */
static  __jo_force_inline void        jo_set_printf_palette_color(const unsigned char index, const unsigned short color) { *(((unsigned short *)VDP2_COLRAM) + 1 + JO_MULT_BY_256(index)) = color; }

/** @brief Set printf foreground color index
  * @param index palette color index
  */
static  __jo_force_inline void        jo_set_printf_color_index(const unsigned char index) { slCurColor(index); }

#endif /* !JO_COMPILE_WITH_PRINTF_SUPPORT */

/** @brief NULL implementation
 */
# define JO_NULL                            ((void *)0)

/** @brief Mark function argument as unused
 */
# define JO_UNUSED_ARG(ARG)                     ((void)ARG)

/** @brief Check if the character is a whitespace
 *  @param c variable of char type
 *  @return true if the character is a whitespace otherwise false
 */
static  __jo_force_inline bool        jo_tools_is_whitespace(const char c) { return c == '\n' || c == '\r' || c == '\t'  || c == ' '; }

/** @brief Create an hash code based on the first four character of the string
 *  @param str String
 *  @return the hashcode (integer)
 */
static  __jo_force_inline int        jo_4_char_hash(const char * const str) { return (int)str[0] | (((int)str[1]) << 8) | (((int)str[2]) << 16) | (((int)str[3]) << 24); }

/** @brief DateTime struct
 */
typedef struct
{
    unsigned char   day;
    unsigned char   month;
    unsigned short  year;
    unsigned char   hour;
    unsigned char   week;
    unsigned char   minute;
    unsigned char   second;
}                   jo_datetime;

/** @brief Language type
 */
typedef enum
{
    English = 0,
    Deutsch = 1,
    French = 2,
    Espanol = 3,
    Italiano = 4,
    Japanese = 5
}                   jo_language;

/** @brief Horizontal move
 */
typedef enum
{
    None,
    Left,
    Right
}                   jo_horizontal_move;

/** @brief Get Sega saturn current language
 *  @return The language
 */
jo_language     jo_get_current_language(void);

/** @brief Clear screen (usefull for printf)
 *  @warning Try to not use it by adding some whitespace at the end of jo_printf
 */
void            jo_clear_screen(void);

/** @brief get current date and time
 *  @param now result
 */
void            jo_getdate(jo_datetime *now);

/** @brief get ticks count
 *  @return ticks count from jo_core_run()
 */
unsigned int    jo_get_ticks(void);

/** @brief atoi implementation
 *  @param str This is the string representation of an integral number
 *  @return This function returns the converted integral number as an int value. If no valid conversion could be performed, it returns zero
 */
int			    jo_tools_atoi(const char * str);

/** @brief strlen implementation
 *  @param str This is the string
 *  @return This function returns the length of str
 */
int				jo_strlen(const char * str);

/** @brief determine if str end with a specific string
 *  @param str This is the string
 *  @param end The ending string
 *  @return This function returns true if str end with "end" param otherwise false
 */
bool             jo_endwith(const char *str, const char *end);

#endif /* !__JO_TOOLS_H__ */

/*
** END OF FILE
*/
