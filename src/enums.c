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

/*------------------------------------------------------------------------------*
 | Code<->string map for enumerations                                           |
 *------------------------------------------------------------------------------*/


/* code <-> string record */
#define rec_t struct rec_s
struct rec_s {
    RB_ENTRY(rec_s) CodeEntry;
    RB_ENTRY(rec_s) StringEntry;
    uint32_t domain;
    uint32_t code;  /* (domain, code) = search key in code tree */
    char     *str;  /* (domain, str) = search key in string tree */
};

/* compare functions */
static int cmp_code(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return (rec1->code < rec2->code ? -1 : rec1->code > rec2->code);
    } 

static int cmp_str(rec_t *rec1, rec_t *rec2) 
    { 
    if(rec1->domain != rec2->domain)
        return (rec1->domain < rec2->domain ? -1 : rec1->domain > rec2->domain);
    return strcmp(rec1->str, rec2->str);
    } 

static RB_HEAD(CodeTree, rec_s) CodeHead = RB_INITIALIZER(&CodeHead);
RB_PROTOTYPE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 
RB_GENERATE_STATIC(CodeTree, rec_s, CodeEntry, cmp_code) 

static RB_HEAD(StringTree, rec_s) StringHead = RB_INITIALIZER(&StringHead);
RB_PROTOTYPE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
RB_GENERATE_STATIC(StringTree, rec_s, StringEntry, cmp_str) 
 
static rec_t *code_remove(rec_t *rec) 
    { return RB_REMOVE(CodeTree, &CodeHead, rec); }
static rec_t *code_insert(rec_t *rec) 
    { return RB_INSERT(CodeTree, &CodeHead, rec); }
static rec_t *code_search(uint32_t domain, uint32_t code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_FIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_first(uint32_t domain, uint32_t code) 
    { rec_t tmp; tmp.domain = domain; tmp.code = code; return RB_NFIND(CodeTree, &CodeHead, &tmp); }
static rec_t *code_next(rec_t *rec)
    { return RB_NEXT(CodeTree, &CodeHead, rec); }
#if 0
static rec_t *code_prev(rec_t *rec)
    { return RB_PREV(CodeTree, &CodeHead, rec); }
static rec_t *code_min(void)
    { return RB_MIN(CodeTree, &CodeHead); }
static rec_t *code_max(void)
    { return RB_MAX(CodeTree, &CodeHead); }
static rec_t *code_root(void)
    { return RB_ROOT(&CodeHead); }
#endif
 
static rec_t *str_remove(rec_t *rec) 
    { return RB_REMOVE(StringTree, &StringHead, rec); }
static rec_t *str_insert(rec_t *rec) 
    { return RB_INSERT(StringTree, &StringHead, rec); }
static rec_t *str_search(uint32_t domain, const char* str) 
    { rec_t tmp; tmp.domain = domain; tmp.str = (char*)str; return RB_FIND(StringTree, &StringHead, &tmp); }
#if 0
static rec_t *str_first(uint32_t domain, const char* str ) 
    { rec_t tmp; tmp.domain = domain; tmp.str = str; return RB_NFIND(StringTree, &StringHead, &tmp); }
static rec_t *str_next(rec_t *rec)
    { return RB_NEXT(StringTree, &StringHead, rec); }
static rec_t *str_prev(rec_t *rec)
    { return RB_PREV(StringTree, &StringHead, rec); }
static rec_t *str_min(void)
    { return RB_MIN(StringTree, &StringHead); }
static rec_t *str_max(void)
    { return RB_MAX(StringTree, &StringHead); }
static rec_t *str_root(void)
    { return RB_ROOT(&StringHead); }
#endif


static int enums_new(lua_State *L, uint32_t domain, uint32_t code, const char *str)
    {
    rec_t *rec;
    if((rec = (rec_t*)Malloc(L, sizeof(rec_t))) == NULL) 
        return luaL_error(L, errstring(ERR_MEMORY));

    memset(rec, 0, sizeof(rec_t));
    rec->domain = domain;
    rec->code = code;
    rec->str = Strdup(L, str);
    if(code_search(domain, code) || str_search(domain, str))
        { 
        Free(L, rec->str);
        Free(L, rec); 
        return unexpected(L); /* duplicate value */
        }
    code_insert(rec);
    str_insert(rec);
    return 0;
    }

static void enums_free(lua_State *L, rec_t* rec)
    {
    if(code_search(rec->domain, rec->code) == rec)
        code_remove(rec);
    if(str_search(rec->domain, rec->str) == rec)
        str_remove(rec);
    Free(L, rec->str);
    Free(L, rec);   
    }

void enums_free_all(lua_State *L)
    {
    rec_t *rec;
    while((rec = code_first(0, 0)))
        enums_free(L, rec);
    }

#if 0
uint32_t enums_code(uint32_t domain, const char *str, int* found)
    {
    rec_t *rec = str_search(domain, str);
    if(!rec)
        { *found = 0; return 0; }
    *found = 1; 
    return rec->code;
    }

const char* enums_string(uint32_t domain, uint32_t code)
    {
    rec_t *rec = code_search(domain, code);
    if(!rec)
        return NULL;
    return rec->str;
    }

#endif


uint32_t enums_test(lua_State *L, uint32_t domain, int arg, int *err)
    {
    rec_t *rec;
    const char *s = luaL_optstring(L, arg, NULL);

    if(!s)
        { *err = ERR_NOTPRESENT; return 0; }

    rec = str_search(domain, s);
    if(!rec)
        { *err = ERR_VALUE; return 0; }
    
    *err = ERR_SUCCESS;
    return rec->code;
    }

uint32_t enums_check(lua_State *L, uint32_t domain, int arg)
    {
    rec_t *rec;
    const char *s = luaL_checkstring(L, arg);

    rec = str_search(domain, s);
    if(!rec)
        return luaL_argerror(L, arg, badvalue(L, s));
    
    return rec->code;
    }

int enums_push(lua_State *L, uint32_t domain, uint32_t code)
    {
    rec_t *rec = code_search(domain, code);

    if(!rec)
        return unexpected(L);

    lua_pushstring(L, rec->str);
    return 1;
    }

int enums_values(lua_State *L, uint32_t domain)
    {
    int i;
    rec_t *rec;

    lua_newtable(L);
    i = 1;
    rec = code_first(domain, 0);
    while(rec)
        {
        if(rec->domain == domain)
            {
            lua_pushstring(L, rec->str);
            lua_rawseti(L, -2, i++);
            }
        rec = code_next(rec);
        }

    return 1;
    }


int* enums_checklist(lua_State *L, uint32_t domain, int arg, size_t *count, int *err)
    {
    int* list;
    size_t i;

    *count = 0;
    *err = 0;
    if(lua_isnoneornil(L, arg))
        { *err = ERR_NOTPRESENT; return NULL; }
    if(lua_type(L, arg) != LUA_TTABLE)
        { *err = ERR_TABLE; return NULL; }

    *count = luaL_len(L, arg);
    if(*count == 0)
        { *err = ERR_NOTPRESENT; return NULL; }

    list = (int*)MallocNoErr(L, sizeof(int) * (*count));
    if(!list)
        { *count = 0; *err = ERR_MEMORY; return NULL; }

    for(i=0; i<*count; i++)
        {
        lua_rawgeti(L, arg, i+1);
        list[i] = enums_test(L, domain, -1, err);
        lua_pop(L, 1);
        if(*err)
            { Free(L, list); *count = 0; return NULL; }
        }
    return list;
    }

void enums_freelist(lua_State *L, int *list)
    {
    if(!list)
        return;
    Free(L, list);
    }

/*------------------------------------------------------------------------------*
 |                                                                              |
 *------------------------------------------------------------------------------*/

static int Enum(lua_State *L)
/* { strings } = cl.enum('type') lists all the values for a given enum type */
    { 
    const char *s = luaL_checkstring(L, 1); 
#define CASE(xxx) if(strcmp(s, ""#xxx) == 0) return values##xxx(L)
    CASE(type);
    CASE(format);
    CASE(subformat);
    CASE(endianness);
    CASE(stringtype);
    CASE(whence);
    CASE(openmode);
    CASE(channelmap);
    CASE(ditherlevel);
    CASE(dithertype);
    CASE(loopmode);
    CASE(ambisonic);
    CASE(bitratemode);
#undef CASE
    return 0;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "enum", Enum },
        { NULL, NULL } /* sentinel */
    };


void moonsndfile_open_enums(lua_State *L)
    {
    uint32_t domain;

    luaL_setfuncs(L, Functions, 0);

    /* Add all the code<->string mappings and the cl.XXX constant strings */
#define ADD(what, s) do { enums_new(L, domain, NONSF_##what, s); } while(0)
    domain = DOMAIN_NONSF_TYPE; 
    ADD(TYPE_SHORT, "short");
    ADD(TYPE_INT, "int");
    ADD(TYPE_FLOAT, "float");
    ADD(TYPE_DOUBLE, "double");
#undef ADD

#define ADD(what, s) do {                               \
    lua_pushstring(L, s); lua_setfield(L, -2, #what);   \
    enums_new(L, domain, SF_##what, s);                 \
} while(0)

#define ADDX(what, s) do {                              \
    lua_pushstring(L, s); lua_setfield(L, -2, #what);   \
    enums_new(L, domain, what, s);                      \
} while(0)

    domain = DOMAIN_FORMAT;
    ADD(FORMAT_WAV, "wav");
    ADD(FORMAT_AIFF, "aiff");
    ADD(FORMAT_AU, "au");
    ADD(FORMAT_RAW, "raw");
    ADD(FORMAT_PAF, "paf");
    ADD(FORMAT_SVX, "svx");
    ADD(FORMAT_NIST, "nist");
    ADD(FORMAT_VOC, "voc");
    ADD(FORMAT_IRCAM, "ircam");
    ADD(FORMAT_W64, "w64");
    ADD(FORMAT_MAT4, "mat4");
    ADD(FORMAT_MAT5, "mat5");
    ADD(FORMAT_PVF, "pvf");
    ADD(FORMAT_XI, "xi");
    ADD(FORMAT_HTK, "htk");
    ADD(FORMAT_SDS, "sds");
    ADD(FORMAT_AVR, "avr");
    ADD(FORMAT_WAVEX, "wavex");
    ADD(FORMAT_SD2, "sd2");
    ADD(FORMAT_FLAC, "flac");
    ADD(FORMAT_CAF, "caf");
    ADD(FORMAT_WVE, "wve");
    ADD(FORMAT_OGG, "ogg");
    ADD(FORMAT_MPC2K, "mpc2k");
    ADD(FORMAT_RF64, "rf64");
    ADD(FORMAT_MPEG, "mpeg");

    domain = DOMAIN_SUBFORMAT;
    ADD(FORMAT_PCM_S8, "pcm s8");
    ADD(FORMAT_PCM_16, "pcm 16");
    ADD(FORMAT_PCM_24, "pcm 24");
    ADD(FORMAT_PCM_32, "pcm 32");
    ADD(FORMAT_PCM_U8, "pcm u8");
    ADD(FORMAT_FLOAT, "float");
    ADD(FORMAT_DOUBLE, "double");
    ADD(FORMAT_ULAW, "ulaw");
    ADD(FORMAT_ALAW, "alaw");
    ADD(FORMAT_IMA_ADPCM, "ima adpcm");
    ADD(FORMAT_MS_ADPCM, "ms adpcm");
    ADD(FORMAT_GSM610, "gsm610");
    ADD(FORMAT_VOX_ADPCM, "vox adpcm");
    ADD(FORMAT_G721_32, "g721 32");
    ADD(FORMAT_G723_24, "g723 24");
    ADD(FORMAT_G723_40, "g723 40");
    ADD(FORMAT_DWVW_12, "dwvw 12");
    ADD(FORMAT_DWVW_16, "dwvw 16");
    ADD(FORMAT_DWVW_24, "dwvw 24");
    ADD(FORMAT_DWVW_N, "dwvw n");
    ADD(FORMAT_DPCM_8, "dpcm 8");
    ADD(FORMAT_DPCM_16, "dpcm 16");
    ADD(FORMAT_VORBIS, "vorbis");
    ADD(FORMAT_ALAC_16, "alac 16");
    ADD(FORMAT_ALAC_20, "alac 20");
    ADD(FORMAT_ALAC_24, "alac 24");
    ADD(FORMAT_ALAC_32, "alac 32");
    ADD(FORMAT_NMS_ADPCM_16, "nms adpcm 16");
    ADD(FORMAT_NMS_ADPCM_24, "nms adpcm 24");
    ADD(FORMAT_NMS_ADPCM_32, "nms adpcm 32");
    ADD(FORMAT_OPUS, "opus");
    ADD(FORMAT_MPEG_LAYER_I, "mpeg layer i");
    ADD(FORMAT_MPEG_LAYER_II, "mpeg layer ii");
    ADD(FORMAT_MPEG_LAYER_III, "mpeg layer iii");

    domain = DOMAIN_ENDIANNESS;
    ADD(ENDIAN_FILE, "file");
    ADD(ENDIAN_LITTLE, "little");
    ADD(ENDIAN_BIG, "big");
    ADD(ENDIAN_CPU, "cpu");

    domain = DOMAIN_STRING_TYPE;
    ADD(STR_TITLE, "title");
    ADD(STR_COPYRIGHT, "copyright");
    ADD(STR_SOFTWARE, "software");
    ADD(STR_ARTIST, "artist");
    ADD(STR_COMMENT, "comment");
    ADD(STR_DATE, "date");
    ADD(STR_ALBUM, "album");
    ADD(STR_LICENSE, "license");
    ADD(STR_TRACKNUMBER, "tracknumber");
    ADD(STR_GENRE, "genre");

    domain = DOMAIN_WHENCE;
#define SF_SEEK_SET     SEEK_SET
#define SF_SEEK_CUR     SEEK_CUR
#define SF_SEEK_END     SEEK_END
#define SF_SEEK_SET_R   SEEK_SET | SFM_READ
#define SF_SEEK_CUR_R   SEEK_CUR | SFM_READ
#define SF_SEEK_END_R   SEEK_END | SFM_READ
#define SF_SEEK_SET_W   SEEK_SET | SFM_WRITE
#define SF_SEEK_CUR_W   SEEK_CUR | SFM_WRITE
#define SF_SEEK_END_W   SEEK_END | SFM_WRITE
    ADD(SEEK_SET, "set");
    ADD(SEEK_CUR, "cur");
    ADD(SEEK_END, "end");
    ADD(SEEK_SET_R, "set r");
    ADD(SEEK_CUR_R, "cur r");
    ADD(SEEK_END_R, "end r");
    ADD(SEEK_SET_W, "set w");
    ADD(SEEK_CUR_W, "cur w");
    ADD(SEEK_END_W, "end w");

    domain = DOMAIN_OPEN_MODE;
    ADDX(SFM_READ, "r");
    ADDX(SFM_WRITE, "w");
    ADDX(SFM_RDWR, "rw");

    domain = DOMAIN_CHANNEL_MAP;
    ADD(CHANNEL_MAP_INVALID, "invalid");
    ADD(CHANNEL_MAP_MONO, "mono");
    ADD(CHANNEL_MAP_LEFT, "left");
    ADD(CHANNEL_MAP_RIGHT, "right");
    ADD(CHANNEL_MAP_CENTER, "center");
    ADD(CHANNEL_MAP_FRONT_LEFT, "front left");
    ADD(CHANNEL_MAP_FRONT_RIGHT, "front right");
    ADD(CHANNEL_MAP_FRONT_CENTER, "front center");
    ADD(CHANNEL_MAP_REAR_CENTER, "rear center");
    ADD(CHANNEL_MAP_REAR_LEFT, "rear left");
    ADD(CHANNEL_MAP_REAR_RIGHT, "rear right");
    ADD(CHANNEL_MAP_LFE, "lfe");
    ADD(CHANNEL_MAP_FRONT_LEFT_OF_CENTER, "front left of center");
    ADD(CHANNEL_MAP_FRONT_RIGHT_OF_CENTER, "front right of center");
    ADD(CHANNEL_MAP_SIDE_LEFT, "side left");
    ADD(CHANNEL_MAP_SIDE_RIGHT, "side right");
    ADD(CHANNEL_MAP_TOP_CENTER, "top center");
    ADD(CHANNEL_MAP_TOP_FRONT_LEFT, "top front left");
    ADD(CHANNEL_MAP_TOP_FRONT_RIGHT, "top front right");
    ADD(CHANNEL_MAP_TOP_FRONT_CENTER, "top front center");
    ADD(CHANNEL_MAP_TOP_REAR_LEFT, "top rear left");
    ADD(CHANNEL_MAP_TOP_REAR_RIGHT, "top rear right");
    ADD(CHANNEL_MAP_TOP_REAR_CENTER, "top rear center");
    ADD(CHANNEL_MAP_AMBISONIC_B_W, "ambisonic b w");
    ADD(CHANNEL_MAP_AMBISONIC_B_X, "ambisonic b x");
    ADD(CHANNEL_MAP_AMBISONIC_B_Y, "ambisonic b y");
    ADD(CHANNEL_MAP_AMBISONIC_B_Z, "ambisonic b z");

    domain = DOMAIN_DITHER_LEVEL;
    ADDX(SFD_DEFAULT_LEVEL, "default");
    ADDX(SFD_CUSTOM_LEVEL, "custom");

    domain = DOMAIN_DITHER_TYPE;
    ADDX(SFD_NO_DITHER, "no dither");
    ADDX(SFD_WHITE, "white");
    ADDX(SFD_TRIANGULAR_PDF, "triangular pdf");

    domain = DOMAIN_LOOP_MODE;
    ADD(LOOP_NONE, "none");
    ADD(LOOP_FORWARD, "forward");
    ADD(LOOP_BACKWARD, "backward");
    ADD(LOOP_ALTERNATING, "alternating");

    domain = DOMAIN_AMBISONIC;
    ADD(AMBISONIC_NONE, "none");
    ADD(AMBISONIC_B_FORMAT, "b format");

    domain = DOMAIN_BITRATE_MODE;
    ADD(BITRATE_MODE_CONSTANT, "constant");
    ADD(BITRATE_MODE_AVERAGE, "average");
    ADD(BITRATE_MODE_VARIABLE, "variable");

#if 0 /* scaffolding */
4yy
#define DOMAIN_
    domain = DOMAIN_;
    ADD(, "");

#endif

#undef ADD
#undef ADDX
    }


