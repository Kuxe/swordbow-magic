cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(swordbow-magic)

#Flags for compiler and linker
SET(COMPILE_FLAGS "-std=c++14")
SET(LINK_FLAGS "-lSDL2 -lSDL2_image")

#Append flags for compiler and linker to cmake variables
#(the compiler uses the cmake variables)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

#Binary should be in a bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

include_directories(./include/)

add_definitions(-g -std=c++14)

add_executable(swordbow-magic
    src/collisionsystem.cpp
    src/componentmanager.cpp
    src/deltatime.cpp
    src/entitymanager.cpp
    src/eventmanager.cpp
    src/flagsystem.cpp
    src/gridindexer.cpp
    src/hashgrid.cpp
    src/idmanager.cpp
    src/isystem.cpp
    src/main.cpp
    src/movecomponent.cpp
    src/movesystem.cpp
    src/rendercomponent.cpp
    src/rendersystem.cpp
    src/systemmanager.cpp
    src/tilecomponent.cpp
    src/world.cpp)

#Run "make run" to run the binary
add_custom_target(run
    COMMAND swordbow-magic
    DEPENDS swordbow-magic
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

install(TARGETS swordbow-magic DESTINATION bin)
