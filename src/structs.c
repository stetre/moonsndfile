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


int checkinfo(lua_State *L, int arg, SF_INFO *p)
    {
    int err, isnum;
    int code, format, subformat, endianness;
    if(lua_isnoneornil(L, arg))
        return luaL_argerror(L, arg, errstring(ERR_NOTPRESENT));
    if(lua_type(L, arg) != LUA_TTABLE)
        return luaL_argerror(L, arg, errstring(ERR_TABLE));

    lua_getfield(L, arg, "code");
    code = lua_tointegerx(L, -1, &isnum); lua_pop(L, 1);

    lua_getfield(L, arg, "format");
    format = testformat(L, -1, &err); lua_pop(L, 1);
    if(err<0) return luaL_argerror(L, arg, "invalid 'format'");

    lua_getfield(L, arg, "subformat");
    subformat = testsubformat(L, -1, &err); lua_pop(L, 1);
    if(err<0) return luaL_argerror(L, arg, "invalid 'subformat'");

    lua_getfield(L, arg, "endianness");
    endianness = testendianness(L, -1, &err); lua_pop(L, 1);
    if(err < 0) return luaL_argerror(L, arg, "invalid 'endianness'");/* defaults to 0 */

    if(isnum)
        {
        if(format && format != (code & SF_FORMAT_TYPEMASK))
            return luaL_argerror(L, arg, "format is present and does not match with code");
        if(subformat && subformat != (code & SF_FORMAT_SUBMASK))
            return luaL_argerror(L, arg, "subformat is present and does not match with code");
        if(endianness && endianness != (code & SF_FORMAT_ENDMASK))
            return luaL_argerror(L, arg, "subformat is present and does not match with code");
        p->format = code;
        }
    else
        {
        if(format==0)
            return luaL_argerror(L, arg, "missing format");
        if(subformat==0)
            return luaL_argerror(L, arg, "missing subformat");
        p->format = format | subformat | endianness;
        }

    lua_getfield(L, arg, "samplerate");
    p->samplerate = lua_tointegerx(L, -1, &isnum); lua_pop(L, 1);
    if(!isnum) return luaL_argerror(L, arg, "missing or invalid 'samplerate'");

    lua_getfield(L, arg, "channels");
    p->channels = lua_tointegerx(L, -1, &isnum); lua_pop(L, 1);
    if(!isnum) return luaL_argerror(L, arg, "missing or invalid 'channels'");

    return 0;
    }

#define FORMAT(format) ((format) & SF_FORMAT_TYPEMASK)
#define SUBFORMAT(format) ((format) & SF_FORMAT_SUBMASK)
#define ENDIANNESS(format) ((format) & SF_FORMAT_ENDMASK)

int pushinfo(lua_State *L, SF_INFO *p)
    {
    lua_newtable(L);
    lua_pushinteger(L, p->format); lua_setfield(L, -2, "code");
    pushformat(L, FORMAT(p->format)); lua_setfield(L, -2, "format");
    pushsubformat(L, SUBFORMAT(p->format)); lua_setfield(L, -2, "subformat");
    pushendianness(L, ENDIANNESS(p->format)); lua_setfield(L, -2, "endianness");
    lua_pushinteger(L, p->frames); lua_setfield(L, -2, "frames");
    lua_pushinteger(L, p->samplerate); lua_setfield(L, -2, "samplerate");
    lua_pushinteger(L, p->channels); lua_setfield(L, -2, "channels");
    lua_pushinteger(L, p->sections); lua_setfield(L, -2, "sections");
    lua_pushboolean(L, p->seekable); lua_setfield(L, -2, "seekable");
    return 1;
    }

static int GetSubFormats(lua_State *L, int format)
/* Expects a formatinfo on top of the stack, and sets the 'subformats' field */
    {
    SF_FORMAT_INFO info;
    SF_INFO sfinfo;
    int i, j, count;
    sf.command(NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &count, sizeof(count));
    lua_newtable(L);
    memset(&info, 0, sizeof(info));
    memset(&sfinfo, 0, sizeof(sfinfo));
    sfinfo.channels = 1;
    j = 0;
    for(i = 0; i < count; i++)
        {
        info.format = i;
        sf.command(NULL, SFC_GET_FORMAT_SUBTYPE, &info, sizeof(info));
        /* check that the combination fo format and subformat is valid */
        sfinfo.format = (format & SF_FORMAT_TYPEMASK) | info.format;
        if(sf.format_check(&sfinfo))
            {
            sf.command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info));
            pushsubformatinfo(L, &info);
            lua_seti(L, -2, ++j);
            }
        }
    return 0;   
    }

int pushformatinfo(lua_State *L, SF_FORMAT_INFO *p)
    {
    lua_newtable(L);
    pushformat(L, p->format); lua_setfield(L, -2, "format");
    lua_pushinteger(L, p->format); lua_setfield(L, -2, "code");
    lua_pushstring(L, p->name); lua_setfield(L, -2, "name");
    lua_pushstring(L, p->extension); lua_setfield(L, -2, "extension");
    GetSubFormats(L, p->format); lua_setfield(L, -2, "subformats"); 
    return 1;
    }

int pushsubformatinfo(lua_State *L, SF_FORMAT_INFO *p)
    {
    lua_newtable(L);
    pushsubformat(L, p->format); lua_setfield(L, -2, "subformat");
    lua_pushinteger(L, p->format); lua_setfield(L, -2, "code");
    lua_pushstring(L, p->name); lua_setfield(L, -2, "name");
    return 1;
    }

int pushsimpleformat(lua_State *L, SF_FORMAT_INFO *p)
    {
    lua_newtable(L);
    lua_pushinteger(L, p->format); lua_setfield(L, -2, "code");
    lua_pushstring(L, p->name); lua_setfield(L, -2, "name");
    lua_pushstring(L, p->extension); lua_setfield(L, -2, "extension");
    return 1;
    }

int freechunk(lua_State *L, chunknode_t *p)
    {
    if(p->chunk.data) Free(L, p->chunk.data);
    Free(L, p);
    return 0;
    }

chunknode_t *checkchunk(lua_State *L, int arg)
    {
    const char *id;
    const char *data;
    size_t id_size, datalen;
    chunknode_t *p;
    if(lua_isnoneornil(L, arg))
        { luaL_argerror(L, arg, errstring(ERR_NOTPRESENT)); return NULL; }
    if(lua_type(L, arg) != LUA_TTABLE)
        { luaL_argerror(L, arg, errstring(ERR_TABLE)); return NULL; }

    lua_getfield(L, arg, "id");
    id = luaL_checklstring(L, -1, &id_size);
    if(!id || id_size == 0 || id_size > 64)
        { lua_pop(L, 1); luaL_argerror(L, arg, "missing or invalid 'id' field"); return NULL; }
    lua_pop(L, 1);
    
    lua_getfield(L, arg, "data");
    data = luaL_optlstring(L, -1, NULL, &datalen);
    lua_pop(L, 1);
    
    p = (chunknode_t*)Malloc(L, sizeof(chunknode_t));
    if(id) memcpy(p->chunk.id, id, id_size);
    if(data && datalen > 0)
        {
        p->chunk.data = MallocNoErr(L, datalen);
        if(!p->chunk.data)
            { 
            freechunk(L, p);
            luaL_argerror(L, arg, errstring(ERR_MEMORY)); 
            return NULL;
            }
        memcpy(p->chunk.data, data, datalen);
        }
    return p;
    }

int pushchunk(lua_State *L, chunk_t *p)
    {
    lua_newtable(L);
    lua_pushlstring(L, p->id, p->id_size);
    lua_setfield(L, -2, "id");
    if(p->data && p->datalen > 0)
        {
        lua_pushlstring(L, (const char*)p->data, p->datalen);
        lua_setfield(L, -2, "data");
        }
    return 1;
    }


