[![Stories in Ready](https://badge.waffle.io/kuxe/swordbow-magic.png?label=ready&title=Ready)](https://waffle.io/kuxe/swordbow-magic)
swordbow-magic
==============
A multiplayer 3D first-person game with procedurally generated worlds. Very much in-progress.

# Dependencies
Dependencies are installed locally (inside the repo, not in system-directories) by cmake. The build-system downloads the repositories using git. Currently swordbow-magic is transitioning from 2D graphics to 3D graphics. Because of this, swordbow-magic compiles but there is no graphics, input or sound present any longer. These features will be back soon™.

## Software
* C++14 compliant compiler
* CMake
* make
* git

## Libraries
* <a href=https://github.com/Taywee/args>args</a>
* <a https://github.com/philsquared/Catch>Catch</a>
* <a href=https://github.com/USCiLab/cereal>Cereal</a>
* <a http://glm.g-truc.net/0.9.8/index.html>glm</a>
* <a https://github.com/soundcloud/ogg>Ogg</a>
* Soon: https://github.com/kuxe/Subber>Subber</a>
* Soon: lowpoly3d (still private, will make it public soon)

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
You can build either a release build or debug build.
### Debug
1. Open up PowerShell and navigate to /swordbow-magic/\<You should be here>.
2. Type this: `mkdir debug; cd debug; cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..; make setup build`
3. Two executables (client and server) is present inside /swordbow-magic/debug/\<executables should be here>

### Release
1. Open PowerShell and navigate to /swordbow-magic/\<You should be here>.
2. Type this: `mkdir debug; cd debug; cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..; make setup build`
3. Two executables (client and server) is present inside /swordbow-magic/release/\<executables should be here>

## Running
There can be some bootup-time for server if running with parameter bigworld. Be patient before connecting a client. There should be two executables in the same directory that `make build` was executed:
* swordbow-magic-server
* swordbow-magic-client

You have to start swordbow-magic-server before starting the client. Check source code <a href="https://github.com/Kuxe/swordbow-magic/blob/master/src/server.cpp">server.cpp</a> and <a href="https://github.com/Kuxe/swordbow-magic/blob/master/src/client.cpp">client.cpp</a> to figure out the parameters and how to specify server IP, port and client port etcetc... These change frequently so I don't bother explaining it here :).

## Additional notes
This is a hobby project which is only developed on master branch. It breaks from time to time.
