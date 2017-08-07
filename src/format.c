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

/* --------- formats --------------------------------------------------------*/

static int FormatCheck(lua_State *L)
/* boolean = sf.format_check(format, subformat) */
    {
    SF_INFO sfinfo;
    int format = checkformat(L, 1);
    int subformat = checksubformat(L, 2);
    
    memset(&sfinfo, 0, sizeof(sfinfo));
    sfinfo.channels = 1;
    sfinfo.format = (format & SF_FORMAT_TYPEMASK) | subformat;
    lua_pushboolean(L, sf.format_check(&sfinfo));
    return 1;
    }

static int GetFormatInfo(lua_State *L)
    {
    SF_FORMAT_INFO info;
    int format = checkformat(L, 1);
    memset(&info, 0, sizeof(info));
    info.format = format;
    sf.command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info));
    pushformatinfo(L, &info);
    return 1;
    }

static int GetSubFormatInfo(lua_State *L)
    {
    SF_FORMAT_INFO info;
    int subformat = checksubformat(L, 1);
    memset(&info, 0, sizeof(info));
    info.format = subformat;
    sf.command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info));
    pushsubformatinfo(L, &info);
    return 1;
    }

static int GetSimpleFormatList(lua_State *L)
    {
    SF_FORMAT_INFO info;
    int i, count;
    sf.command(NULL, SFC_GET_SIMPLE_FORMAT_COUNT, &count, sizeof(count));
    lua_newtable(L);
    for(i = 0; i < count; i++) 
        {
        info.format = i;
        sf.command(NULL, SFC_GET_SIMPLE_FORMAT, &info, sizeof(info));
        pushsimpleformat(L, &info);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }

static int GetFormatList(lua_State *L)
    {
    SF_FORMAT_INFO info;
    int i, count;
    sf.command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof(count));

    lua_newtable(L);
    memset(&info, 0, sizeof(info));
    for(i = 0; i < count; i++) 
        {
        info.format = i;
        sf.command(NULL, SFC_GET_FORMAT_MAJOR, &info, sizeof(info));
        sf.command(NULL, SFC_GET_FORMAT_INFO, &info, sizeof(info));
        pushformatinfo(L, &info);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }


static const struct luaL_Reg Functions[] = 
    {
        { "format_check", FormatCheck },
        { "get_simple_format_list", GetSimpleFormatList },
        { "get_format_list", GetFormatList },
        { "get_format_info", GetFormatInfo },
        { "get_subformat_info", GetSubFormatInfo },
        { NULL, NULL } /* sentinel */
    };


void moonsndfile_open_format(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

