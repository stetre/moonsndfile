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

#include "internal.h"

moonsndfile_sf_dt_t sf;   /* sndfile.h dispatch table */

#define FP(f) *(void**)(&(f))
/* Cast to silent compiler warnings without giving up the -Wpedantic flag.
 *("ISO C forbids conversion of function pointer to object pointer type")
 */

#if defined(LINUX)
#include <dlfcn.h>
#define LIBNAME "libsndfile.so"
static void *Handle = NULL;

#elif defined(MINGW)
#include <windows.h>
#define LIBNAME "libsndfile-1.dll"
#define LLIBNAME L"libsndfile-1.dll"
#define LIBNAME1 "sndfile.dll"
#define LLIBNAME1 L"sndfile.dll"
static HMODULE Handle = NULL;

#else
#error "Cannot determine platform"
#endif

static int Init(lua_State *L)
    {
#if defined(LINUX)
    char *err;
    Handle = dlopen(LIBNAME, RTLD_LAZY | RTLD_LOCAL);
    if(!Handle)
        {
        err = dlerror();
        return luaL_error(L, err != NULL ? err : "cannot load "LIBNAME);
        }
#define GET(fn) do {                                             \
    FP(sf.fn) = dlsym(Handle, "sf_"#fn);                         \
    if(!sf.fn) return luaL_error(L, "cannot find sf_"#fn);       \
} while(0)
#define OPT(fn) do {    /* optional */                           \
    FP(sf.fn) = dlsym(Handle, "sf_"#fn);                         \
} while(0)

#elif defined(MINGW)
    Handle = LoadLibraryW(LLIBNAME);
    if(!Handle)
        Handle = LoadLibraryW(LLIBNAME1);
    if(!Handle)
        return luaL_error(L, "cannot load " LIBNAME " or " LIBNAME1);
#define GET(fn) do {                                              \
    sf.fn = (PFN_sf_##fn)GetProcAddress(Handle, "sf_"#fn);        \
    if(!sf.fn) return luaL_error(L, "cannot find sf_"#fn);        \
} while(0)
#define OPT(fn) do {    /* optional */                            \
    sf.fn = (PFN_sf_##fn)GetProcAddress(Handle, "sf_"#fn);        \
} while(0)
#endif

    /* If MoonSndFile loads successfully, these function pointers are guaranteed
     * to be valid so they need not be checked before using them.
     */
    GET(open);
    GET(open_fd);
    GET(open_virtual);
    GET(error);
    GET(strerror);
    GET(error_number);
    GET(perror);
    GET(error_str);
    GET(command);
    GET(format_check);
    GET(seek);
    GET(set_string);
    GET(get_string);
    GET(version_string);
    GET(read_raw);
    GET(write_raw);
    GET(readf_short);
    GET(writef_short);
    GET(readf_int);
    GET(writef_int);
    GET(readf_float);
    GET(writef_float);
    GET(readf_double);
    GET(writef_double);
    GET(read_short);
    GET(write_short);
    GET(read_int);
    GET(write_int);
    GET(read_float);
    GET(write_float);
    GET(read_double);
    GET(write_double);
    GET(close);
    GET(write_sync);
#if(defined(ENABLE_SNDFILE_WINDOWS_PROTOTYPES) && ENABLE_SNDFILE_WINDOWS_PROTOTYPES)
    GET(wchar_open);
#endif
    /* Optional functions (i.e. from versions > v1.0.25)
     * These functions pointers may be NULL, so check before using them.
     */
    OPT(current_byterate); /* ver >= 1.0.26 */
    OPT(set_chunk); /* ver >= 1.0.26 */
    OPT(get_chunk_iterator); /* ver >= 1.0.26 */
    OPT(next_chunk_iterator); /* ver >= 1.0.26 */
    OPT(get_chunk_size);/* ver >= 1.0.26 */
    OPT(get_chunk_data);/* ver >= 1.0.26 */
#undef GET
#undef OPT
    return 0;
    }

/*---------------------------------------------------------------------------*/

void moonsndfile_atexit_getproc(void)
    {
#if defined(LINUX)
    if(Handle) dlclose(Handle);
#elif defined(MINGW)
    if(Handle) FreeLibrary(Handle);
#endif
    }

int moonsndfile_open_getproc(lua_State *L)
    {
    Init(L);
    return 0;
    }

