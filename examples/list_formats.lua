#!/usr/bin/env lua
-- MoonSndFile example: list_formats.lua
--
-- This is the Lua version of the examples/list_formats.c 
-- example that comes with the -- libsndfile-1.0.28.tar.gz tarball.
--

sf = require('moonsndfile')

local function printf(...) io.write(string.format(...)) end

-- List formats:

formats = sf.get_format_list()

print("\nFound "..#formats.." major formats:")

for i, info in ipairs(formats) do
   printf("%2d: '%s' '%s' .%s [0x%.8x]\n", i, info.format, info.name, info.extension, info.code)
   for j, subinfo in ipairs(info.subformats) do
      printf("   %2d: '%s' '%s' [0x%.8x]\n", j, subinfo.subformat, subinfo.name, subinfo.code)
   end
end

-- List simpleformats:

simpleformats = sf.get_simple_format_list()

print("\nFound "..#simpleformats.." simple formats:")
for i, info in ipairs(simpleformats) do
   printf("%2d: '%s' .%s [0x%.8x]\n", i, info.name, info.extension, info.code)
end

