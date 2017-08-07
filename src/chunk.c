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

static void insertchunk(ud_t *ud, chunknode_t *p)
/* insert new chunknode in ud->chunklist */
    {
    p->next = ud->chunklist;
    ud->chunklist = p;
    }

void freechunks(lua_State *L, chunknode_t *list)
/* free all cunks in ud->chunklist */
    {
    chunknode_t *pp, *p = list;
    while(p)
        {
        pp = p;
        p = p->next;
        freechunk(L, pp);
        }
    }

static int SetChunk(lua_State *L)
    {
    ud_t *ud;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    chunknode_t *chunknode = checkchunk(L, 2);
    
    CheckPfn(L, set_chunk, 1, 0, 26);
    if(sf.set_chunk(sndfile, &chunknode->chunk) != 0)
        {
        freechunk(L, chunknode);
        return sndfileerror(L, sndfile);
        }
    
    insertchunk(ud, chunknode); 
    /* The chunk will be released after file closure, as requested in sndfile.h:
     * ("The chunk_info->data pointer must be valid until the file is closed.")
     */
    return 0;
    }

static int GetChunks(lua_State *L)
/* {chunk} = get_chunks(sndfile, [id]) */
    {
    ud_t *ud;
    const char *id;
    size_t id_size, i;
    chunk_t chunk;
    chunkiter_t *iter;

    sndfile_t sndfile = checksndfile(L, 1, &ud);
    id = luaL_optlstring(L, 2, NULL, &id_size);
    CheckPfn(L, get_chunk_iterator, 1, 0, 26);

    if(id)
        {
        if((id_size == 0) || (id_size > 64))
            return luaL_argerror(L, 2, "invalid id length");
        memset(&chunk, 0, sizeof(chunk));
        memcpy(&chunk.id, id, id_size);
        chunk.id_size = id_size;
        }

    iter = sf.get_chunk_iterator(sndfile, id ? &chunk : NULL);

    lua_newtable(L);
    i = 0;
    while(iter)
        {
        memset(&chunk, 0, sizeof(chunk));
        if(sf.get_chunk_size(iter, &chunk) != 0)
            {
            lua_pop(L, 1);
            return sndfileerror(L, sndfile);
            }
        /* Now chunk.id and chunk.id_size are properly set and chunk.datalen is set
         * to the data length. We need to allocate the memory for the data field,
         * and then retrieve the data.
         */
        if(chunk.datalen > 0)
            chunk.data = Malloc(L, chunk.datalen);
        if(sf.get_chunk_data(iter, &chunk) != 0)
            { 
            Free(L, chunk.data); 
            lua_pop(L, 1);
            return sndfileerror(L, sndfile);
            }
        pushchunk(L, &chunk);
        lua_rawseti(L, -2, ++i);
        Free(L, chunk.data); 
        iter = sf.next_chunk_iterator(iter);
        }
    return 1;
    }

static int TODO(lua_State *L) 
    {
    return luaL_error(L, "not implemented yet");
    }

#define SetCartInfo TODO //@@
#define GetCartInfo TODO //@@
#define GetCueCount TODO //@@
#define GetCue TODO //@@
#define SetCue TODO //@@
#define GetInstrument TODO //@@
#define SetInstrument TODO //@@
#define GetLoopInfo TODO //@@
#define GetBroadcastInfo TODO //@@
#define SetBroadcastInfo TODO //@@

#if 0 /* scaffolding */
int sf.command(SNDFILE *sndfile, int command, void *data, int datasize);
int sf.command(sndfile, command, void *data, int datasize);

static int (lua_State *L)
    { return (L, sfc_); }


        { "",  },
static int (lua_State *L) //@@
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
int sf.command(sndfile, command, void *data, int datasize);
    return 0;
    }
#endif


static const struct luaL_Reg Functions[] = 
    {
        { "set_chunk", SetChunk },
        { "get_chunks", GetChunks },
        { "set_cart_info", SetCartInfo },
        { "get_cart_info", GetCartInfo },
        { "get_cue_count", GetCueCount },
        { "get_cue", GetCue },
        { "set_cue", SetCue },
        { "get_instrument", GetInstrument },
        { "set_instrument", SetInstrument },
        { "get_loop_info", GetLoopInfo },
        { "get_broadcast_info", GetBroadcastInfo },
        { "set_broadcast_info", SetBroadcastInfo },
        { NULL, NULL } /* sentinel */
    };


void moonsndfile_open_chunk(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


