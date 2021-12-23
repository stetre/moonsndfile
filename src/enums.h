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

#ifndef enumsDEFINED
#define enumsDEFINED

/* enums.c */
#define enums_free_all moonsndfile_enums_free_all
void enums_free_all(lua_State *L);
#define enums_test moonsndfile_enums_test
uint32_t enums_test(lua_State *L, uint32_t domain, int arg, int *err);
#define enums_check moonsndfile_enums_check
uint32_t enums_check(lua_State *L, uint32_t domain, int arg);
#define enums_push moonsndfile_enums_push
int enums_push(lua_State *L, uint32_t domain, uint32_t code);
#define enums_values moonsndfile_enums_values
int enums_values(lua_State *L, uint32_t domain);
#define enums_checklist moonsndfile_enums_checklist
int* enums_checklist(lua_State *L, uint32_t domain, int arg, size_t *count, int *err);
#define enums_freelist moonsndfile_enums_freelist
void enums_freelist(lua_State *L, int *list);

/* Enum domains */
#define DOMAIN_RESULT           0
#define DOMAIN_FORMAT           1 /* format major type */
#define DOMAIN_SUBFORMAT        2 /* format subtype */
#define DOMAIN_ENDIANNESS       3 /* format endianness */
#define DOMAIN_STRING_TYPE      4
#define DOMAIN_WHENCE           5
#define DOMAIN_OPEN_MODE        6
#define DOMAIN_CHANNEL_MAP      7
#define DOMAIN_DITHER_LEVEL     8
#define DOMAIN_DITHER_TYPE      9
#define DOMAIN_LOOP_MODE        10
#define DOMAIN_AMBISONIC        11
#define DOMAIN_BITRATE_MODE     12

/* NONSF additions */
#define DOMAIN_NONSF_TYPE                  101

/* Types for al.sizeof() & friends */
#define NONSF_TYPE_SHORT        1
#define NONSF_TYPE_INT          2
#define NONSF_TYPE_FLOAT        3
#define NONSF_TYPE_DOUBLE       4


#define testtype(L, arg, err) (int)enums_test((L), DOMAIN_NONSF_TYPE, (arg), (err))
#define checktype(L, arg) (int)enums_check((L), DOMAIN_NONSF_TYPE, (arg))
#define pushtype(L, val) enums_push((L), DOMAIN_NONSF_TYPE, (uint32_t)(val))
#define valuestype(L) enums_values((L), DOMAIN_NONSF_TYPE)

#define testformat(L, arg, err) (int)enums_test((L), DOMAIN_FORMAT, (arg), (err))
#define checkformat(L, arg) (int)enums_check((L), DOMAIN_FORMAT, (arg))
#define pushformat(L, val) enums_push((L), DOMAIN_FORMAT, (uint32_t)(val))
#define valuesformat(L) enums_values((L), DOMAIN_FORMAT)

#define testsubformat(L, arg, err) (int)enums_test((L), DOMAIN_SUBFORMAT, (arg), (err))
#define checksubformat(L, arg) (int)enums_check((L), DOMAIN_SUBFORMAT, (arg))
#define pushsubformat(L, val) enums_push((L), DOMAIN_SUBFORMAT, (uint32_t)(val))
#define valuessubformat(L) enums_values((L), DOMAIN_SUBFORMAT)

#define testendianness(L, arg, err) (int)enums_test((L), DOMAIN_ENDIANNESS, (arg), (err))
#define checkendianness(L, arg) (int)enums_check((L), DOMAIN_ENDIANNESS, (arg))
#define pushendianness(L, val) enums_push((L), DOMAIN_ENDIANNESS, (uint32_t)(val))
#define valuesendianness(L) enums_values((L), DOMAIN_ENDIANNESS)

#define teststringtype(L, arg, err) (int)enums_test((L), DOMAIN_STRING_TYPE, (arg), (err))
#define checkstringtype(L, arg) (int)enums_check((L), DOMAIN_STRING_TYPE, (arg))
#define pushstringtype(L, val) enums_push((L), DOMAIN_STRING_TYPE, (uint32_t)(val))
#define valuesstringtype(L) enums_values((L), DOMAIN_STRING_TYPE)

#define testwhence(L, arg, err) (int)enums_test((L), DOMAIN_WHENCE, (arg), (err))
#define checkwhence(L, arg) (int)enums_check((L), DOMAIN_WHENCE, (arg))
#define pushwhence(L, val) enums_push((L), DOMAIN_WHENCE, (uint32_t)(val))
#define valueswhence(L) enums_values((L), DOMAIN_WHENCE)

#define testopenmode(L, arg, err) (int)enums_test((L), DOMAIN_OPEN_MODE, (arg), (err))
#define checkopenmode(L, arg) (int)enums_check((L), DOMAIN_OPEN_MODE, (arg))
#define pushopenmode(L, val) enums_push((L), DOMAIN_OPEN_MODE, (uint32_t)(val))
#define valuesopenmode(L) enums_values((L), DOMAIN_OPEN_MODE)

#define testchannelmap(L, arg, err) (int)enums_test((L), DOMAIN_CHANNEL_MAP, (arg), (err))
#define checkchannelmap(L, arg) (int)enums_check((L), DOMAIN_CHANNEL_MAP, (arg))
#define pushchannelmap(L, val) enums_push((L), DOMAIN_CHANNEL_MAP, (uint32_t)(val))
#define valueschannelmap(L) enums_values((L), DOMAIN_CHANNEL_MAP)
#define checkchannelmaplist(L, arg, count, err) enums_checklist((L), DOMAIN_CHANNEL_MAP, (arg), (count), (err))
#define freechannelmaplist(L, list) enums_freelist((L), list)

#define testditherlevel(L, arg, err) (int)enums_test((L), DOMAIN_DITHER_LEVEL, (arg), (err))
#define checkditherlevel(L, arg) (int)enums_check((L), DOMAIN_DITHER_LEVEL, (arg))
#define pushditherlevel(L, val) enums_push((L), DOMAIN_DITHER_LEVEL, (uint32_t)(val))
#define valuesditherlevel(L) enums_values((L), DOMAIN_DITHER_LEVEL)

#define testdithertype(L, arg, err) (int)enums_test((L), DOMAIN_DITHER_TYPE, (arg), (err))
#define checkdithertype(L, arg) (int)enums_check((L), DOMAIN_DITHER_TYPE, (arg))
#define pushdithertype(L, val) enums_push((L), DOMAIN_DITHER_TYPE, (uint32_t)(val))
#define valuesdithertype(L) enums_values((L), DOMAIN_DITHER_TYPE)

#define testloopmode(L, arg, err) (int)enums_test((L), DOMAIN_LOOP_MODE, (arg), (err))
#define checkloopmode(L, arg) (int)enums_check((L), DOMAIN_LOOP_MODE, (arg))
#define pushloopmode(L, val) enums_push((L), DOMAIN_LOOP_MODE, (uint32_t)(val))
#define valuesloopmode(L) enums_values((L), DOMAIN_LOOP_MODE)

#define testambisonic(L, arg, err) (int)enums_test((L), DOMAIN_AMBISONIC, (arg), (err))
#define checkambisonic(L, arg) (int)enums_check((L), DOMAIN_AMBISONIC, (arg))
#define pushambisonic(L, val) enums_push((L), DOMAIN_AMBISONIC, (uint32_t)(val))
#define valuesambisonic(L) enums_values((L), DOMAIN_AMBISONIC)

#define testbitratemode(L, arg, err) (int)enums_test((L), DOMAIN_BITRATE_MODE, (arg), (err))
#define checkbitratemode(L, arg) (int)enums_check((L), DOMAIN_BITRATE_MODE, (arg))
#define pushbitratemode(L, val) enums_push((L), DOMAIN_BITRATE_MODE, (uint32_t)(val))
#define valuesbitratemode(L) enums_values((L), DOMAIN_BITRATE_MODE)

#if 0 /* scaffolding 8yy */
#define testxxx(L, arg, err) (Xxx)enums_test((L), DOMAIN_XXX, (arg), (err))
#define checkxxx(L, arg) (Xxx)enums_check((L), DOMAIN_XXX, (arg))
#define pushxxx(L, val) enums_push((L), DOMAIN_XXX, (uint32_t)(val))
#define valuesxxx(L) enums_values((L), DOMAIN_XXX)
#define checkxxxlist(L, arg, count, err) (Xxx*)enums_checklist((L), DOMAIN_XXX, (arg), (count), (err))
#define freexxxlist(L, list) enums_freelist((L), (uint32_t*)(list))
    CASE(xxx);

#endif

#endif /* enumsDEFINED */


