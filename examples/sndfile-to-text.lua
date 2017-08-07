#!/usr/bin/env lua
-- MoonSndFile example: sndfile-to-text.lua
--
-- This is the Lua version of the examples/sndfile-to-text.c 
-- example that comes with the -- libsndfile-1.0.28.tar.gz tarball.
--

sf = require('moonsndfile')

fmt = string.format

NFRAMES = 64

function convert_to_text (infile, outfile, channels, precision)
   local nframes = NFRAMES
   local FMT = " %12.10f" -- default output format
   if precision then FMT = " %."..precision.."e" end -- e.g " %.8e"
      
   while true do
      local data, nframes = sf.read(infile, 'float', nframes)
      if not data then break end
      local samples = sf.unpack('float', data)
      for f = 1, nframes do
         for c = 1, channels do
            outfile:write(fmt(FMT, samples[(f-1)*channels + c]))
         end
         outfile:write("\n")
      end
   end
end

progname = arg[0]
USAGE = string.format("\n\nUsage : %s <input file> <output file> [precision]"..
"\nWhere the output file will contain a line for each frame and a column for each channel.\n\n",
progname)

infilename = arg[1]
outfilename = arg[2]
precision = arg[3]
assert(infilename, "missing input file" .. USAGE)
assert(outfilename, "missing output file" .. USAGE)
assert(infilename ~= outfilename, "input and output are the same"..USAGE)
if precision then
   precision = math.tointeger(precision)
   assert(precision and precision >= 0, "precision must be a non negative integer"..USAGE)
end

infile, sfinfo = sf.open(infilename, 'r')
outfile, errmsg = io.open(outfilename, "w")
assert(outfile, errmsg)

outfile:write(fmt("# Converted from file %s.\n", infilename))
outfile:write(fmt("# Channels %d, Sample rate %d\n", sfinfo.channels, sfinfo.samplerate))

convert_to_text (infile, outfile, sfinfo.channels, precision)

sf.close(infile)
outfile:close(outfile)

