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

#define MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))

static uint32_t VerMajor, VerMinor, VerPatch;
static uint32_t Version;

int checkminversion(uint32_t major, uint32_t minor, uint32_t patch)
/* Checks that libsndfile.so Version is at least major.minor.patch */
    { return (Version >= MAKE_VERSION(major, minor, patch)); }

uint32_t sfversion(void)
    { return Version; }

static int AddVersions(lua_State *L)
    {
    int ec;
    const char *ver;

    lua_pushstring(L, "_VERSION");
    lua_pushstring(L, "MoonSndFile "MOONSNDFILE_VERSION);
    lua_settable(L, -3);

    ver = sf.version_string();
    if(!ver) return luaL_error(L, "cannot determine libsndfile version");
    ec = sscanf(ver, "libsndfile-%u.%u.%u", &VerMajor, &VerMinor, &VerPatch);
    Version = MAKE_VERSION(VerMajor, VerMinor, VerPatch);
    if(ec!=3) return luaL_error(L, "cannot determine libsndfile version");

    return 0;
    }

static int VersionString(lua_State *L)
    {
    lua_pushstring(L, sf.version_string());
    return 1;
    }

static int VersionNumbers(lua_State *L)
    {
    lua_pushinteger(L, VerMajor);
    lua_pushinteger(L, VerMinor);
    lua_pushinteger(L, VerPatch);
    return 3;
    }

static const struct luaL_Reg Functions[] =
    {
        { "version_string", VersionString },
        { "version_numbers", VersionNumbers },
        { NULL, NULL } /* sentinel */
    };

void moonsndfile_open_version(lua_State *L)
    {
    AddVersions(L);
    luaL_setfuncs(L, Functions, 0);
    }

