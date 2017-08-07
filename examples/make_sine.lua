#!/usr/bin/env lua
-- MoonSndFile example: make_sine.lua
--
-- This is the Lua version of the examples/make_sine.c 
-- example that comes with the -- libsndfile-1.0.28.tar.gz tarball.
--

sf = require('moonsndfile')

pi = math.pi
sin = math.sin
floor = math.floor

SAMPLE_RATE = 44100 -- hz
SAMPLE_COUNT = SAMPLE_RATE * 4 -- 4 seconds
AMPLITUDE = 1.0 * 0x7F000000
LEFT_FREQ = 344.0 * 2 * pi / SAMPLE_RATE
RIGHT_FREQ = 466.0 * 2 * pi / SAMPLE_RATE

-- Open file to be written:
f, sfinfo = sf.open("sine.wav", "w", {
      format = "wav", 
      subformat = "pcm 24",
      samplerate = SAMPLE_RATE,
      frames     = SAMPLE_COUNT,
      channels      = 2
})

-- Prepare the sinusoid(s):
buffer = {}
if sfinfo.channels == 1 then
   for k = 1, SAMPLE_COUNT do
      buffer[k] = floor(AMPLITUDE * sin (LEFT_FREQ * (k-1)))
   end
elseif sfinfo.channels == 2 then
   for k = 1, SAMPLE_COUNT do
      buffer[2*k - 1] = floor(AMPLITUDE * sin (LEFT_FREQ * (k-1)))
      buffer[2*k] = floor(AMPLITUDE * sin (RIGHT_FREQ * (k-1)))
   end
else
   error("make_sine can only generate mono or stereo files")
end

-- Convert data table to a binary string of C integers:
data = sf.pack('int', buffer)
-- assert(#data == SAMPLE_COUNT * sfinfo.channels * sf.sizeof('int'))

-- Write data to the file:
nframes, frames = sf.write(f, 'int', data)
assert(nframes == frames)

-- Close file (unneeded at exit, actually)
sf.write_sync(f)
sf.close(f)

