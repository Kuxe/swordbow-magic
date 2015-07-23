[![Stories in Ready](https://badge.waffle.io/kuxe/swordbow-magic.png?label=ready&title=Ready)](https://waffle.io/kuxe/swordbow-magic)
swordbow-magic
==============
A multiplayer 2D top-down game with procedurally generated worlds. Very much in-progress.

# Dependencies
Before building you need to make sure some dependencies are installed, namely some software and libraries:

## Software
* C++14 compliant compiler
* CMake
* make

## Libraries
* SDL2
* SDL2_image
* SDL2_ttf
* SDL2_mixer
* OGG (as in Ogg Vorbis libraries)
* <a href=https://github.com/USCiLab/cereal>Cereal</a>

If these are not installed, the build wil fail. CMake will tell you what dependencies are missing when running the `cmake ..` step as described bellow

# Building
## Linux and OSX
1. Open up your favorite terminal and navigate to /swordbow-magic/\<You should be here>.
2. Type this: `mkdir build && cd build/ && cmake .. && make build`
3. A executable "swordbow-magic" is present inside /swordbow-magic/\<executable should be here>

## Windows (untested)
1. Open up your favorite terminal and navigate to \\swordbow-magic\\\<You should be here>.
2. Type these: `mkdir build` + `cd .\build` + `cmake -G "MinGW Makefiles"..` + `make build`
3. A executable "swordbow-magic" is present inside \\swordbow-magic\\\<executable should be here>

I don't know if it builds at all on Windows or what steps must be taken for CMake and make to be succesful.

## Running
Navigate to git root, after a successful build there should be two executables:

* swordbow-magic-server
* swordbow-magic-client

You have to start swordbow-magic-server before starting the client. Check source code <a href="https://github.com/Kuxe/swordbow-magic/blob/master/src/server.cpp">server.cpp</a> and <a href="https://github.com/Kuxe/swordbow-magic/blob/master/src/client.cpp">client.cpp</a> to figure out the parameters and how to specify server IP, port and client port etcetc... These change frequently so I don't bother explaining it here :).

## Additional notes
This is a hobby project which is only developed on master branch. It breaks from time to time.
