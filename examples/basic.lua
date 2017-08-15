#!/usr/bin/env lua
-- MoonSndFile example: basic.lua
--
-- Basic example that shows how to open sound files and write/read
-- audio data to/from them. Also shows how to pack/unpack data.
-- Note that most of the assert() in the example are not really needed,
-- they are there just to highlight facts of interest.

sf = require("moonsndfile")

FILENAME = "rand.wav"
NFRAMES = 64
NCHANNELS = 2

function printinfo(fileinfo)
-- Quick and dirty utility to print the fileinfo fields:
   print("fileinfo:")
   for k, v in pairs(fileinfo) do print(k, v) end
end

-------------------------------------------------------------------------------
-- 1) Writing audio data to a sound file
-------------------------------------------------------------------------------

print("\nOpening '"..FILENAME.."' in write mode.")
-- When opening a file in write mode, we must pass a fileinfo to specify
-- the format, the number of channels, and the sampling rate:
f, fileinfo = sf.open(FILENAME, "w", {
   format = 'wav',
   subformat = 'pcm 24',
   channels = NCHANNELS,
   samplerate = 44100,
})

-- Generate some random data to fill the file with:
samples = {}
math.randomseed(os.time())
for i= 1, NFRAMES do 
   for i= 1, NCHANNELS do 
   samples[#samples+1] = (math.random() - 0.5)*2
   end
end
-- Convert the table of numbers to a binary string encoding them as doubles
-- (MoonSndFiles provides the sf.pack() utility for this purpose):
data = sf.pack('double', samples)
assert(#data == NFRAMES * NCHANNELS * sf.sizeof('double')) -- got it?

-- Now write the data, get the updated file info, and print it:
sf.write(f, 'double', data)
fileinfo = sf.get_fileinfo(f)
printinfo(fileinfo)
assert(fileinfo.frames == NFRAMES) -- right?

-- Close the file. We will open it below in read mode to retrieve the data.
sf.close(f)

-------------------------------------------------------------------------------
-- 2) Reading audio data from a sound file
-------------------------------------------------------------------------------

print("\nOpening '"..FILENAME.."' in read mode.")
-- When opening a file in read mode, libsndfile retrieves the file information
-- from the file itself (unless it is a 'raw' format file), so there is no need
-- to pass the fileinfo argument (actually, it would cause an error...).
f, fileinfo = sf.open(FILENAME, "r")
printinfo(fileinfo)

-- Read from the file the frames we wrote before. 
data1, nframes = sf.read(f, 'double', NFRAMES)
assert(nframes == NFRAMES) -->OK?

-- The data is returned as a binary string. Convert it to a table of numbers:
samples1 = sf.unpack('double', data1)
assert(#samples1 == #samples) -- == NFRAMES * NCHANNELS

-- Now we would like the retrieved samples to be the same as those we wrote,
-- but there is a catch: conversion errors. That is, those samples were first 
-- converted from doubles to the internal format of the file when writing, and
-- then back to doubles when reading. We cannot expect them to be exactly equal.
-- So instead of checking for equality (the check would fail) we compute the
-- maximum error between the samples we wrote and the corresponding samples we
-- read, and we just print it.
err = {}
for i = 1, #samples do 
   err[i] = math.abs(samples[i] - samples1[i])
end
maxerr = math.max(table.unpack(err))
print("max roundtrip conversion error = "..maxerr) -- is it small enough?

sf.close(f)

--[[ @@ Currently works only under GNU/Linux
-------------------------------------------------------------------------------
-- 3) Loading a sound file from an already open file (sf.open_fd())
-------------------------------------------------------------------------------
-- The following shows how to use sf.open_fd() combined with the standard Lua io.
-- Using sf.open_fd() we can load the sound file from an already open file.
-- sf.open_fd() accepts either an integer file descriptor or, more conveniently,
-- a Lua file handle returned by io.open(), as showwn here.
-- (MoonSndFile will extract the fd from the Lua file handle).
print("\nOpening '"..FILENAME.."' from file handle.")
fh = io.open(FILENAME, "r") 
f, fileinfo = sf.open_fd(fh, "r", nil, true)
printinfo(fileinfo)
sf.close(f)
--]]

