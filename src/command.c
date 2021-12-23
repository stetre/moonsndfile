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

int command_void(lua_State *L, int command)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    sf.command(sndfile, command, NULL, 0);
    return 0;
    }

int command_set_count(lua_State *L, int command)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    sf_count_t frames = luaL_checkinteger(L, 2);    
    int res = sf.command(sndfile, command, &frames, sizeof(frames));
    lua_pushboolean(L, res == 0);
    return 1;
    }

int command_get_boolean(lua_State *L, int command)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int val = sf.command(sndfile, command, NULL, 0);
    lua_pushboolean(L, val);
    return 1;
    }

int command_set_boolean(lua_State *L, int command)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int val = checkboolean(L, 2);
    sf.command(sndfile, command, NULL, val);
    return 0;
    }

int command_get_integer(lua_State *L, int command)
    {
    int val;
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    if(sf.command(sndfile, command, &val, sizeof(val)) != 0)
        return sndfileerror(L, sndfile);
    lua_pushinteger(L, val);
    return 1;
    }

int command_set_integer(lua_State *L, int command)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int val = luaL_checkinteger(L, 2);
    if(sf.command(sndfile, command, &val, sizeof(val)) != SF_TRUE)
        return sndfileerror(L, sndfile);
    return 0;
    }

int command_get_double(lua_State *L, int command)
    {
    double val;
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    if(sf.command(sndfile, command, &val, sizeof(val)) != 0)
        return sndfileerror(L, sndfile);
    lua_pushnumber(L, val);
    return 1;
    }

int command_set_double(lua_State *L, int command)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    double val = luaL_checknumber(L, 2);
    if(sf.command(sndfile, command, &val, sizeof(val)) != SF_TRUE)
        return sndfileerror(L, sndfile);
    return 0;
    }

int command_get_double_all_channels(lua_State *L, int command, int success_retcode)
    {
    ud_t *ud;
    int i;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    size_t size = ud->sfinfo.channels * sizeof(double);
    double *val = (double*)Malloc(L, size);
    int res = sf.command(sndfile, command, val, size);
    if(res != success_retcode)
        return sndfileerror(L, sndfile);
    lua_newtable(L);
    for(i = 0; i < ud->sfinfo.channels; i++)
        {
        lua_pushnumber(L, val[i]);
        lua_rawseti(L, -2, i+1);
        }
    Free(L, val);
    return 1;
    }

/* --------- signal info ----------------------------------------------------*/

static int CalcSignalMax(lua_State *L)
    { return command_get_double(L, SFC_CALC_SIGNAL_MAX); }

static int CalcNormSignalMax(lua_State *L)
    { return command_get_double(L, SFC_CALC_NORM_SIGNAL_MAX); }

static int CalcMaxAllChannels(lua_State *L)
    { return command_get_double_all_channels(L, SFC_CALC_MAX_ALL_CHANNELS, 0); }

static int CalcNormMaxAllChannels(lua_State *L)
    { return command_get_double_all_channels(L, SFC_CALC_NORM_MAX_ALL_CHANNELS, 0); }

static int GetSignalMax(lua_State *L)
    { return command_get_double(L, SFC_GET_SIGNAL_MAX); }

static int GetMaxAllChannels(lua_State *L)
    { return command_get_double_all_channels(L, SFC_GET_MAX_ALL_CHANNELS, SF_TRUE); }


static int GetChannelMapInfo(lua_State *L)
    {
    ud_t *ud;
    int i;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    size_t size = ud->sfinfo.channels * sizeof(int);
    int *val = (int*)Malloc(L, size);
    int res = sf.command(sndfile, SFC_GET_CHANNEL_MAP_INFO, val, size);
    if(res == 0)
        return sndfileerror(L, sndfile);
    lua_newtable(L);
    for(i = 0; i < ud->sfinfo.channels; i++)
        {
        pushchannelmap(L, val[i]);
        lua_rawseti(L, -2, i+1);
        }
    Free(L, val);
    return 1;
    }

static int SetChannelMapInfo(lua_State *L)
    {
    ud_t *ud;
    size_t count, size;
    int err, res;
    sndfile_t sndfile = checksndfile(L, 1, &ud);
    int *list = checkchannelmaplist(L, 2, &count, &err);
    if(err)
        return luaL_argerror(L, 2, errstring(err));
    if(count != (size_t)ud->sfinfo.channels)
        {
        freechannelmaplist(L, list);
        return luaL_argerror(L, 2, errstring(ERR_LENGTH));
        }
    size = count * sizeof(int);
    res = sf.command(sndfile, SFC_SET_CHANNEL_MAP_INFO, list, size);
    freechannelmaplist(L, list);
    if(res == 0)
        return sndfileerror(L, sndfile);
    return 1;
    }

static int WavexGetAmbisonic(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int val = sf.command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0);
    if(val==0)
        return sndfileerror(L, sndfile);
    pushambisonic(L, val);
    return 1;
    }

static int WavexSetAmbisonic(lua_State *L)
    {
    sndfile_t sndfile = checksndfile(L, 1, NULL);
    int val = checkambisonic(L, 2);
    int res = sf.command(sndfile, SFC_WAVEX_SET_AMBISONIC, NULL, val);
    if(res == 0)
        return sndfileerror(L, sndfile);
    return 0;
    }

static int SetOriginalSamplerate(lua_State *L)
    { return command_set_integer(L, SFC_SET_ORIGINAL_SAMPLERATE); }

static int GetOriginalSamplerate(lua_State *L)
    { return command_get_integer(L, SFC_GET_ORIGINAL_SAMPLERATE); }

static const struct luaL_Reg Functions[] = 
    {
//      { "get_lib_version", GetLibVersion },
//      { "get_log_info", GetLogInfo }, @@FARE?
        { "calc_signal_max", CalcSignalMax },
        { "calc_norm_signal_max", CalcNormSignalMax },
        { "calc_max_all_channels", CalcMaxAllChannels },
        { "calc_norm_max_all_channels", CalcNormMaxAllChannels },
        { "get_signal_max", GetSignalMax },
        { "get_max_all_channels", GetMaxAllChannels },
//      { "set_add_header_pad_chunk", SetAddHeaderPadChunk }, not documented in libsndfile
//      { "set_dither_on_write", SetDitherOnWrite }, not documented in libsndfile @@FARE?
//      { "set_dither_on_read", SetDitherOnRead }, not documented in libsndfile @@FARE?
//      { "get_dither_info_count", GetDitherInfoCount }, not implemented in libsndfile
//      { "get_dither_info", GetDitherInfo }, not implemented in libsndfile
        { "get_channel_map_info", GetChannelMapInfo },
        { "set_channel_map_info", SetChannelMapInfo },
        { "wavex_set_ambisonic", WavexSetAmbisonic },
        { "wavex_get_ambisonic", WavexGetAmbisonic },
//      { "test_ieee_float_replace", TestIeeeFloatReplace }, not documented in libsndfile @@FARE?
        { "set_original_samplerate", SetOriginalSamplerate },
        { "get_original_samplerate", GetOriginalSamplerate },
        { NULL, NULL } /* sentinel */
    };

void moonsndfile_open_command(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }

