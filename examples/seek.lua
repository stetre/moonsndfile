#!/usr/bin/env lua
-- MoonSndFile example: seek.lua
--
-- Experiments with sf.seek()

sf = require("moonsndfile")

f, info= sf.open("input.wav", "rw")
if not f then error(errmsg) end

function seek(frames, whence) 
   local offset, errmsg = sf.seek(f, frames, whence)
   io.write(string.format("seek(%d, '%s') --> ", frames, whence))
   if errmsg then
      io.write(string.format("error (%s)\n", errmsg))
   else
      io.write(string.format("%d\n", offset))
   end
end

print("No. of frames: "..info.frames)
seek(0, "cur r") --> current read position
seek(1, "cur r") --> current read position
seek(2, "cur r") --> current read position
seek(0, "end")
seek(-1, "end")
seek(10, "end") --> not error, but we're going beyond the file...
seek(-10, "set") --> error
seek(-10, "cur")
seek(0, "cur") --> current position
seek(0, "cur w") --> current write position
seek(10, "set r")
seek(20, "set r")
seek(0, "cur r") --> current read position
seek(0, "cur w") --> current write position

