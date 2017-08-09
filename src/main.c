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

static lua_State *moonsndfile_L = NULL;

static void AtExit(void)
    {
    if(moonsndfile_L)
        {
        enums_free_all(moonsndfile_L);
        moonsndfile_atexit_getproc();
        moonsndfile_L = NULL;
        }
    }

int luaopen_moonsndfile(lua_State *L)
/* Lua calls this function to load the module */
    {
    moonsndfile_L = L;

    moonsndfile_utils_init(L);
    atexit(AtExit);

    lua_newtable(L); /* the cl table */

    /* add functions: */
    moonsndfile_open_getproc(L);
    moonsndfile_open_version(L);
    moonsndfile_open_enums(L);
    moonsndfile_open_tracing(L);
    moonsndfile_open_sndfile(L);
    moonsndfile_open_readwrite(L);
    moonsndfile_open_rawreadwrite(L);
    moonsndfile_open_datahandling(L);
    moonsndfile_open_command(L);
    moonsndfile_open_format(L);
    moonsndfile_open_chunk(L);

#if 0 //@@
    /* Add functions implemented in Lua */
    lua_pushvalue(L, -1); lua_setglobal(L, "moonsndfile");
    if(luaL_dostring(L, "require('moonsndfile.utils')") != 0) lua_error(L);
    lua_pushnil(L);  lua_setglobal(L, "moonsndfile");
#endif

    return 1;
    }

