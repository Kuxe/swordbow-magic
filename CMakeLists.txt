CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

PROJECT(swordbow-magic)

INCLUDE(FindPkgConfig)

#Create a library containing game-functionality shared between server and client
ADD_LIBRARY(sbm SHARED
    src/componentmanager.cpp
    src/deltatime.cpp
    src/hashgridsystem.cpp
    src/ipaddress.cpp
    src/logger.cpp
    src/rect.cpp
    src/systemmanager.cpp
)

#Server specific sources
ADD_LIBRARY(sbm-server SHARED
    src/animationsystem.cpp
    src/attacksystem.cpp
    src/collisionsystem.cpp
    src/entitymanager.cpp
    src/healthsystem.cpp
    src/idmanager.cpp
    src/inputsystem.cpp
    src/movediffsystem.cpp
    src/movesystem.cpp
    src/noisebrush.cpp
    src/playsound.cpp
    src/removesystem.cpp
    src/renderdiffsystem.cpp
    src/sizeboundingbox.cpp
    src/world.cpp
)

#Client specific sources
ADD_LIBRARY(sbm-client SHARED
    src/camerasystem.cpp
    src/renderer.cpp
    src/rendersystem.cpp
    src/soundengine.cpp
    src/text.cpp
    src/textureboundingbox.cpp
)

#Flags for compiler and linker
SET(CMAKE_CXX_COMPILER "g++")
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -pthread -std=c++14")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -pthread -fomit-frame-pointer -march=native -flto -s -std=c++14")

#Need to link to wsock32 on windows
IF(${CMAKE_SYSTEM_NAME} EQUAL "Windows")
    TARGET_LINK_LIBRARIES(sbm -lwsock32)
ENDIF()

#Find SDL2 dependencies
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2MIXER REQUIRED SDL2_mixer)
INCLUDE_DIRECTORIES(./include/ ./catch/ ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(sbm-client ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})

#Find modules that isn't supported by cmake. Have to put .cmake files in the
#/cmake/Modules/ folder for that to work. Find those files by googling.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

FIND_PACKAGE(Cereal REQUIRED)
INCLUDE_DIRECTORIES(${CEREAL_INCLUDE_DIRS})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CEREAL_DEFINITIONS}")

FIND_PACKAGE(GLM REQUIRED)
INCLUDE_DIRECTORIES(${GLM_INCLUDE_DIRS})

FIND_PACKAGE(OGG REQUIRED)
INCLUDE_DIRECTORIES(${OGG_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(sbm-client ${OGG_LIBRARIES})

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-client src/client.cpp)

#What linker flags are required for swordbow-magic-client?
TARGET_LINK_LIBRARIES(swordbow-magic-client sbm sbm-client)

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-server src/server.cpp)

#What linker flags are required for swordbow-magic-server?
TARGET_LINK_LIBRARIES(swordbow-magic-server sbm sbm-server)

#Create a test-executable using Catch
ADD_EXECUTABLE(swordbow-magic-test
    catch/src/config.cpp
    catch/src/hashgridsystem_test.cpp
    catch/src/rect_test.cpp
    catch/src/rendersystem_test.cpp
)

#What linker flags are required for swordbow-magic-test?
TARGET_LINK_LIBRARIES(swordbow-magic-test sbm sbm-server sbm-client)

#Run "make client" to build the client
ADD_CUSTOM_TARGET(client
    DEPENDS swordbow-magic-client
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

#Run "make server" to build the server
ADD_CUSTOM_TARGET(server
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
