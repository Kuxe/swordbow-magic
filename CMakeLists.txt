cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(swordbow-magic)

#Binary should be in root folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(swordbow-magic
    src/animationsystem.cpp
    src/attacksystem.cpp
    src/collisionsystem.cpp
    src/componentmanager.cpp
    src/deltatime.cpp
    src/entitymanager.cpp
    src/eventmanager.cpp
    src/hashgridsystem.cpp
    src/healthsystem.cpp
    src/idmanager.cpp
    src/inputsystem.cpp
    src/main.cpp
    src/movecomponent.cpp
    src/movesystem.cpp
    src/playsound.cpp
    src/removesystem.cpp
    src/rendersystem.cpp
    src/sizeboundingbox.cpp
    src/soundsystem.cpp
    src/systemmanager.cpp
    src/text.cpp
    src/textureboundingbox.cpp
    src/world.cpp)

INCLUDE(FindPkgConfig)

#Find SDL2 dependencies
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)

#Flags for compiler and linker
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
SET(COMPILE_FLAGS "-std=c++14")

#Append flags for compiler and linker to cmake variables
#(the compiler uses the cmake variables)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

#What include directories are there?..:
include_directories(./include/ ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})

#What linker flags are required?..:
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})

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
