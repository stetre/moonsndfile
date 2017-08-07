#!/usr/bin/env lua
-- MoonSndFile example: sfprocess.lua
--
-- This is the Lua version of the examples/sfprocess.c 
-- example that comes with the -- libsndfile-1.0.28.tar.gz tarball.
--

sf = require('moonsndfile')

-- This will be the number of frames we'll request at each read:
NFRAMES = 64
-- A frame is composed of 1 sample per each channel contained in the audio file.
-- So, if for example we have 2 channels and samples of the double type, we will
-- retrieve (at most) 2*sf.sizeof('double')*NSAMPLES bytes of data at each read.

-- libsndfile can handle more than 6 channels but we'll restrict it to 6.
CHANNEL_GAIN = { 0.5, 0.8, 0.1, 0.4, 0.4, 0.9 }
MAX_CHANNELS = #CHANNEL_GAIN

-- The process data function receives a chunk of audio data and the number of
-- channels, processes it, and returns a new chunk of audio data.
-- The passed audio data must be a binary string encoding double audio samples.
function process_data(data, channels)
   -- Unpack the data binary string to a table of doubles:
   local data = sf.unpack('double', data)

   -- Process the data here.
   -- If the soundfile contains more then 1 channel you need to take care of
   -- the data interleaving youself.
   -- Current we just apply a channel dependant gain.
   for chan = 1, channels do
      for k = chan, #data, channels do
         data[k] = data[k] * CHANNEL_GAIN[chan]
      end
   end
   
   -- Pack the table of doubles to a binary string, and return it.
   return sf.pack('double', data)
end

-- The sf.open function, when it succeeds in opening the file, returns a sndfile object
-- and its updated fileinfo table (that corresponds to the SF_INFO C struct).
-- On error, it raises a Lua error() so there is no need to check the return values.
-- If don't want it to raise an error, you can wrap it with the Lua pcall() function
-- as shown in the code commented out below (search for 'pcall').

-- A fileinfo argument is passed to sf.open only if the openmode is 'w' (write) or
-- if the file format is 'raw'. In such cases, the fileinfo is needed in order to
-- specify the file format, the number of channels and the samplerate.
-- If topenmode is 'r' or 'rw', the file information is read from the file itself
-- (except for raw files).
infilename = "input.wav"
outfilename = "output.wav"

infile, fileinfo = sf.open(infilename, 'r')
--[[
ok, infile, fileinfo = pcall(sf.open, infilename, 'r')
if not ok then
   -- An error occurred. The infile variable contains the error message.
   print("Not able to open input file "..infilename..".")
   error(infile)
end
--]]

if fileinfo.channels > MAX_CHANNELS then
   error("Not able to process more than "..MAX_CHANNELS.. "channels")
end

-- Open the output file. We pass the fileinfo of the input file so that
-- the output file will have the same format and no. of channels:
outfile, fileinfo = sf.open(outfilename, 'w', fileinfo)

-- While there are frames in the input file, read them, process them and write
-- them to the output file.
while true do 
   local data, nframes = sf.read(infile, 'double', NFRAMES)
   if not data then break end
   data = process_data(data, fileinfo.channels)
   sf.write(outfile, 'double', data)
end

-- Close input and output files. 
-- This is really not needed if the program is exiting (as in this case)
-- because MoonSndFile automatically closes any sndfile that is still open.
sf.close(infile)
sf.close(outfile)

