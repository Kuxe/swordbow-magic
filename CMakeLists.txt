cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(swordbow-magic)

#Binary should be in root folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

INCLUDE(FindPkgConfig)

#Find SDL2 dependencies
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)

#Flags for compiler and linker
SET(CMAKE_CXX_COMPILER "clang++")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14")

#What include directories are there?..:
include_directories(./include/ ./catch/ ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})

#Create a library containing game-functionality
#game-executable and test-executable uses this library
#Want to re-use *.o files in both game-exec and test-exec...
add_library(game SHARED
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
    src/movesystem.cpp
    src/noisebrush.cpp
    src/playsound.cpp
    src/rect.cpp
    src/removesystem.cpp
    src/rendersystem.cpp
    src/sizeboundingbox.cpp
    src/soundsystem.cpp
    src/systemmanager.cpp
    src/text.cpp
    src/textureboundingbox.cpp
    src/world.cpp
    )

TARGET_LINK_LIBRARIES(game ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})

#Create a game-executable
add_executable(swordbow-magic src/main.cpp)

#What linker flags are required for swordbow-magic?
TARGET_LINK_LIBRARIES(swordbow-magic game)

#Create a test-executable using Catch
add_executable(swordbow-magic-test
    catch/src/config.cpp
    catch/src/hashgridsystem_test.cpp
    catch/src/rect_test.cpp
    catch/src/rendersystem_test.cpp
)

#What linker flags are required for swordbow-magic-test?
TARGET_LINK_LIBRARIES(swordbow-magic-test game)

#Run "make run" to run the binary
add_custom_target(run
    COMMAND swordbow-magic
    DEPENDS swordbow-magic
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(rum
    COMMAND echo Yarr!
)

add_custom_target(profile
    COMMAND rm -f ./callgrind.out.* && valgrind --tool=callgrind ./swordbow-magic && kcachegrind callgrind.out.*
    DEPENDS swordbow-magic
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

install(TARGETS swordbow-magic DESTINATION bin)
