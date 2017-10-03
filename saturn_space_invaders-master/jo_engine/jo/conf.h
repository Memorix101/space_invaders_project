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
/** @file conf.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine configuration
 *  @bug No known bugs.
 *  @todo 480 mode (Hi res)
 */

#ifndef __JO_CONF_H__
# define __JO_CONF_H__

/*
** OPTIONS (comment lines to disable features)
*/
/** @brief Comment this line to disable video support */
# define JO_COMPILE_WITH_VIDEO_SUPPORT
/** @brief Comment this line to disable backup support */
# define JO_COMPILE_WITH_BACKUP_SUPPORT
/** @brief Comment this line to disable jo_printf() support */
# define JO_COMPILE_WITH_PRINTF_SUPPORT
/** @brief Comment this line to disable TGA file format support */
# define JO_COMPILE_WITH_TGA_SUPPORT
/** @brief Comment this line to disable audio feature */
# define JO_COMPILE_WITH_AUDIO_SUPPORT
/** @brief Comment this line to disable 3D feature */
# define JO_COMPILE_WITH_3D_SUPPORT
/** @brief Comment this line to disable pseudo mode7 support */
# define JO_COMPILE_WITH_PSEUDO_MODE7_SUPPORT
/** @brief Comment this line to disable advanced 2D/3D effects */
# define JO_COMPILE_WITH_EFFECTS_SUPPORT

/** @brief Comment this line to switch to PAL mode (screen resolution) */
# define JO_NTSC_VERSION
/** Set HD resolution mode. /!\ Under development don't use it */
//# define JO_480p
/** @brief Comment this line to disable internal debugging : jo_get_last_error()
 *  @warning Reduce performance
 */
# define JO_DEBUG

/*
** PARAMS
*/

# if defined(JO_NTSC_VERSION)
 # if defined(JO_480p)
    /** @brief Sega Saturn NTSC Screen resolution (internal use) */
    # define JO_TV_RES							(TV_704x480)
    /** @brief NTSC Screen width */
    # define JO_TV_WIDTH						(704)
    /** @brief NTSC Screen width divided by 2 (math optimization purpose) */
    # define JO_TV_WIDTH_2						(352)
    /** @brief NTSC Screen width divided by 4 (math optimization purpose) */
    # define JO_TV_WIDTH_4						(176)
    /** @brief NTSC Screen height */
    # define JO_TV_HEIGHT						(480)
    /** @brief NTSC Screen height divided by 2 (math optimization purpose) */
    # define JO_TV_HEIGHT_2						(240)
    /** @brief NTSC Screen height divided by 4 (math optimization purpose) */
    # define JO_TV_HEIGHT_4						(120)
 #else
    /** @brief Sega Saturn NTSC Screen resolution (internal use) */
    # define JO_TV_RES							(TV_320x240)
    /** @brief NTSC Screen width */
    # define JO_TV_WIDTH						(320)
    /** @brief NTSC Screen width divided by 2 (math optimization purpose) */
    # define JO_TV_WIDTH_2						(160)
    /** @brief NTSC Screen width divided by 4 (math optimization purpose) */
    # define JO_TV_WIDTH_4						(80)
    /** @brief NTSC Screen height */
    # define JO_TV_HEIGHT						(240)
    /** @brief NTSC Screen height divided by 2 (math optimization purpose) */
    # define JO_TV_HEIGHT_2						(120)
    /** @brief NTSC Screen height divided by 4 (math optimization purpose) */
    # define JO_TV_HEIGHT_4						(60)
#endif

#else /* PAL Version */

/** @brief Sega Saturn PAL Screen resolution (internal use) */
# define JO_TV_RES							(TV_320x256)
/** @brief PAL Screen width */
# define JO_TV_WIDTH						(320)
/** @brief PAL Screen width divided by 2 (math optimization purpose) */
# define JO_TV_WIDTH_2						(160)
/** @brief PAL Screen width divided by 4 (math optimization purpose) */
# define JO_TV_WIDTH_4						(80)
/** @brief PAL Screen height */
# define JO_TV_HEIGHT						(256)
/** @brief PAL Screen height divided by 2 (math optimization purpose) */
# define JO_TV_HEIGHT_2						(128)
/** @brief PAL Screen height divided by 4 (math optimization purpose) */
# define JO_TV_HEIGHT_4						(64)

#endif

# if defined (JO_480p)
    /** @brief VDP2 Background bitmap size */
    # define JO_VDP2_SIZE                       (BM_1024x512)
    /** @brief VDP2 Background bitmap width */
    # define JO_VDP2_WIDTH                      (1024)
    /** @brief VDP2 Background bitmap height */
    # define JO_VDP2_HEIGHT                     (512)
#else
    /** @brief VDP2 Background bitmap size */
    # define JO_VDP2_SIZE                       (BM_512x256)
    /** @brief VDP2 Background bitmap width */
    # define JO_VDP2_WIDTH                      (512)
    /** @brief VDP2 Background bitmap height */
    # define JO_VDP2_HEIGHT                     (256)
#endif

/** @brief Real frame rate = (60 / JO_FRAMERATE) */
# define JO_FRAMERATE						(1)
/** @brief Max different sprite available in game at the same time */
# define JO_MAX_SPRITE						(255)
# if defined (JO_COMPILE_WITH_PRINTF_SUPPORT)
/** @brief Max character in available in jo_printf() */
#  define JO_PRINTF_BUF_SIZE				(64)
# endif /* !JO_COMPILE_WITH_PRINTF_SUPPORT */

/** @brief Available global memory for jo_malloc() in bytes
  * @details Reduce this value if you use many embedded image (hardcoded image)
  */
# define JO_GLOBAL_MEMORY_SIZE_FOR_MALLOC   (512 * 1024)

/** @brief Max filename length (DOS) <=> GFS_FNAME_LEN + 1 */
# define JO_MAX_FILENAME_LENGTH             (13)
/** @brief Max file available in the entire file system (on the CD of course) */
# define JO_FS_MAX_FILES					(256)
/** @brief Max background file reading */
# define JO_FS_MAX_BACKGROUND_JOBS          (4)
/** @brief Max file in image pack for jo_sprite_add_image_pack() */
# define JO_MAX_FILE_IN_IMAGE_PACK			(32)
/** @brief Max map layer on the screen (useful for parallax scrolling) */
# define JO_MAP_MAX_LAYER					(8)
/** @brief Max sprite animation on the screen */
# define JO_MAX_SPRITE_ANIM					(16)

/** @brief force inline attribute (and prevent Doxygen prototype parsing bug) */
# define __jo_force_inline                  __attribute__((always_inline)) inline

#if JO_FRAMERATE < 1
# error "JO_FRAMERATE must be greater than zero"
#endif

#endif /* !__JO_CONF_H__ */

/*
** END OF FILE
*/
