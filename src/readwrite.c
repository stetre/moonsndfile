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

static int Seek(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    sf_count_t frames = luaL_checkinteger(L, 2);
    int whence = checkwhence(L, 3);
    sf_count_t offset = sf.seek(sndfile, frames, whence) ;
    if(offset == -1)
        return nil_sndfileerror(L, sndfile);
    lua_pushinteger(L, offset);
    return 1;
    }

static int WriteSync(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    sf.write_sync(sndfile);
    return 0;
    }

static int CurrentByterate(lua_State *L)
    {
    int val;
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    if(!sf.current_byterate) //CheckPfn(L, current_byterate, 1,0,26);
        return 0;
    val = sf.current_byterate(sndfile);
    if(val == -1) 
        return 0; /* unknown */
    lua_pushinteger(L, val);
    return 1;
    }

#define CHECK_SIZE_FUNC(T)                                                      \
static sf_count_t CheckSize_##T(lua_State *L, int arg, ud_t* ud, sf_count_t size)   \
    {                                                                           \
    if((size % (ud->sfinfo.channels * sizeof(T))) != 0)                         \
        return luaL_argerror(L, arg, "invalid size");                           \
    return size / (ud->sfinfo.channels * sizeof(T)); /* no. of frames */        \
    }

CHECK_SIZE_FUNC(short)
CHECK_SIZE_FUNC(int)
CHECK_SIZE_FUNC(float)
CHECK_SIZE_FUNC(double)


static int Read(lua_State *L)
    {
    ud_t *ud;
    size_t size;
    char *ptr;
    sf_count_t nframes;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    int t = checktype(L, 2);
    sf_count_t frames = luaL_checkinteger(L, 3);

    if(frames == 0)
        { lua_pushnil(L); lua_pushinteger(L, 0); return 2; }

    switch(t)
        {
        case NONSF_TYPE_SHORT:  size = frames * sizeof(short) * ud->sfinfo.channels;
                                ptr = (char*)Malloc(L, size);
                                nframes = sf.readf_short(sndfile, (short*)ptr, frames);
                                /* update size to account for cases where nframes < frames */
                                size = nframes * sizeof(short) * ud->sfinfo.channels;
                                break;
        case NONSF_TYPE_INT:    size = frames * sizeof(int) * ud->sfinfo.channels;
                                ptr = (char*)Malloc(L, size);
                                nframes = sf.readf_int(sndfile, (int*)ptr, frames);
                                size = nframes * sizeof(int) * ud->sfinfo.channels;
                                break;
        case NONSF_TYPE_FLOAT:  size = frames * sizeof(float) * ud->sfinfo.channels;
                                ptr = (char*)Malloc(L, size);
                                nframes = sf.readf_float(sndfile, (float*)ptr, frames);
                                size = nframes * sizeof(float) * ud->sfinfo.channels;
                                break;
        case NONSF_TYPE_DOUBLE: size = frames * sizeof(double) * ud->sfinfo.channels;
                                ptr = (char*)Malloc(L, size);
                                nframes = sf.readf_double(sndfile, (double*)ptr, frames);
                                size = nframes * sizeof(double) * ud->sfinfo.channels;
                                break;
        default:
            return luaL_argerror(L, 2, "invalid type");
        }

    if(nframes == 0) 
        lua_pushnil(L);
    else
        lua_pushlstring(L, ptr, size);
    lua_pushinteger(L, nframes);
    Free(L, ptr);
    return 2;
    }

static int Write(lua_State *L)
    {
    ud_t *ud;
    size_t size;
    sf_count_t frames, nframes = 0;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    int t = checktype(L, 2);
    const char *ptr = luaL_checklstring(L, 3, &size);
    switch(t)
        {
        case NONSF_TYPE_SHORT:  frames = CheckSize_short(L, 3, ud, size);
                                nframes = sf.writef_short(sndfile, (short*)ptr, frames);
                                break;
        case NONSF_TYPE_INT:    frames = CheckSize_int(L, 3, ud, size);
                                nframes = sf.writef_int(sndfile, (int*)ptr, frames);
                                break;
        case NONSF_TYPE_FLOAT:  frames = CheckSize_float(L, 3, ud, size);
                                nframes = sf.writef_float(sndfile, (float*)ptr, frames);
                                break;
        case NONSF_TYPE_DOUBLE: frames = CheckSize_double(L, 3, ud, size);
                                nframes = sf.writef_double(sndfile, (double*)ptr, frames);
                                break;
        default:
            return luaL_argerror(L, 2, "invalid type");
        }
    lua_pushinteger(L, nframes);
    lua_pushinteger(L, frames);
    return 2;
    }

/* ------------------------------------------------------------------------ */


static int SetClipping(lua_State *L)
    { return command_set_boolean(L, SFC_SET_CLIPPING); }

static int GetClipping(lua_State *L)
    { return command_get_boolean(L, SFC_GET_CLIPPING); }

static int GetNormDouble(lua_State *L)
    { return command_get_boolean(L, SFC_GET_NORM_DOUBLE); }

static int GetNormFloat(lua_State *L)
    { return command_get_boolean(L, SFC_GET_NORM_FLOAT); }

static int SetNormDouble(lua_State *L)
    { return command_set_boolean(L, SFC_SET_NORM_DOUBLE); }

static int SetNormFloat(lua_State *L)
    { return command_set_boolean(L, SFC_SET_NORM_FLOAT); }

static int SetScaleFloatIntRead(lua_State *L)
    { return command_set_boolean(L, SFC_SET_SCALE_FLOAT_INT_READ); }

static int SetScaleIntFloatWrite(lua_State *L)
    { return command_set_boolean(L, SFC_SET_SCALE_INT_FLOAT_WRITE); }

static int UpdateHeaderNow(lua_State *L)
    { return command_void(L, SFC_UPDATE_HEADER_NOW); }

static int SetUpdateHeaderAuto(lua_State *L)
    { return command_set_boolean(L, SFC_SET_UPDATE_HEADER_AUTO); }

static int FileTruncate(lua_State *L)
    { return command_set_count(L, SFC_FILE_TRUNCATE); }

static int Rf64AutoDowngrade(lua_State *L)
    { return command_set_boolean(L, SFC_RF64_AUTO_DOWNGRADE); }

static int SetVbrEncodingQuality(lua_State *L)
    { return command_set_double(L, SFC_SET_VBR_ENCODING_QUALITY); }

static int SetCompressionLevel(lua_State *L)
    { return command_set_double(L, SFC_SET_COMPRESSION_LEVEL); }

static int SetAddPeakChunk(lua_State *L)
    { return command_set_boolean(L, SFC_SET_ADD_PEAK_CHUNK); }

static int SetOggPageLatencyMs(lua_State *L)
    { return command_set_double(L, SFC_SET_OGG_PAGE_LATENCY_MS); }

static int SetBitrateMode(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int val = checkbitratemode(L, 2);
    sf.command(sndfile, SFC_SET_BITRATE_MODE, NULL, val);
    return 0;
    }

static int GetBitrateMode(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int val = sf.command(sndfile, SFC_GET_BITRATE_MODE, NULL, 0);
    pushbitratemode(L, val);
    return 1;
    }



/* ------------------------------------------------------------------------ */

#if 0 //(defined (ENABLE_SNDFILE_WINDOWS_PROTOTYPES) && ENABLE_SNDFILE_WINDOWS_PROTOTYPES)
SNDFILE* sf.wchar_open (LPCWSTR wpath, int mode, SF_INFO *sfinfo) ;
#endif

static const struct luaL_Reg Methods[] = 
    {
        { "seek", Seek },
        { "write_sync", WriteSync },
        { "read", Read },
        { "write", Write },
        { NULL, NULL } /* sentinel */
    };


static const struct luaL_Reg Functions[] = 
    {
        { "seek", Seek },
        { "write_sync", WriteSync },
        { "current_byterate", CurrentByterate },
        { "read", Read },
        { "write", Write },
        { "set_clipping", SetClipping },
        { "get_clipping", GetClipping },
        { "get_norm_double", GetNormDouble },
        { "get_norm_float", GetNormFloat },
        { "set_norm_double", SetNormDouble },
        { "set_norm_float", SetNormFloat },
        { "set_scale_float_int_read", SetScaleFloatIntRead },
        { "set_scale_int_float_write", SetScaleIntFloatWrite },
        { "file_truncate", FileTruncate },
        { "update_header_now", UpdateHeaderNow },
        { "set_update_header_auto", SetUpdateHeaderAuto },
        { "rf64_auto_downgrade", Rf64AutoDowngrade },
        { "set_vbr_encoding_quality", SetVbrEncodingQuality },
        { "set_compression_level", SetCompressionLevel },
        { "set_add_peak_chunk", SetAddPeakChunk },
        { "set_ogg_page_latency_ms", SetOggPageLatencyMs },
        { "set_bitrate_mode", SetBitrateMode },
        { "get_bitrate_mode", GetBitrateMode },
        { NULL, NULL } /* sentinel */
    };


void moonsndfile_open_readwrite(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    udata_addmethods(L, SNDFILE_MT, Methods);
    }

