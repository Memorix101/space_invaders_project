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
/** @file core.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine core function prototypes
 *  @bug No known bugs.
 */

#ifndef __JO_CORE_H__
# define __JO_CORE_H__

#ifdef JO_DEBUG

/** @brief Error handling (Internal usage)
 *  @warning MC Hammer: don't touch this
 */
extern char		__jo_last_error[JO_PRINTF_BUF_SIZE];
void    __jo_core_error(char *message, const char *function);
# define jo_core_error(...)		do {sprintf(__jo_last_error, __VA_ARGS__); __jo_core_error(__jo_last_error, __FUNCTION__);} while(0)

#endif

/** @brief Jo Engine Version */
# define JO_ENGINE_VERSION  (6.0)

/** @brief Abstracted name of the entry point */
void			ss_main(void);
# define jo_main	ss_main

/*
** TYPEDEFS
*/
/** @brief Function prototype for jo_core_add_callback() */
typedef void	(*jo_event_callback)(void);

/** @brief Init the engine
 *  @param back_color Default background color (colors.h)
 *  @warning Must be the first function called in jo_main()
 */
void	jo_core_init(const jo_color back_color);

/** @brief Run the game
 *  @warning Must be the last function called in jo_main()
 */
void	jo_core_run(void);

/** @brief Stop the execution until someone press Start
 */
void    jo_core_suspend(void);

/** @brief Add a callback in the game loop
 *  @param callback Function name with no parameters and no return value
 *  @warning Must be called before jo_core_run()
 *  @return Event Id (usefull for jo_core_remove_callback())
 */
int     jo_core_add_callback(const jo_event_callback callback);

/** @brief Remove a callback in the game loop
 *  @param event_id Value returned by jo_core_add_callback()
 */
void    jo_core_remove_callback(const int event_id);

/** @brief Return to the boot menu
 */
void    jo_goto_boot_menu(void);

#ifdef JO_DEBUG

/** @brief Get last error
 *  @return Formated error
 */
static  __jo_force_inline char * jo_get_last_error(void) { return (__jo_last_error); }

#endif

#endif /* !__JO_CORE_H__ */

/*
** END OF FILE
*/
