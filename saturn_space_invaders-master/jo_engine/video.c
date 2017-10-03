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
#include <sl_def.h>
#include <SEGA_GFS.H>
#include <stdbool.h>
#include "jo/conf.h"
#include "jo/types.h"
#include "jo/core.h"
#include "jo/tools.h"
#include "jo/video.h"

#ifdef JO_COMPILE_WITH_VIDEO_SUPPORT

/** @brief Header
 */
typedef struct
{
    char                signature[4]; /* FILM */
    int                 total_length;
    char                version[4];
    int                 reserved;
}                       jo_cpk_film_header;

/** @brief FDSC chunk (contains file description information)
 */
typedef struct
{
    char                signature[4]; /* FDSC */
    int                 fdsc_chunk_length;
    char                fourcc_video_codec[4]; /* CVID */
    int                 video_frame_height;
    int                 video_frame_width;
    char                bits_per_pixel; /* Always 24 */
    char                number_of_audio_channels;
    char                audio_sampling_resolution_in_bits; /* 8 or 16 */
    char                audio_compression;
    short               audio_sampling_frequency_in_hz;
    int                 reserved;
}                       jo_cpk_film_fdsc;

/** @brief STAB chunk (contains a table of media sample information)
 */
typedef struct
{
    char                signature[4]; /* STAB */
    int                 stab_chunk_length;
    int                 framerate_base_frequency_in_hz;
    int                 number_of_entries_in_sample_table;
}                       jo_cpk_film_stab;

void		            jo_video_init(void)
{
}

/*static void             jo_video_decode(GfsHn *gfs, jo_cpk_film_fdsc *fdsc, jo_cpk_film_stab *stab)
{
    JO_UNUSED_ARG(gfs);
    JO_UNUSED_ARG(fdsc);
    JO_UNUSED_ARG(stab);

}

void					jo_video_open_file(const char *const filename)
{
    jo_cpk_film_header  header;
    jo_cpk_film_fdsc    fdsc;
    jo_cpk_film_stab    stab;
    GfsHn			    gfs;
    int			        fid;
	Sint32			    sctsize;
	Sint32              nsct;
	Sint32              lastsize;

#ifdef JO_DEBUG
   if (filename == JO_NULL)
   {
	 	jo_core_error("JO_NULL filename in jo_fs_read_file()");
	 	return ;
   }
#endif
	fid = GFS_NameToId((Sint8 *)filename);
	if (fid < 0)
	{
#ifdef JO_DEBUG
		jo_core_error("GFS_NameToId() failed");
#endif
		return ;
	}
    if ((gfs = GFS_Open(fid)) == JO_NULL)
	{
#ifdef JO_DEBUG
        jo_core_error("GFS_Open() failed");
#endif
		return ;
	}
    // TODO READ ASYNC
    if (header.signature[0] != 'F' || header.signature[1] != 'I' || header.signature[2] != 'L' || header.signature[3] != 'M')
    {
#ifdef JO_DEBUG
	 	jo_core_error("BAD HEADER");
#endif
	 	return ;
    }
    // TODO READ ASYNC
    if (fdsc.signature[0] != 'F' || fdsc.signature[1] != 'D' || fdsc.signature[2] != 'S' || fdsc.signature[3] != 'C')
    {
#ifdef JO_DEBUG
	 	jo_core_error("BAD FDSC");
#endif
	 	return ;
    }
    // TODO READ ASYNC
    if (stab.signature[0] != 'S' || stab.signature[1] != 'T' || stab.signature[2] != 'A' || stab.signature[3] != 'B')
    {
#ifdef JO_DEBUG
	 	jo_core_error("BAD STAB");
#endif
	 	return ;
    }
    jo_video_decode(&gfs, &fdsc, &stab);
	GFS_Close(gfs);
}
*/

#endif /* !JO_COMPILE_WITH_VIDEO_SUPPORT */

/*
** END OF FILE
*/
