#!/usr/bin/env lua
-- MoonSndFile example: generate.lua
--
-- This is the Lua version of the examples/generate.c 
-- example that comes with the -- libsndfile-1.0.28.tar.gz tarball.
--
sf = require('moonsndfile')

function main()
   if not arg[1] then
      print(string.format("\nEncode a single input file into a number of different output encodings."..
            "\nThese output encodings can then be moved to another OS for testing."..
            "\n\nUsage : %s <filename>\n", arg[0]))
      os.exit(true)
   end

   -- A couple of standard WAV files. Make sure Win32 plays these.
   encode_file(arg[1], "pcmu8.wav", 'wav', 'pcm u8')
   encode_file(arg[1], "pcm16.wav", 'wav', 'pcm 16')
   encode_file(arg[1], "imaadpcm.wav", 'wav', 'ms adpcm')
   encode_file(arg[1], "msadpcm.wav", 'wav', 'ima adpcm')
   --encode_file(arg[1], "gsm610.wav" , 'wav', 'gsm610') --@@ invalid format not detected by sf.format_check()?

   -- Soundforge W64.
   encode_file(arg[1], "pcmu8.w64", 'w64', 'pcm u8')
   encode_file(arg[1], "pcm16.w64", 'w64', 'pcm 16')
   encode_file(arg[1], "imaadpcm.w64", 'w64', 'ms adpcm')
   encode_file(arg[1], "msadpcm.w64", 'w64', 'ima adpcm')
   --encode_file(arg[1], "gsm610.w64", 'w64', 'gsm610')
end


function put_dots(k)
   repeat
      io.write('.')
      k = k - 1
   until k == 0
   io.write(' ')
end

function encode_file(infilename, outfilename, format, subformat)
   io.write(   "    "..infilename.." -> "..outfilename)
   io.flush()

   put_dots(16 - #outfilename)

   
   local infile, sfinfo = sf.open(infilename, 'r')

   sfinfo.code = nil -- set to nil otherwise sf.open() would take the format from here
   sfinfo.format = format
   sfinfo.subformat = subformat

   if not sf.format_check(format, subformat) then
      sf.close(infile)
      print("Invalid encoding")
      return
   end

   local outfile, sfinfo = sf.open(outfilename, 'w', sfinfo)

   local nframes = 64
   while true do
      local data, nframes = sf.read(infile, 'float', nframes)
      if not data then break end
      sf.write(outfile, 'float', data)
   end

   sf.close(infile)
   sf.close(outfile)

   io.write("ok\n")
end

main()

