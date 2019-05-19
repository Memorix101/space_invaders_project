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
#include "jo/math.h"
#include "jo/fs.h"
#include "jo/tools.h"
#include "jo/malloc.h"

/** @brief Max file opened at the same time
 *  @warning NEVER CHANGE THIS VALUE
 */
# define JO_OPEN_MAX						(1)

/** @brief Maximum sector fetched once (for jo_fs_read_file_async())
 */
# define JO_MAXIMUM_SECTOR_FETCHED_ONCE     (1)

/** @brief Maximum sector fetched once (for jo_fs_read_file_async())
 */
# define JO_SECTOR_SIZE                     (2048)

/** @brief Size of each read (for jo_fs_do_background_jobs())
 */
# define JO_READ_SIZE                       (JO_MAXIMUM_SECTOR_FETCHED_ONCE * JO_SECTOR_SIZE)

typedef struct
{
    bool                        active;
    jo_fs_async_read_callback   callback;
    GfsHn                       gfs;
    int                         file_length;
    char                        *contents;
    char                        *ptr;
    int                         token;
}                               __jo_fs_background_job;

/*
** GLOBALS
*/
static char                     __jo_fs_work[GFS_WORK_SIZE(JO_OPEN_MAX)];
static GfsDirTbl				__jo_fs_dirtbl;
static GfsDirName				__jo_fs_dirname[JO_FS_MAX_FILES];
static __jo_fs_background_job   __jo_fs_background_jobs[JO_FS_MAX_BACKGROUND_JOBS];
unsigned int                    __jo_fs_background_job_count;

int								jo_fs_init()
{
    register int                i;

    JO_ZERO(__jo_fs_background_job_count);
    for (JO_ZERO(i); i < JO_FS_MAX_BACKGROUND_JOBS; ++i)
        __jo_fs_background_jobs[i].active = false;
	GFS_DIRTBL_TYPE(&__jo_fs_dirtbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&__jo_fs_dirtbl) = __jo_fs_dirname;
	GFS_DIRTBL_NDIR(&__jo_fs_dirtbl) = JO_FS_MAX_FILES;
	if (GFS_Init(JO_OPEN_MAX, __jo_fs_work, &__jo_fs_dirtbl) <= 2)
	{
#ifdef JO_DEBUG
	jo_core_error("GFS_Init() failed");
#endif
		return (0);
	}
	return (1);
}

void                    jo_fs_do_background_jobs(void)
{
    register int        i;
    Sint32              stat;
    Sint32              nbyte;

    for (JO_ZERO(i); i < JO_FS_MAX_BACKGROUND_JOBS; ++i)
    {
        if (!__jo_fs_background_jobs[i].active)
            continue;
        GFS_NwFread(__jo_fs_background_jobs[i].gfs, JO_MAXIMUM_SECTOR_FETCHED_ONCE, __jo_fs_background_jobs[i].ptr, JO_READ_SIZE);
        do
        {
            GFS_NwExecOne(__jo_fs_background_jobs[i].gfs);
            GFS_NwGetStat(__jo_fs_background_jobs[i].gfs, &stat, &nbyte);
        } while (nbyte < JO_READ_SIZE && stat != GFS_SVR_COMPLETED);
        if (stat == GFS_SVR_COMPLETED)
        {
            GFS_Close(__jo_fs_background_jobs[i].gfs);
            --__jo_fs_background_job_count;
            JO_ZERO(__jo_fs_background_jobs[i].contents[__jo_fs_background_jobs[i].file_length]);
            __jo_fs_background_jobs[i].active = false;
            __jo_fs_background_jobs[i].callback(__jo_fs_background_jobs[i].contents, __jo_fs_background_jobs[i].file_length, __jo_fs_background_jobs[i].token);
        }
        else
            __jo_fs_background_jobs[i].ptr += nbyte;
    }
}

int                     jo_fs_read_file_async(const char *const filename, jo_fs_async_read_callback callback, int optional_token)
{
    register int        i;
    int			        fid;
    Sint32			    sctsize;
	Sint32              nsct;
	Sint32              lastsize;

#ifdef JO_DEBUG
    if (callback == JO_NULL)
    {
        jo_core_error("callback is null");
        return 0;
    }
#endif
    for (JO_ZERO(i); i < JO_FS_MAX_BACKGROUND_JOBS; ++i)
    {
        if (__jo_fs_background_jobs[i].active)
            continue;
        fid = GFS_NameToId((Sint8 *)filename);
        if (fid < 0)
        {
#ifdef JO_DEBUG
            jo_core_error("%s: File not found", filename);
#endif
            return (0);
        }
        if ((__jo_fs_background_jobs[i].gfs = GFS_Open(fid)) == JO_NULL)
        {
#ifdef JO_DEBUG
            jo_core_error("%s: GFS_Open() failed", filename);
#endif
            return (0);
        }
        GFS_GetFileSize(__jo_fs_background_jobs[i].gfs, &sctsize, &nsct, &lastsize);
        __jo_fs_background_jobs[i].file_length = (sctsize * (nsct - 1) + lastsize);
        if ((__jo_fs_background_jobs[i].contents = jo_malloc(sctsize * nsct + 1)) == JO_NULL)
        {
#ifdef JO_DEBUG
            jo_core_error("%s: Out of memory", filename);
#endif
            GFS_Close(__jo_fs_background_jobs[i].gfs);
            return (0);
        }
        __jo_fs_background_jobs[i].token = optional_token;
        __jo_fs_background_jobs[i].callback = callback;
        __jo_fs_background_jobs[i].ptr = __jo_fs_background_jobs[i].contents;
        GFS_NwCdRead(__jo_fs_background_jobs[i].gfs, sctsize * nsct);
        GFS_SetTransPara(__jo_fs_background_jobs[i].gfs, JO_MAXIMUM_SECTOR_FETCHED_ONCE);
        __jo_fs_background_jobs[i].active = true;
        ++__jo_fs_background_job_count;
        return (1);
    }
    jo_core_error("%s: Too many background jobs", filename);
    return (0);
}

void			        jo_fs_cd(const char *const sub_dir)
{
	Sint32			    fid;

#ifdef JO_DEBUG
   if (sub_dir == JO_NULL)
   {
	 	jo_core_error("sub_dir is null");
	 	return ;
   }
#endif
	fid = GFS_NameToId((Sint8 *)sub_dir);
	GFS_DIRTBL_TYPE(&__jo_fs_dirtbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&__jo_fs_dirtbl) = __jo_fs_dirname;
	GFS_DIRTBL_NDIR(&__jo_fs_dirtbl) = JO_FS_MAX_FILES;
	GFS_LoadDir(fid, &__jo_fs_dirtbl);
	GFS_SetDir(&__jo_fs_dirtbl);
}

static int		        jo_fs_get_file_size(int file_id)
{
	GfsHn			    gfs;
	Sint32			    sctsize;
	Sint32              nsct;
	Sint32              lastsize;

	if ((gfs = GFS_Open(file_id)) == JO_NULL)
	{
#ifdef JO_DEBUG
        jo_core_error("GFS_Open() failed");
#endif
		return (-1);
	}
	GFS_GetFileSize(gfs, &sctsize, &nsct, &lastsize);
	GFS_Close(gfs);
	return (sctsize * (nsct - 1) + lastsize);
}

char*					jo_fs_read_file(const char *const filename, int *len)
{
	int			        fid;
	int			        fsize;
	char                *stream;

#ifdef JO_DEBUG
   if (filename == JO_NULL)
   {
	 	jo_core_error("filename is null");
	 	return (0);
   }
#endif
	fid = GFS_NameToId((Sint8 *)filename);
	if (fid < 0)
	{
#ifdef JO_DEBUG
        jo_core_error("%s: File not found", filename);
#endif
		return (JO_NULL);
	}
	fsize = jo_fs_get_file_size(fid);
	if (fsize < 0)
		return (JO_NULL);
    if ((stream = (char *)jo_malloc_with_behaviour((fsize + 1) * sizeof(*stream), JO_MALLOC_TRY_REUSE_BLOCK)) == JO_NULL)
    {
#ifdef JO_DEBUG
        jo_core_error("%s: Out of memory", filename);
#endif
        return (JO_NULL);
    }
	GFS_Load(fid, 0, stream, fsize);
	JO_ZERO(stream[fsize]);
	if (len != JO_NULL)
		*len = fsize;
	return stream;
}

char*					jo_fs_read_file_in_dir(const char *const filename, const char *const sub_dir, int *len)
{
    char                *stream;

	if (sub_dir != JO_NULL)
		jo_fs_cd(sub_dir);
    stream = jo_fs_read_file(filename, len);
	if (sub_dir != JO_NULL)
		jo_fs_cd(JO_PARENT_DIR);
	return stream;
}

/*
** END OF FILE
*/
