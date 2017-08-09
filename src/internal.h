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

/********************************************************************************
 * Internal common header                                                       *
 ********************************************************************************/

#ifndef internalDEFINED
#define internalDEFINED

#define _ISOC11_SOURCE /* see man aligned_alloc(3) */
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "moonsndfile.h"

/* API TYPES ADAPTATION (for internal use) ------------- */
#define sndfile_t SNDFILE*
#define chunk_t SF_CHUNK_INFO
#define chunkiter_t SF_CHUNK_ITERATOR

typedef struct chunknode_s chunknode_t;
struct chunknode_s {
    chunknode_t *next;
    chunk_t chunk;
};

/* ----------------------------------------------------- */

#define TOSTR_(x) #x
#define TOSTR(x) TOSTR_(x)

#include "tree.h"
#include "getproc.h"
#include "objects.h"
#include "enums.h"

/* Note: all the dynamic symbols of this library (should) start with 'moonsndfile_' .
 * The only exception is the luaopen_moonsndfile() function, which is searched for
 * with that name by Lua.
 * MoonSndFile's string references on the Lua registry also start with 'moonsndfile_'.
 */


#if 0
/* .c */
#define  moonsndfile_
#endif

/* utils.c */
void moonsndfile_utils_init(lua_State *L);
#define noprintf moonsndfile_noprintf
int noprintf(const char *fmt, ...); 
#define now moonsndfile_now
double now(void);
#define since(t) (now() - (t))
#define sleeep moonsndfile_sleeep
void sleeep(double seconds);
#define notavailable moonsndfile_notavailable
int notavailable(lua_State *L, ...);
#define tablelen moonsndfile_tablelen
int tablelen(lua_State *L, int arg);
#define Malloc moonsndfile_Malloc
void *Malloc(lua_State *L, size_t size);
#define MallocNoErr moonsndfile_MallocNoErr
void *MallocNoErr(lua_State *L, size_t size);
#define Strdup moonsndfile_Strdup
char *Strdup(lua_State *L, const char *s);
#define Free moonsndfile_Free
void Free(lua_State *L, void *ptr);
#define checkboolean moonsndfile_checkboolean
int checkboolean(lua_State *L, int arg);
#define testboolean moonsndfile_testboolean
int testboolean(lua_State *L, int arg, int *err);
#define optboolean moonsndfile_optboolean
int optboolean(lua_State *L, int arg, int d);
#define checklightuserdata moonsndfile_checklightuserdata
void *checklightuserdata(lua_State *L, int arg);
#define optlightuserdata moonsndfile_optlightuserdata
void *optlightuserdata(lua_State *L, int arg);
#if 0
#define testindex moonsndfile_testindex
int testindex(lua_State *L, int arg, int *err);
#define checkindex moonsndfile_checkindex
int checkindex(lua_State *L, int arg);
#define optindex moonsndfile_optindex
int optindex(lua_State *L, int arg, int optval);
#define pushindex moonsndfile_pushindex
void pushindex(lua_State *L, int val);
#endif

#define sndfileerror moonsndfile_sndfileerror
int sndfileerror(lua_State *L, sndfile_t sndfile);
#define nil_sndfileerror moonsndfile_nil_sndfileerror
int nil_sndfileerror(lua_State *L, sndfile_t sndfile);

/* Internal error codes */
#define ERR_NOTPRESENT       1
#define ERR_SUCCESS          0
#define ERR_GENERIC         -1
#define ERR_TYPE            -2
#define ERR_VALUE           -3
#define ERR_TABLE           -4
#define ERR_EMPTY           -5
#define ERR_MEMORY          -6
#define ERR_LENGTH          -7
#define ERR_POOL            -8
#define ERR_BOUNDARIES      -9
#define ERR_UNKNOWN         -10
#define errstring moonsndfile_errstring
const char* errstring(int err);

/* tracing.c */
#define trace_objects moonsndfile_trace_objects
extern int trace_objects;

/* sfversion.c */
#define sfversion moonsndfile_sfversion
uint32_t sfversion(void);
#define checkminversion moonsndfile_checkminversion
int checkminversion(uint32_t major, uint32_t minor, uint32_t patch);

/* structs.c */
#define checkinfo moonsndfile_checkinfo
int checkinfo(lua_State *L, int arg, SF_INFO *p);
#define pushinfo moonsndfile_pushinfo
int pushinfo(lua_State *L, SF_INFO *p);
#define pushformatinfo moonsndfile_pushformatinfo
int pushformatinfo(lua_State *L, SF_FORMAT_INFO *p);
#define pushsubformatinfo moonsndfile_pushsubformatinfo
int pushsubformatinfo(lua_State *L, SF_FORMAT_INFO *p);
#define pushsimpleformat moonsndfile_pushsimpleformat
int pushsimpleformat(lua_State *L, SF_FORMAT_INFO *p);
#define freechunk moonsndfile_freechunk
int freechunk(lua_State *L, chunknode_t *p);
#define checkchunk moonsndfile_checkchunk
chunknode_t *checkchunk(lua_State *L, int arg);
#define pushchunk moonsndfile_pushchunk
int pushchunk(lua_State *L, chunk_t *p);

/* sndfile.c */
#define update_sfinfo moonsndfile_update_sfinfo
int update_sfinfo(lua_State *L, sndfile_t sndfile, ud_t *ud);

/* command.c.c */
#define command_void moonsndfile_command_void
int command_void(lua_State *L, int command);
#define command_set_count moonsndfile_command_set_count
int command_set_count(lua_State *L, int command);
#define command_get_boolean moonsndfile_command_get_boolean
int command_get_boolean(lua_State *L, int command);
#define command_set_boolean moonsndfile_command_set_boolean
int command_set_boolean(lua_State *L, int command);
#define command_get_double_all_channels moonsndfile_command_get_double_all_channels
int command_get_double_all_channels(lua_State *L, int command, int success_retcode);
#define command_get_double moonsndfile_command_get_double
int command_get_double(lua_State *L, int command);
#define command_set_double moonsndfile_command_set_double
int command_set_double(lua_State *L, int command);

/* chunk.c */
#define freechunks moonsndfile_freechunks
void freechunks(lua_State *L, chunknode_t *p);

/* main.c */
int luaopen_moonsndfile(lua_State *L);
void moonsndfile_open_version(lua_State *L);
int moonsndfile_open_getproc(lua_State *L);
void moonsndfile_atexit_getproc(void);
void moonsndfile_open_enums(lua_State *L);
void moonsndfile_open_tracing(lua_State *L);
void moonsndfile_open_readwrite(lua_State *L);
void moonsndfile_open_rawreadwrite(lua_State *L);
void moonsndfile_open_datahandling(lua_State *L);
void moonsndfile_open_command(lua_State *L);
void moonsndfile_open_format(lua_State *L);
void moonsndfile_open_chunk(lua_State *L);


/*------------------------------------------------------------------------------*
 | Debug and other utilities                                                    |
 *------------------------------------------------------------------------------*/

#define TRACE_CREATE(p, ttt) do {                                   \
    if(trace_objects) { printf("create "ttt" 0x%.16llx\n", (long long unsigned int)(uintptr_t)(p)); }   \
} while(0)

#define TRACE_DELETE(p, ttt) do {                                   \
    if(trace_objects) { printf("delete "ttt" 0x%.16llx\n", (long long unsigned int)(uintptr_t)(p)); }   \
} while(0)

/* If this is printed, it denotes a suspect bug: */
#define UNEXPECTED_ERROR "unexpected error (%s, %d)", __FILE__, __LINE__
#define unexpected(L) luaL_error((L), UNEXPECTED_ERROR)

#define notsupported(L) luaL_error((L), "operation not supported")

#define badvalue(L,s)   lua_pushfstring((L), "invalid value '%s'", (s))

/* DEBUG -------------------------------------------------------- */
#if defined(DEBUG)

#define DBG printf
#define TR() do { printf("trace %s %d\n",__FILE__,__LINE__); } while(0)
#define BK() do { printf("break %s %d\n",__FILE__,__LINE__); getchar(); } while(0)
#define TSTART double ts = now();
#define TSTOP do {                                          \
    ts = since(ts); ts = ts*1e6;                            \
    printf("%s %d %.3f us\n", __FILE__, __LINE__, ts);      \
    ts = now();                                             \
} while(0);

#else 

#define DBG noprintf
#define TR()
#define BK()
#define TSTART do {} while(0) 
#define TSTOP do {} while(0)    

#endif /* DEBUG ------------------------------------------------- */


#endif /* internalDEFINED */
