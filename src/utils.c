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

/*------------------------------------------------------------------------------*
 | Misc utilities                                                               |
 *------------------------------------------------------------------------------*/

int noprintf(const char *fmt, ...) 
    { (void)fmt; return 0; }

int notavailable(lua_State *L, ...) 
    { 
    return luaL_error(L, "function not available in this CL version");
    }

int tablelen(lua_State *L, int arg)
/* counts the elements of the table at arg */
    {
    int n = 0;
    if(!lua_istable(L, arg))
        return luaL_error(L, errstring(ERR_TABLE));
    lua_pushnil(L);
    while(lua_next(L, arg) != 0)
        { lua_pop(L, 1); n++; }
    return n;
    }


/*------------------------------------------------------------------------------*
 | Time utilities (POSIX)                                                       |
 *------------------------------------------------------------------------------*/

double now(void)
    {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    if(clock_gettime(CLOCK_MONOTONIC,&ts)!=0)
        { printf("clock_gettime error\n"); return -1; }
    return ts.tv_sec + ts.tv_nsec*1.0e-9;
#else
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0)
        { printf("gettimeofday error\n"); return -1; }
    return tv.tv_sec + tv.tv_usec*1.0e-6;
#endif
    }

double tstosec(const struct timespec *ts)
    {
    return ts->tv_sec*1.0+ts->tv_nsec*1.0e-9;
    }

void sectots(struct timespec *ts, double seconds)
    {
    ts->tv_sec=(time_t)seconds;
    ts->tv_nsec=(long)((seconds-((double)ts->tv_sec))*1.0e9);
    }

void sleeep(double seconds)
    {
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts, ts1;
    struct timespec *req, *rem, *tmp;
    sectots(&ts, seconds);
    req = &ts;
    rem = &ts1; 
    while(1)
        {   
        if(nanosleep(req, rem) == 0) 
            return;
        tmp = req;
        req = rem;
        rem = tmp;
        }
#else
    usleep((useconds_t)(seconds*1.0e6));
#endif
    }


/*------------------------------------------------------------------------------*
 | Malloc                                                                       |
 *------------------------------------------------------------------------------*/

/* We do not use malloc(), free() etc directly. Instead, we inherit the memory 
 * allocator from the main Lua state instead (see lua_getallocf in the Lua manual)
 * and use that.
 *
 * By doing so, we can use an alternative malloc() implementation without recompiling
 * this library (we have needs to recompile lua only, or execute it with LD_PRELOAD
 * set to the path to the malloc library we want to use).
 */
static lua_Alloc Alloc = NULL;
static void* AllocUd = NULL;

void malloc_init(lua_State *L)
    {
    if(Alloc) unexpected(L);
    Alloc = lua_getallocf(L, &AllocUd);
    }

static void* Malloc_(size_t size)
    { return Alloc ? Alloc(AllocUd, NULL, 0, size) : NULL; }

static void Free_(void *ptr)
    { if(Alloc) Alloc(AllocUd, ptr, 0, 0); }

void *Malloc(lua_State *L, size_t size)
    {
    void *ptr = Malloc_(size);
    if(ptr==NULL)
        { luaL_error(L, errstring(ERR_MEMORY)); return NULL; }
    memset(ptr, 0, size);
    //DBG("Malloc %p\n", ptr);
    return ptr;
    }

void *MallocNoErr(lua_State *L, size_t size) /* do not raise errors (check the retval) */
    {
    void *ptr = Malloc_(size);
    (void)L;
    if(ptr==NULL)
        return NULL;
    memset(ptr, 0, size);
    //DBG("MallocNoErr %p\n", ptr);
    return ptr;
    }

char *Strdup(lua_State *L, const char *s)
    {
    size_t len = strnlen(s, 256);
    char *ptr = (char*)Malloc(L, len + 1);
    if(len>0)
        memcpy(ptr, s, len);
    ptr[len]='\0';
    return ptr;
    }


void Free(lua_State *L, void *ptr)
    {
    (void)L;
    //DBG("Free %p\n", ptr);
    if(ptr) Free_(ptr);
    }

/*------------------------------------------------------------------------------*
 | Light userdata                                                               |
 *------------------------------------------------------------------------------*/

void *checklightuserdata(lua_State *L, int arg)
    {
    if(lua_type(L, arg) != LUA_TLIGHTUSERDATA)
        { luaL_argerror(L, arg, "expected lightuserdata"); return NULL; }
    return lua_touserdata(L, arg);
    }
    
void *optlightuserdata(lua_State *L, int arg)
    {
    if(lua_isnoneornil(L, arg))
        return NULL;
    return checklightuserdata(L, arg);
    }

/*------------------------------------------------------------------------------*
 | Custom luaL_checkxxx() style functions                                       |
 *------------------------------------------------------------------------------*/

int checkboolean(lua_State *L, int arg)
    {
    if(!lua_isboolean(L, arg))
        return (int)luaL_argerror(L, arg, "boolean expected");
    return lua_toboolean(L, arg);
    }


int testboolean(lua_State *L, int arg, int *err)
    {
    if(!lua_isboolean(L, arg))
        { *err = ERR_TYPE; return 0; }
    *err = 0;
    return lua_toboolean(L, arg);
    }


int optboolean(lua_State *L, int arg, int d)
    {
    if(!lua_isboolean(L, arg))
        return d;
    return lua_toboolean(L, arg);
    }

#if 0
/* 1-based index to 0-based ------------------------------------------*/

int testindex(lua_State *L, int arg, int *err)
    {
    int val;
    if(!lua_isinteger(L, arg))
        { *err = ERR_TYPE; return 0; }
    val = lua_tointeger(L, arg);
    if(val < 1)
        { *err = ERR_GENERIC; return 0; }
    *err = 0;
    return val - 1;
    }

int checkindex(lua_State *L, int arg)
    {
    int val = luaL_checkinteger(L, arg);
    if(val < 1)
        return luaL_argerror(L, arg, "positive integer expected");
    return val - 1;
    }

int optindex(lua_State *L, int arg, int optval /* 0-based */)
    {
    int val = luaL_optinteger(L, arg, optval + 1);
    if(val < 1)
        return luaL_argerror(L, arg, "positive integer expected");
    return val - 1;
    }

void pushindex(lua_State *L, int val)
    { lua_pushinteger((L), (val) + 1); }
#endif

/*------------------------------------------------------------------------------*
 | Internal error codes                                                         |
 *------------------------------------------------------------------------------*/

const char* errstring(int err)
    {
    switch(err)
        {
        case 0: return "success";
        case ERR_GENERIC: return "generic error";
        case ERR_TABLE: return "not a table";
        case ERR_EMPTY: return "empty list";
        case ERR_TYPE: return "invalid type";
        case ERR_VALUE: return "invalid value";
        case ERR_NOTPRESENT: return "missing";
        case ERR_MEMORY: return "out of memory";
        case ERR_LENGTH: return "invalid length";
        case ERR_POOL: return "elements are not from the same pool";
        case ERR_BOUNDARIES: return "invalid boundaries";
        case ERR_UNKNOWN: return "unknown field name";
        default:
            return "???";
        }
    return NULL; /* unreachable */
    }


/*------------------------------------------------------------------------------*
 | libsndfile error codes                                                       |
 *------------------------------------------------------------------------------*/

#if 0
int pushsferror(lua_State *L, int ec)
    {
    const char *str;
    switch(ec)
        {
#define CASE(c, s) case c: lua_pushstring(L, s); return 1
        CASE(SF_ERR_NO_ERROR, "no error");
        CASE(SF_ERR_UNRECOGNISED_FORMAT, "unrecognised format");
        CASE(SF_ERR_SYSTEM, "system error");
        CASE(SF_ERR_MALFORMED_FILE, "malformed file");
        CASE(SF_ERR_UNSUPPORTED_ENCODING, "unsupported encoding");
#undef CASE
        default:
            str = sf.error_number(ec);
            lua_pushfstring(L, "%s", str ? str : "unknown error");
            /* "There are also a large number of private error numbers which are 
             * internal to the library which can change at any time." (sndfile.h)
             */
            return 1;
        }
    return 0;
    }
#endif

int sndfileerror(lua_State *L, sndfile_t sndfile)
    { 
    const char *str = sf.strerror(sndfile);
    lua_pushstring(L, str ? str : "unknown error");
    return lua_error(L);
    }

int nil_sndfileerror(lua_State *L, sndfile_t sndfile)
    { 
    const char *str = sf.strerror(sndfile);
    lua_pushnil(L);
    lua_pushstring(L, str ? str : "unknown error");
    return 2;
    }

