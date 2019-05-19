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
/** @file fs.h
 *  @author Johannes Fetz
 *
 *  @brief Jo Engine File System function prototypes
 *  @bug No known bugs.
 */

#ifndef __JO_FS_H__
# define __JO_FS_H__

/*
** MACROS
*/
/** @brief Specify the root directory (on the CD) for jo_fs_read_file(), jo_map_load_from_file(), etc.  */
# define JO_ROOT_DIR				((const char *)0)
/** @brief Specify the current directory (on the CD) for jo_fs_read_file(), jo_map_load_from_file(), etc.  */
# define JO_CURRENT_DIR				((const char *)0)
/** @brief Specify the root directory (on the CD) for jo_fs_read_file(), jo_map_load_from_file(), etc.  */
# define JO_PARENT_DIR				("..")

/*
** TYPEDEFS
*/
/** @brief Function prototype for () */
typedef void	(*jo_fs_async_read_callback)(char *contents, int length, int optional_token);

/** @brief Change the current directory (equivalent of Unix cd command)
 *  @param sub_dir Sub directory name (use JO_PARENT_DIR for parent directory)
 */
void            jo_fs_cd(const char *const sub_dir);

/** @brief Read a file on the CD
 *  @param filename Filename (upper case and shorter as possible like "A.TXT")
 *  @param sub_dir Sub directory name (use JO_ROOT_DIR if the file is on the root directory)
 *  @param len return the file length
 *  @return The stream
 */
char*			jo_fs_read_file_in_dir(const char *const filename, const char *const sub_dir, int *len);

/** @brief Read a file on the CD
 *  @param filename Filename (upper case and shorter as possible like "A.TXT")
 *  @param len return the file length
 *  @return The stream
 */
char*			jo_fs_read_file(const char *const filename, int *len);

int             jo_fs_read_file_async(const char *const filename, jo_fs_async_read_callback callback, int optional_token);

#endif /* !__JO_FS_H__ */

/*
** END OF FILE
*/
