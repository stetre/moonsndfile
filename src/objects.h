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

#ifndef objectsDEFINED
#define objectsDEFINED

#include "tree.h"
#include "udata.h"


/*------------------------------------------------------*/

/* Objects' metatable names */
#define SNDFILE_MT "moonsndfile_sndfile"

/* Userdata memory associated with objects */
#define ud_t moonsndfile_ud_t
typedef struct moonsndfile_ud_s ud_t;

struct moonsndfile_ud_s {
    void *handle; /* the object handle bound to this userdata */
    int (*destructor)(lua_State *L, ud_t *ud);  /* self destructor */
    ud_t *parent_ud; /* the ud of the parent object */
    uint32_t marks;
    SF_INFO sfinfo;
    chunknode_t *chunklist;
    void *info; /* object specific info (ud_info_t, subject to Free() at destruction, if not NULL) */
};
    
/* Marks.  m_ = marks word (uint32_t) , i_ = bit number (0 .. 31)  */
#define MarkGet(m_,i_)  (((m_) & ((uint32_t)1<<(i_))) == ((uint32_t)1<<(i_)))
#define MarkSet(m_,i_)  do { (m_) = ((m_) | ((uint32_t)1<<(i_))); } while(0)
#define MarkReset(m_,i_) do { (m_) = ((m_) & (~((uint32_t)1<<(i_)))); } while(0)

#define IsValid(ud)             MarkGet((ud)->marks, 0)
#define MarkValid(ud)           MarkSet((ud)->marks, 0) 
#define CancelValid(ud)         MarkReset((ud)->marks, 0)

#if 0
/* .c */
#define  moonsndfile_
#endif

#define newuserdata moonsndfile_newuserdata
ud_t *newuserdata(lua_State *L, void *handle, const char *mt);
#define freeuserdata moonsndfile_freeuserdata
int freeuserdata(lua_State *L, ud_t *ud);
#define pushuserdata moonsndfile_pushuserdata 
int pushuserdata(lua_State *L, ud_t *ud);

#define userdata_unref(L, handle) udata_unref((L),(handle))

#define UD(handle) userdata((handle)) /* dispatchable objects only */
#define userdata moonsndfile_userdata
ud_t *userdata(void *handle);
#define testxxx moonsndfile_testxxx
void *testxxx(lua_State *L, int arg, ud_t **udp, const char *mt);
#define checkxxx moonsndfile_checkxxx
void *checkxxx(lua_State *L, int arg, ud_t **udp, const char *mt);
#define pushxxx moonsndfile_pushxxx
int pushxxx(lua_State *L, void *handle);
#define checkxxxlist moonsndfile_checkxxxlist
void** checkxxxlist(lua_State *L, int arg, uint32_t *count, int *err, const char *mt);

#define freechildren moonsndfile_freechildren
int freechildren(lua_State *L,  const char *mt, ud_t *parent_ud);

/* sndfile.c */
#define checksndfile(L, arg, udp) (sndfile_t)checkxxx((L), (arg), (udp), SNDFILE_MT)
#define testsndfile(L, arg, udp) (sndfile_t)testxxx((L), (arg), (udp), SNDFILE_MT)
#define pushsndfile(L, handle) pushxxx((L), (handle))
#define checksndfilelist(L, arg, count, err) (sndfile_t*)checkxxxlist((L), (arg), (count), (err), SNDFILE_MT)

#if 0 /* scaffolding 6yy */
/* zzz.c */
#define checkzzz(L, arg, udp) (zzz_t)checkxxx((L), (arg), (udp), ZZZ_MT)
#define testzzz(L, arg, udp) (zzz_t)testxxx((L), (arg), (udp), ZZZ_MT)
#define pushzzz(L, handle) pushxxx((L), (handle))
#define checkzzzlist(L, arg, count, err) (zzz_t*)checkxxxlist((L), (arg), (count), (err), ZZZ_MT)

#endif

/* used in main.c */
void moonsndfile_open_sndfile(lua_State *L);

#define RAW_FUNC(xxx)                       \
static int Raw(lua_State *L)                \
    {                                       \
    lua_pushinteger(L, (uintptr_t)check##xxx(L, 1, NULL));  \
    return 1;                               \
    }

#define TYPE_FUNC(xxx) /* NONCL */          \
static int Type(lua_State *L)               \
    {                                       \
    (void)check##xxx(L, 1, NULL);           \
    lua_pushstring(L, ""#xxx);              \
    return 1;                               \
    }

#define DELETE_FUNC(xxx)                    \
static int Delete(lua_State *L)             \
    {                                       \
    ud_t *ud;                               \
    (void)test##xxx(L, 1, &ud);             \
    if(!ud) return 0; /* already deleted */ \
    return ud->destructor(L, ud);           \
    }

#define PARENT_FUNC(xxx)                    \
static int Parent(lua_State *L)             \
    {                                       \
    ud_t *ud;                               \
    (void)check##xxx(L, 1, &ud);            \
    if(!ud->parent_ud) return 0;            \
    return pushuserdata(L, ud->parent_ud);  \
    }

#endif /* objectsDEFINED */
