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
    
static int Type(lua_State *L)
    {
#define TRY(xxx) do { if(test##xxx(L, 1, NULL) != 0) { lua_pushstring(L, ""#xxx); return 1; } } while(0)
    TRY(sndfile);
    return 0;
#undef TRY
    }

int trace_objects = 0;

static int TraceObjects(lua_State *L)
    {
    trace_objects = checkboolean(L, 1);
    return 0;
    }

#if 0
static int Now(lua_State *L)
    {
    lua_pushnumber(L, now());
    return 1;
    }

static int Since(lua_State *L)
    {
    double t = luaL_checknumber(L, 1);
    lua_pushnumber(L, since(t));
    return 1;
    }

static int Sleep(lua_State *L)
    {
    double seconds = luaL_checknumber(L, 1);
    sleeep(seconds);
    return 0;
    }
#endif

/* ----------------------------------------------------------------------- */

static const struct luaL_Reg Functions[] = 
    {
        { "type", Type },
        { "trace_objects", TraceObjects },
#if 0
        { "now", Now },
        { "since", Since },
        { "sleep", Sleep },
#endif
        { NULL, NULL } /* sentinel */
    };

void moonsndfile_open_tracing(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


