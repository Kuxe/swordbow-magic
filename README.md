[![Stories in Ready](https://badge.waffle.io/kuxe/swordbow-magic.png?label=ready&title=Ready)](https://waffle.io/kuxe/swordbow-magic)
swordbow-magic
==============
A multiplayer 2D top-down game with procedurally generated worlds. Very much in-progress.

# Dependencies
Dependencies are installed locally (inside the repo, not in system-directories) by cmake. The build-system downloads the repositories using git and mercurial, so both git and mercurial need to be installed by you manually.

## Software
* C++14 compliant compiler
* CMake
* make
* git
* mercurial (hg)

## Libraries
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer
* OGG (as in Ogg Vorbis libraries)
* <a href=https://github.com/USCiLab/cereal>Cereal</a>

# Building
## Linux and OSX
You can build either a release build or debug build.
### Debug
1. Open up your favorite terminal and navigate to /swordbow-magic/\<You should be here>.
2. Type this: `mkdir debug && cd debug/ && cmake -DCMAKE_BUILD_TYPE=Debug .. && make setup build`
3. Two executables (client and server) is present inside /swordbow-magic/debug/\<executables should be here>

### Release
1. Open up your favorite terminal and navigate to /swordbow-magic/\<You should be here>.
2. Type this: `mkdir release && cd release/ && cmake -DCMAKE_BUILD_TYPE=Release .. && make setup build`
3. Two executables (client and server) is present inside /swordbow-magic/release/\<executables should be here>

## Windows
Not yet.

## Running
There can be some bootup-time for server if running with parameter bigworld. Be patient before connecting a client. There should be two executables in the same directory that `make build` was executed:
* swordbow-magic-server
* swordbow-magic-client

You have to start swordbow-magic-server before starting the client. Check source code <a href="https://github.com/Kuxe/swordbow-magic/blob/master/src/server.cpp">server.cpp</a> and <a href="https://github.com/Kuxe/swordbow-magic/blob/master/src/client.cpp">client.cpp</a> to figure out the parameters and how to specify server IP, port and client port etcetc... These change frequently so I don't bother explaining it here :).

## Additional notes
This is a hobby project which is only developed on master branch. It breaks from time to time.
