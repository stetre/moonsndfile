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

static int ReadRaw(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    size_t bytes = luaL_checkinteger(L, 2);
    char *ptr = (char*)Malloc(L, bytes);
    sf_count_t nbytes = sf.read_raw(sndfile, ptr, bytes);
    lua_pushlstring(L, ptr, nbytes);
    Free(L, ptr);
    return 1;
    }

static int WriteRaw(lua_State *L)
    {
    size_t bytes;
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    const char *ptr = luaL_checklstring(L, 2, &bytes);
    sf_count_t nbytes = sf.write_raw(sndfile, ptr, bytes);
    lua_pushinteger(L, nbytes);
    return 1;
    }

static int SetRawStartOffset(lua_State *L)
    { return command_set_count(L, SFC_SET_RAW_START_OFFSET); }

static int RawDataNeedsEndswap(lua_State *L)
    { return command_get_boolean(L, SFC_RAW_DATA_NEEDS_ENDSWAP); }

static const struct luaL_Reg Functions[] = 
    {
        { "read_raw", ReadRaw },
        { "write_raw", WriteRaw },
        { "set_raw_start_offset", SetRawStartOffset },
        { "raw_data_needs_endswap", RawDataNeedsEndswap },
        { NULL, NULL } /* sentinel */
    };

void moonsndfile_open_rawreadwrite(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

