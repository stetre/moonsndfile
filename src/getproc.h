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

#ifndef getprocDEFINED
#define getprocDEFINED

#include "pfn.h"

#define CheckPfn(L, pfn, major, minor, patch) do {          \
    if(sf.pfn==NULL)                                        \
        {                                                   \
        if(!checkminversion((major), (minor), (patch)))     \
            return luaL_error((L),                          \
                "sf_"#pfn" requires libsndfile version >= %d.%d.%d", (major), (minor), (patch));\
        else                                                \
            return luaL_error((L), "sf_"#pfn" address not found");  \
        }                                                   \
} while(0)

/* Dispatch tables */

/* Global functions */
#define sf_dt_t moonsndfile_sf_dt_t
typedef struct {
    PFN_sf_open open;
    PFN_sf_open_fd open_fd;
    PFN_sf_open_virtual open_virtual;
    PFN_sf_error error;
    PFN_sf_strerror strerror;
    PFN_sf_error_number error_number;
    PFN_sf_perror perror;
    PFN_sf_error_str error_str;
    PFN_sf_command command;
    PFN_sf_format_check format_check;
    PFN_sf_seek seek;
    PFN_sf_set_string set_string;
    PFN_sf_get_string get_string;
    PFN_sf_version_string version_string;
    PFN_sf_current_byterate current_byterate;
    PFN_sf_read_raw read_raw;
    PFN_sf_write_raw write_raw;
    PFN_sf_readf_short readf_short;
    PFN_sf_writef_short writef_short;
    PFN_sf_readf_int readf_int;
    PFN_sf_writef_int writef_int;
    PFN_sf_readf_float readf_float;
    PFN_sf_writef_float writef_float;
    PFN_sf_readf_double readf_double;
    PFN_sf_writef_double writef_double;
    PFN_sf_read_short read_short;
    PFN_sf_write_short write_short;
    PFN_sf_read_int read_int;
    PFN_sf_write_int write_int;
    PFN_sf_read_float read_float;
    PFN_sf_write_float write_float;
    PFN_sf_read_double read_double;
    PFN_sf_write_double write_double;
    PFN_sf_close close;
    PFN_sf_write_sync write_sync;
#if(defined(ENABLE_SNDFILE_WINDOWS_PROTOTYPES) && ENABLE_SNDFILE_WINDOWS_PROTOTYPES)
    PFN_sf_wchar_open wchar_open;
#endif
    PFN_sf_set_chunk set_chunk;
    PFN_sf_get_chunk_iterator get_chunk_iterator;
    PFN_sf_next_chunk_iterator next_chunk_iterator;
    PFN_sf_get_chunk_size get_chunk_size;
    PFN_sf_get_chunk_data get_chunk_data;
} sf_dt_t;

#define sf moonsndfile_sf
extern sf_dt_t sf;

#endif /* getprocDEFINED */

