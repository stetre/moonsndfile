#!/usr/bin/env lua
-- MoonSndFile example: version.lua
sf = require("moonsndfile")

print("MoonSndFile version: "..sf._VERSION)
print("libsndfile.so version: "..sf.version_string())
major, minor, patch = sf.version_numbers()
print("libsndfile.so version major: "..major)
print("libsndfile.so version minor: "..minor)
print("libsndfile.so version patch: "..patch)

