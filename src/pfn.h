/* The MIT License (MIT)
 *
 * Copyright (c) 2017 Stefano Trettel
 *
 * Software repository: MoonSndFile, https://github.com/stetre/moonsndfile
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef pfnDEFINED
#define pfnDEFINED

#include "moonsndfile.h"

typedef SNDFILE* (*PFN_sf_open)(const char *path, int mode, SF_INFO *sfinfo);
typedef SNDFILE* (*PFN_sf_open_fd)(int fd, int mode, SF_INFO *sfinfo, int close_desc);
typedef SNDFILE* (*PFN_sf_open_virtual)(SF_VIRTUAL_IO *sfvirtual, int mode, SF_INFO *sfinfo, void *user_data);
typedef int (*PFN_sf_error)(SNDFILE *sndfile);
typedef const char* (*PFN_sf_strerror)(SNDFILE *sndfile);
typedef const char* (*PFN_sf_error_number)(int errnum);
typedef int (*PFN_sf_perror)(SNDFILE *sndfile);
typedef int (*PFN_sf_error_str)(SNDFILE *sndfile, char* str, size_t len);
typedef int (*PFN_sf_command)(SNDFILE *sndfile, int command, void *data, int datasize);
typedef int (*PFN_sf_format_check)(const SF_INFO *info);
typedef sf_count_t (*PFN_sf_seek)(SNDFILE *sndfile, sf_count_t frames, int whence);
typedef int (*PFN_sf_set_string)(SNDFILE *sndfile, int str_type, const char* str);
typedef const char* (*PFN_sf_get_string)(SNDFILE *sndfile, int str_type);
typedef const char * (*PFN_sf_version_string)(void);
typedef int (*PFN_sf_current_byterate)(SNDFILE *sndfile);
typedef sf_count_t (*PFN_sf_read_raw)(SNDFILE *sndfile, void *ptr, sf_count_t bytes);
typedef sf_count_t (*PFN_sf_write_raw)(SNDFILE *sndfile, const void *ptr, sf_count_t bytes);
typedef sf_count_t (*PFN_sf_readf_short)(SNDFILE *sndfile, short *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_writef_short)(SNDFILE *sndfile, const short *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_readf_int)(SNDFILE *sndfile, int *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_writef_int)(SNDFILE *sndfile, const int *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_readf_float)(SNDFILE *sndfile, float *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_writef_float)(SNDFILE *sndfile, const float *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_readf_double)(SNDFILE *sndfile, double *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_writef_double)(SNDFILE *sndfile, const double *ptr, sf_count_t frames);
typedef sf_count_t (*PFN_sf_read_short)(SNDFILE *sndfile, short *ptr, sf_count_t items);
typedef sf_count_t (*PFN_sf_write_short)(SNDFILE *sndfile, const short *ptr, sf_count_t items);
typedef sf_count_t (*PFN_sf_read_int)(SNDFILE *sndfile, int *ptr, sf_count_t items);
typedef sf_count_t (*PFN_sf_write_int)(SNDFILE *sndfile, const int *ptr, sf_count_t items);
typedef sf_count_t (*PFN_sf_read_float)(SNDFILE *sndfile, float *ptr, sf_count_t items);
typedef sf_count_t (*PFN_sf_write_float)(SNDFILE *sndfile, const float *ptr, sf_count_t items);
typedef sf_count_t (*PFN_sf_read_double)(SNDFILE *sndfile, double *ptr, sf_count_t items);
typedef sf_count_t (*PFN_sf_write_double)(SNDFILE *sndfile, const double *ptr, sf_count_t items);
typedef int (*PFN_sf_close)(SNDFILE *sndfile);
typedef void (*PFN_sf_write_sync)(SNDFILE *sndfile);
#if(defined(ENABLE_SNDFILE_WINDOWS_PROTOTYPES) && ENABLE_SNDFILE_WINDOWS_PROTOTYPES)
typedef SNDFILE* (*PFN_sf_wchar_open)(LPCWSTR wpath, int mode, SF_INFO *sfinfo);
#endif
typedef int (*PFN_sf_set_chunk)(SNDFILE * sndfile, const SF_CHUNK_INFO * chunk_info);
typedef SF_CHUNK_ITERATOR * (*PFN_sf_get_chunk_iterator)(SNDFILE * sndfile, const SF_CHUNK_INFO * chunk_info);
typedef SF_CHUNK_ITERATOR * (*PFN_sf_next_chunk_iterator)(SF_CHUNK_ITERATOR * iterator);
typedef int (*PFN_sf_get_chunk_size)(const SF_CHUNK_ITERATOR * it, SF_CHUNK_INFO * chunk_info);
typedef int (*PFN_sf_get_chunk_data)(const SF_CHUNK_ITERATOR * it, SF_CHUNK_INFO * chunk_info);

#endif /* pfnDEFINED */
