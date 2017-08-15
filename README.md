## MoonSndFile: Lua bindings for libsndfile

MoonSndFile is a Lua binding library for Erik de Castro Lopo's [libsndfile](http://www.mega-nerd.com/libsndfile/).

It runs on GNU/Linux and on Windows (MSYS2/MinGW) and requires
[Lua](http://www.lua.org/) (>=5.3) and [libsndfile](http://www.mega-nerd.com/libsndfile/#Download) (>= 1.0.25).


_Author:_ _[Stefano Trettel](https://www.linkedin.com/in/stetre)_

[![Lua logo](./doc/powered-by-lua.gif)](http://www.lua.org/)

#### License

MIT/X11 license (same as Lua). See [LICENSE](./LICENSE).

#### Documentation

See the [Reference Manual](https://stetre.github.io/moonsndfile/doc/index.html).

#### Getting and installing

Setup the build environment as described [here](https://github.com/stetre/moonlibs), then:

```sh
$ git clone https://github.com/stetre/moonsndfile
$ cd moonsndfile
moonsndfile$ make
moonsndfile$ sudo make install
```

Note: that MoonSndFile does not link directly to the libsndfile library (`libsndfile.so` on Linux), 
but it builds dynamically its own internal dispatch tables instead.
As a consequence, `libsndfile.so` is not needed at compile time but it is required 
at runtime, so you have to make sure that it is reachable by the linker. 
You can do this by installing it in the standard search directories (e.g. `/usr/lib/`),
or by properly setting the LD_LIBRARY_PATH environment variable in the shell where you execute
the Lua scripts. 

#### Example

The example below is the Lua version of the _sfprocess.c_ example that comes with libsndfile-1.0.28.

Other examples can be found in the **examples/** directory contained in the release package.

```lua
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
```

The script can be executed at the shell prompt with the standard Lua interpreter:

```shell
$ lua hello.lua
```

#### See also

* [MoonLibs - Graphics and Audio Lua Libraries](https://github.com/stetre/moonlibs).
