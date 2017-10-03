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
#include <sega_bup.h>
#include <stdbool.h>
#include "jo/conf.h"
#include "jo/types.h"
#include "jo/core.h"
#include "jo/math.h"
#include "jo/tools.h"
#include "jo/malloc.h"
#include "jo/backup.h"

# ifdef JO_COMPILE_WITH_BACKUP_SUPPORT

/*
** INTERNAL MACROS
*/
# define JO_BACKUP_LIB_SPACE_SIZE               (16384)
# define JO_BACKUP_WORK_SPACE_SIZE              (2048)
# define JO_BACKUP_OVERRIDE_FILE_IF_EXISTS      (OFF)

/** @brief Internal backup device struct */
typedef struct
{
        BupStat             sttb;
        Sint32              status;
        bool                is_mounted;
}                           __jo_backup_device;

/*
** GLOBALS
*/
static __jo_backup_device   __jo_backup_devices[3];
static bool                 __jo_backup_initialized = false;
static Uint32               *__jo_backup_lib_space;
static Uint32               *__jo_backup_work_space;

/*
** PRIVATE FUNCTIONS
*/

static void                 jo_backup_init(void)
{
    static BupConfig        cntb[3];

    if ((__jo_backup_lib_space = (Uint32 *)jo_malloc_with_behaviour(JO_BACKUP_LIB_SPACE_SIZE, JO_FAST_ALLOCATION)) == JO_NULL)
	{
#ifdef JO_DEBUG
        jo_core_error("Out of memory #1");
#endif
		return;
	}
	if ((__jo_backup_work_space = (Uint32 *)jo_malloc_with_behaviour(JO_BACKUP_WORK_SPACE_SIZE, JO_FAST_ALLOCATION)) == JO_NULL)
	{
#ifdef JO_DEBUG
        jo_core_error("Out of memory #2");
#endif
		return;
	}
    slResetDisable();
	BUP_Init(__jo_backup_lib_space, __jo_backup_work_space, cntb);
	slResetEnable();
	__jo_backup_devices[JoInternalMemoryBackup].is_mounted = false;
	__jo_backup_devices[JoCartridgeMemoryBackup].is_mounted = false;
	__jo_backup_devices[JoExternalDeviceBackup].is_mounted = false;
	__jo_backup_initialized = true;
}

/*
** PUBLIC FUNCTIONS
*/

int                         jo_backup_get_free_block_count(const jo_backup_device backup_device)
{
    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (-1);
    }
    return (__jo_backup_devices[backup_device].sttb.freeblock);
}

bool                        jo_backup_format_device(const jo_backup_device backup_device)
{
    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (false);
    }
    slResetDisable();
	__jo_backup_devices[backup_device].status = BUP_Format(backup_device);
	slResetEnable();
    BUP_Stat(backup_device, 10, &__jo_backup_devices[backup_device].sttb);
	return (__jo_backup_devices[backup_device].status == 0);
}

bool                        jo_backup_mount(const jo_backup_device backup_device)
{
    if (!__jo_backup_initialized)
        jo_backup_init();
    if ((__jo_backup_devices[backup_device].status = BUP_Stat(backup_device, 10, &__jo_backup_devices[backup_device].sttb)) == BUP_UNFORMAT)
        jo_backup_format_device(backup_device);
    __jo_backup_devices[backup_device].is_mounted = (__jo_backup_devices[backup_device].status == 0);
    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Fail to mount device");
#endif
        return (false);
    }
    return (__jo_backup_devices[backup_device].is_mounted);
}

bool                jo_backup_file_exists(const jo_backup_device backup_device, const char * const fname)
{
    BupDir          dir;

    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (false);
    }
    __jo_backup_devices[backup_device].status = BUP_Dir(backup_device, (Uint8 *)fname, 1, &dir);
    return (__jo_backup_devices[backup_device].status == 1);
}

bool                jo_backup_save_file_contents(const jo_backup_device backup_device, const char * const fname, const char * const comment, void *contents, unsigned short content_size)
{
    BupDate         date;
    BupDir          dir;
    jo_datetime     now;
    register int    len;
    register int    i;

    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (false);
    }
    if (comment == JO_NULL)
    {
#ifdef JO_DEBUG
        jo_core_error("comment is null");
#endif
        return (false);
    }
    if (contents == JO_NULL)
    {
#ifdef JO_DEBUG
        jo_core_error("contents is null");
#endif
        return (false);
    }
    if (fname == JO_NULL)
    {
#ifdef JO_DEBUG
        jo_core_error("fname is null");
#endif
        return (false);
    }
    len = jo_strlen(fname);
    if (len > 11)
    {
#ifdef JO_DEBUG
        jo_core_error("fname too long (%d) (max 11)", len);
#endif
        return (false);
    }
    for (JO_ZERO(i); i < len; ++i)
        dir.filename[i] = (Uint8)fname[i];
    JO_ZERO(dir.filename[i]);
    len = jo_strlen(comment);
    if (len > 10)
    {
#ifdef JO_DEBUG
        jo_core_error("comment too long (%d) (max 10)", len);
#endif
        return (false);
    }
    for (JO_ZERO(i); i < len; ++i)
        dir.comment[i] = (Uint8)comment[i];
    JO_ZERO(dir.comment[i]);
    /* LANGUAGE */
    switch (jo_get_current_language())
    {
        case Espanol:
            dir.language = BUP_ESPANOL;
            break;
        case Japanese:
            dir.language = BUP_JAPANESE;
            break;
        case Italiano:
            dir.language = BUP_ITALIANO;
            break;
        case Deutsch:
            dir.language = BUP_DEUTSCH;
            break;
        case French:
            dir.language = BUP_FRANCAIS;
            break;
        default:
            dir.language = BUP_ENGLISH;
            break;
    }
    /* DATE */
    jo_getdate(&now);
    date.year = now.year - 1980;
    date.month = now.month;
    date.week = now.week;
    date.day = now.day;
    date.time = now.hour;
    date.min = now.minute;
    dir.date = BUP_SetDate(&date);
    dir.datasize = content_size;
	slResetDisable();
	__jo_backup_devices[backup_device].status = BUP_Write(backup_device, &dir, contents, JO_BACKUP_OVERRIDE_FILE_IF_EXISTS);
    if (__jo_backup_devices[backup_device].status == 0)
        __jo_backup_devices[backup_device].status = BUP_Verify(backup_device, dir.filename, contents);
	slResetEnable();
	BUP_Stat(backup_device, 10, &__jo_backup_devices[backup_device].sttb);
    return (__jo_backup_devices[backup_device].status == 0);
}

void                    *jo_backup_load_file_contents(const jo_backup_device backup_device, const char * const fname, unsigned int *length)
{
    BupDir              dir;
    Uint8               *content;

    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (JO_NULL);
    }
    if (BUP_Dir(backup_device, (Uint8 *)fname, 1, &dir) != 1)
        return (JO_NULL);
    if ((content = (Uint8 *)jo_malloc_with_behaviour(dir.datasize + 1, JO_MALLOC_TRY_REUSE_BLOCK)) == JO_NULL)
    {
#ifdef JO_DEBUG
        jo_core_error("Out of memory");
#endif
        return (JO_NULL);
    }
    slResetDisable();
    __jo_backup_devices[backup_device].status = BUP_Read(backup_device, (Uint8 *)fname, content);
    if (__jo_backup_devices[backup_device].status == 0)
        __jo_backup_devices[backup_device].status = BUP_Verify(backup_device, (Uint8 *)fname, content);
    slResetEnable();
    if (__jo_backup_devices[backup_device].status != 0)
    {
        jo_free(content);
        return (JO_NULL);
    }
    if (length != JO_NULL)
        *length = dir.datasize;
    JO_ZERO(content[dir.datasize]);
    return (content);
}

bool                    jo_backup_delete_file(const jo_backup_device backup_device, const char * const fname)
{
    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (false);
    }
    slResetDisable();
	__jo_backup_devices[backup_device].status = BUP_Delete(backup_device, (Uint8 *)fname);
	slResetEnable();
	BUP_Stat(backup_device, 10, &__jo_backup_devices[backup_device].sttb);
    return (__jo_backup_devices[backup_device].status == 0);
}

bool                        jo_backup_unmount(const jo_backup_device backup_device)
{
    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (false);
    }
    __jo_backup_devices[backup_device].is_mounted = false;
    if (!__jo_backup_devices[JoInternalMemoryBackup].is_mounted && !__jo_backup_devices[JoCartridgeMemoryBackup].is_mounted && !__jo_backup_devices[JoExternalDeviceBackup].is_mounted)
    {
        jo_free(__jo_backup_lib_space);
        jo_free(__jo_backup_work_space);
        __jo_backup_initialized = false;
    }
    return (true);
}

bool                jo_backup_get_file_last_modified_date(const jo_backup_device backup_device, const char * const fname, jo_datetime *datetime)
{
    BupDir          dir;
    BupDate         bdate;

    if (datetime == JO_NULL)
    {
#ifdef JO_DEBUG
        jo_core_error("datetime is null");
#endif
        return (false);
    }
    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (false);
    }
    if (BUP_Dir(backup_device, (Uint8 *)fname, 1, &dir) == 1)
    {
        BUP_GetDate(dir.date, &bdate);
        datetime->year = bdate.year + 1980;
        datetime->month = bdate.month;
        datetime->week = bdate.week;
        datetime->day = bdate.day;
        datetime->hour = bdate.time;
        datetime->minute = bdate.min;
        return (true);
    }
    return (false);
}

jo_backup_status    jo_backup_get_last_status(const jo_backup_device backup_device)
{
    if (!__jo_backup_devices[backup_device].is_mounted)
    {
#ifdef JO_DEBUG
        jo_core_error("Device not mounted");
#endif
        return (JoBackupStatusUnknown);
    }
    switch (__jo_backup_devices[backup_device].status)
    {
        case 0:
        case BUP_NON:
            return (JoBackupStatusOk);
        case BUP_WRITE_PROTECT:
            return (JoBackupStatusWriteProtect);
        case BUP_NOT_ENOUGH_MEMORY:
            return (JoBackupStatusOutOfMemory);
        case BUP_NOT_FOUND:
            return (JoBackupStatusNotFound);
        case BUP_NO_MATCH:
            return (JoBackupStatusNoMatch);
        case BUP_BROKEN:
            return (JoBackupStatusBroken);
        default:
            return (JoBackupStatusUnknown);
    }
}

#endif /* !JO_COMPILE_WITH_BACKUP_SUPPORT */

/*
** END OF FILE
*/
