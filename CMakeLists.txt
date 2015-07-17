CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

PROJECT(swordbow-magic)

#Binary should be in root folder
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

INCLUDE(FindPkgConfig)

#Flags for compiler and linker
SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread -std=c++14")
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#Create a library containing game-functionality
#game-executable and test-executable uses this library
#Want to re-use *.o files in both game-exec and test-exec...
ADD_LIBRARY(game SHARED
    src/animationsystem.cpp
    src/attacksystem.cpp
    src/camerasystem.cpp
    src/collisionsystem.cpp
    src/commandcomponent.cpp
    src/componentmanager.cpp
    src/deltatime.cpp
    src/entitymanager.cpp
    src/hashgridsystem.cpp
    src/healthsystem.cpp
    src/idmanager.cpp
    src/inputsystem.cpp
    src/keyboardsystem.cpp
    src/movecomponent.cpp
    src/movediffsystem.cpp
    src/movesystem.cpp
    src/noisebrush.cpp
    src/playsound.cpp
    src/rect.cpp
    src/removesystem.cpp
    src/renderdiffsystem.cpp
    src/renderer.cpp
    src/rendersystem.cpp
    src/servernetworksystem.cpp
    src/sizeboundingbox.cpp
    src/soundengine.cpp
    src/systemmanager.cpp
    src/text.cpp
    src/textureboundingbox.cpp
    src/world.cpp
    )

#Find SDL2 dependencies
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)
INCLUDE_DIRECTORIES(./include/ ./catch/ ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(game ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})

#Find Boost dependency
FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(game ${Boost_LIBRARIES})

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-client src/client.cpp)

#What linker flags are required for swordbow-magic-client?
TARGET_LINK_LIBRARIES(swordbow-magic-client game)

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-server src/server.cpp)

#What linker flags are required for swordbow-magic-server?
TARGET_LINK_LIBRARIES(swordbow-magic-server game)

#Create a test-executable using Catch
ADD_EXECUTABLE(swordbow-magic-test
    catch/src/config.cpp
    catch/src/hashgridsystem_test.cpp
    catch/src/rect_test.cpp
    catch/src/rendersystem_test.cpp
)

#What linker flags are required for swordbow-magic-test?
TARGET_LINK_LIBRARIES(swordbow-magic-test game)

#Run "make buildclient" to build the client
ADD_CUSTOM_TARGET(buildclient
    DEPENDS swordbow-magic-client
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

#Run "make buildserver" to build the server
ADD_CUSTOM_TARGET(buildserver
    DEPENDS swordbow-magic-server
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

#Run "make build" to build client and server
ADD_CUSTOM_TARGET(build
    DEPENDS swordbow-magic-client
    DEPENDS swordbow-magic-server
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(rum
    COMMAND echo Yarr!
)
