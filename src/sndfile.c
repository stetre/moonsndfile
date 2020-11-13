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

static int freesndfile(lua_State *L, ud_t *ud)
    {
    int ec;
    sndfile_t sndfile = (sndfile_t)ud->handle;
    chunknode_t *chunklist = ud->chunklist;
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(sndfile, "sndfile");
    ec = sf.close(sndfile);
    freechunks(L, chunklist);
    if(ec != 0) 
        return luaL_error(L, sf.error_number(ec));
    return 0;
    }

static int Create(lua_State *L, sndfile_t sndfile, const SF_INFO *info)
    {
    ud_t *ud;
    ud = newuserdata(L, sndfile, SNDFILE_MT);
    ud->parent_ud = NULL;
    ud->destructor = freesndfile;
    memcpy(&ud->sfinfo, info, sizeof(SF_INFO));
    TRACE_CREATE(sndfile, "sndfile");
    pushinfo(L, &ud->sfinfo);
    return 2;
    }

static int Open(lua_State *L, int openfd)
    {
    SF_INFO info;
    sndfile_t sndfile;
    const char *path;
    luaL_Stream *s;
    int mode, fd = -1, close_desc = 1;

    if(openfd) //@@FIXME: does this work under MINGW?
        {
        s = (luaL_Stream*)luaL_testudata(L, 1, LUA_FILEHANDLE);
        if(s)
            {
            fd = fileno(s->f); /* POSIX */
            if(fd == -1)
                return luaL_argerror(L, 1, errstring(ERR_TYPE));
            }
        else
            fd = luaL_checkinteger(L, 1);

        mode = checkopenmode(L, 2);
        close_desc = optboolean(L, 4, 0);
        }
    else
        {
        path = luaL_checkstring(L, 1);
        mode = checkopenmode(L, 2);
        }

    /* openmode="w" or format="raw" require the user to specify format, channels and samplerate.
     * openmode="r", the info is obtained from the file (provided it exists)
     * openmode="rw", the info is obtained from the file (provided it exists)
     * @@ if openmode="rw" and the file does not exist, then a new file is created
     *    but an error is raised because info.format is 0.
     */
    memset(&info, 0, sizeof(info));
    if(!lua_isnoneornil(L, 3))
        {
        checkinfo(L, 3, &info);
        if((((info.format & SF_FORMAT_TYPEMASK) != SF_FORMAT_RAW)) && (mode != SFM_WRITE))
            return luaL_argerror(L, 3, "info must be specified only for format='raw' or openmode ='w'");
//      if(!sf.format_check(&info))
//          return luaL_argerror(L, 3, "invalid combination of info values");
        }
    else if(mode == SFM_WRITE)
        return luaL_argerror(L, 3, "missing info, required for openmode ='w'");

    if(openfd)
        sndfile = sf.open_fd(fd, mode, &info, close_desc);
    else    
        sndfile = sf.open(path, mode, &info);

    if(!sndfile)
        return sndfileerror(L, NULL);

    return Create(L, sndfile, &info);
    }

static int OpenPath(lua_State *L)
    { return Open(L, 0); }

static int OpenFd(lua_State *L)
    { return Open(L, 1); }

static int SetString(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int str_type = checkstringtype(L, 2);
    const char *str = luaL_checkstring(L, 3);
    if(sf.set_string(sndfile, str_type, str))
        return sndfileerror(L, sndfile);
    return 0;
    }

static int GetString(lua_State *L)
    {
    int str_type = checkstringtype(L, 2);
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    const char *str = sf.get_string(sndfile, str_type);
    if(!str) return 0;
    lua_pushstring(L, str);
    return 1;
    }

int update_sfinfo(lua_State *L, sndfile_t sndfile, ud_t *ud)
    {
    (void)L;
    sf.command(sndfile, SFC_GET_CURRENT_SF_INFO, &ud->sfinfo, sizeof(SF_INFO));
    return 0;
    }

static int GetFileinfo(lua_State *L)
    {
    ud_t *ud;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    update_sfinfo(L, sndfile, ud);
    pushinfo(L, &ud->sfinfo);
    return 1;
    }

static int GetFormat(lua_State *L)
    {
    ud_t *ud;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    update_sfinfo(L, sndfile, ud);
    pushformat(L, ud->sfinfo.format & SF_FORMAT_TYPEMASK);
    return 1;
    }

static int GetSubformat(lua_State *L)
    {
    ud_t *ud;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    update_sfinfo(L, sndfile, ud);
    pushsubformat(L, ud->sfinfo.format & SF_FORMAT_SUBMASK);
    return 1;
    }

static int GetEndianness(lua_State *L)
    {
    ud_t *ud;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    update_sfinfo(L, sndfile, ud);
    pushendianness(L, ud->sfinfo.format & SF_FORMAT_ENDMASK);
    return 1;
    }

#define GETINT_FUNC(Func, what)         \
static int Func(lua_State *L)           \
    {                                   \
    ud_t *ud;                           \
    sndfile_t sndfile = checksndfile(L, 1, &ud);    \
    update_sfinfo(L, sndfile, ud);                  \
    lua_pushinteger(L, ud->sfinfo.what);            \
    return 1;                                       \
    }

GETINT_FUNC(GetCode, format)
GETINT_FUNC(GetFrames, frames)
GETINT_FUNC(GetSamplerate, samplerate)
GETINT_FUNC(GetChannels, channels)
GETINT_FUNC(GetSections, sections)
#undef GETINT_FUNC

static int GetSeekable(lua_State *L)
    {
    ud_t *ud;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    update_sfinfo(L, sndfile, ud);
    lua_pushboolean(L, ud->sfinfo.seekable);
    return 1;
    }


static int GetEmbedFileInfo (lua_State *L)
    {
    SF_EMBED_FILE_INFO info;
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    if(sf.command(sndfile, SFC_GET_EMBED_FILE_INFO, &info, sizeof(info)) != 0)
        return sndfileerror(L, sndfile);
    lua_pushinteger(L, info.offset);
    lua_pushinteger(L, info.length);
    return 2;
    }

RAW_FUNC(sndfile)
TYPE_FUNC(sndfile)
PARENT_FUNC(sndfile)
DELETE_FUNC(sndfile)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "close", Delete },
        { "fileinfo", GetFileinfo },
        { "format", GetFormat },
        { "subformat", GetSubformat },
        { "endianness", GetEndianness },
        { "code", GetCode },
        { "frames", GetFrames },
        { "samplerate", GetSamplerate },
        { "channels", GetChannels },
        { "sections", GetSections },
        { "seekable", GetSeekable },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "open", OpenPath },
        { "open_fd", OpenFd },
//      { "open_virtual", OpenVirtual }, @@ sf_open_virtual() not supported yet
        { "close", Delete },
        { "set_string", SetString },
        { "get_string", GetString },
        { "get_fileinfo", GetFileinfo },
        { "get_embed_file_info", GetEmbedFileInfo },
        { NULL, NULL } /* sentinel */
    };


void moonsndfile_open_sndfile(lua_State *L)
    {
    udata_define(L, SNDFILE_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

