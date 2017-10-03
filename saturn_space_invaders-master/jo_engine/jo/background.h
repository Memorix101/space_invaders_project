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
/** @file background.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine Background definition and tools
 *  @bug No known bugs.
 */

#ifndef __JO_BACKGROUND_H__
# define __JO_BACKGROUND_H__

/*
** Background (VDP2)
*/

/** @brief Draw a line using Bresenham's line algorithm
 *  @param x0 horizontal location of the beginning of the line
 *  @param y0 vertical location of the beginning of the line
 *  @param x1 horizontal location of the end of the line
 *  @param y1 vertical location of the end of the line
 *  @param color Color (ex: JO_COLOR_Red)
 */
void        jo_draw_background_line(int x0, int y0, int x1, int y1, const jo_color color);

/** @brief Put pixel in background using color
 *  @param x horizontal location
 *  @param y vertical location
 *  @param color Color (ex: JO_COLOR_Red)
 */
static  __jo_force_inline void	jo_put_pixel_in_background(const int x, const int y, const jo_color color) { *(((unsigned short *)VDP2_VRAM_A0) + x + y * JO_VDP2_WIDTH) = color; }

/** @brief Put pixel in background using composite color
 *  @param x horizontal location
 *  @param y vertical location
 *  @param r Red color component
 *  @param g Green color component
 *  @param b Blue color component
 */
static  __jo_force_inline void	jo_put_pixel_in_background_rgb(const int x, const int y, unsigned char r, unsigned char g, unsigned char b) { jo_put_pixel_in_background(x, y, C_RGB(r, g, b)); }

/** @brief Move background (scrolling)
 *  @param x horizontal location
 *  @param y vertical location
 */
static  __jo_force_inline void	jo_move_background(const int x, const int y) { slScrPosNbg1(JO_MULT_BY_65536(x), JO_MULT_BY_65536(y)); }

/** @brief Zoom background width and height independently
 *  @param width_factor Width zoom factor
 *  @param height_factor Height zoom factor
 */
static  __jo_force_inline void	jo_zoom_background2(const float width_factor, const float height_factor) { slZoomNbg1(toFIXED(width_factor), toFIXED(height_factor)); }

/** @brief Zoom background
 *  @param factor Zoom factor
 */
static  __jo_force_inline void	jo_zoom_background(const float factor) { jo_zoom_background2(factor, factor); }

/** @brief Add background sprite
 *  @param img Pointer to an image struct
 *  @param left Left location
 *  @param top Top location
 */
void	jo_set_background_sprite(const jo_img * const img, const unsigned short left, const unsigned short top);

/** @brief Clear the background
 *  @param color Clear color
 */
void    jo_clear_background(const jo_color color);

/** @brief Draw a square on the background
 *  @param x Square horizontal location
 *  @param y Square vertical location
 *  @param width Square width
 *  @param height Square height
 *  @param color Color (ex: JO_COLOR_Red)
 */
static  __jo_force_inline void	jo_draw_background_square(const int x, const int y, const short width, const short height, const jo_color color)
{
    jo_draw_background_line(x, y, x + width, y, color);
    jo_draw_background_line(x + width, y + height, x + width, y, color);
    jo_draw_background_line(x, y + height, x + width, y + height, color);
    jo_draw_background_line(x, y + height, x, y, color);
}

#endif /* !__JO_BACKGROUND_H__ */

/*
** END OF FILE
*/
