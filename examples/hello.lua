#!/usr/bin/env lua
-- MoonSndFile example: hello.lua

sf = require('moonsndfile')

NFRAMES = 64 -- no. of frames requested at each read
CHANNEL_GAIN = { 0.5, 0.8, 0.1, 0.4, 0.4, 0.9 }
MAX_CHANNELS = #CHANNEL_GAIN

function process_data(data, channels)
   -- Unpack the data binary string to a table of doubles:
   local data = sf.unpack('double', data)

   -- Process the data (apply a channel dependent gain):
   for chan = 1, channels do
      for k = chan, #data, channels do
         data[k] = data[k] * CHANNEL_GAIN[chan]
      end
   end
   
   -- Pack the table of doubles to a binary string, and return it:
   return sf.pack('double', data)
end

-- Open the input file:
infile, fileinfo = sf.open("input.wav", 'r')

if fileinfo.channels > MAX_CHANNELS then
   error("Not able to process more than "..MAX_CHANNELS.. "channels")
end

-- Open the output file, with the same properties as the input file:
outfile, fileinfo = sf.open("output.wav", 'w', fileinfo)

-- Read input frames / process them / write output frames:
while true do 
   local data, nframes = sf.read(infile, 'double', NFRAMES)
   if not data then break end
   data = process_data(data, fileinfo.channels)
   sf.write(outfile, 'double', data)
end

-- Close files:
sf.close(infile)
sf.close(outfile)

