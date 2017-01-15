CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

#Find modules that isn't supported by cmake. Have to put .cmake files in the
#/cmake/Modules/ folder for that to work. Find those files by googling.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


INCLUDE(FindPkgConfig)
INCLUDE(ExternalProject)
INCLUDE(CMakeParseArguments)

PROJECT(swordbow-magic)

#===========================
#======SETUP LIBRARIES======
#===========================

# Dependency tree for targets
#                   sbm
#                   /  \
#                  /    \
#                 /      \
#                /        \
#          sbm-client  sbm-server
#              /            \
#             /              \
#            /                \
#           /                  \
#          /                    \
# swordbow-magic-client swordbow-magic-server

#Create a library containing game-functionality shared between server and client
ADD_LIBRARY(sbm SHARED
    src/componentmanager.cpp
    src/deltatime.cpp
    src/hashgridsystem.cpp
    src/ipaddress.cpp
    src/logger.cpp
    src/rect.cpp
    src/systemmanager.cpp
    src/timer.cpp
)


#Server specific sources
ADD_LIBRARY(sbm-server SHARED
    src/accelerationsystem.cpp
    src/animationsystem.cpp
    src/attacksystem.cpp
    src/birdsystem.cpp
    src/collisionsystem.cpp
    src/entitymanager.cpp
    src/healthsystem.cpp
    src/idmanager.cpp
    src/initialcomponentssystem.cpp
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
    src/client.cpp
    src/clientdisconnectedstate.cpp
    src/clientreceiveinitialstate.cpp
    src/clientrunningstate.cpp
    src/overlay.cpp
    src/renderer.cpp
    src/rendersystem.cpp
    src/soundengine.cpp
    src/text.cpp
    src/textureboundingbox.cpp
)

#Flags for compiler and linker
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -pthread -std=c++14")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -pthread -fomit-frame-pointer -march=native -flto -s -std=c++14")

#===========================
#====SETUP DEPENDENCIES=====
#===========================
#NOTE: For lots of dependencies, INSTALL_COMMAND is just set to ""
#this is because they shouldn't be installed on the system (ie in /usr/local/libs or alikes)
#This is fine, because all required files will be
#somewhere inside ${CMAKE_SOURCE_DIR}/deps/<somehere here or further down>/

#Run "make setup" to download dependencies required
#Other target are dependant on this target but not
#marked as dependant in this CMakeLists.txt, or any other
#CMakeLists.txt because this is a time consuming step
#that only needs to be executed once
ADD_CUSTOM_TARGET(setup
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

#Without these, no dependencies can be downloaded
#Hence these must be setup by everyone intended to build swordbow-magic
FIND_PACKAGE(Git REQUIRED)

SET(DEP_DIR "${CMAKE_SOURCE_DIR}/deps")
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_BASE "${DEP_DIR}")

#PLATFORM INDEPENDENT DOWNLOADS
EXTERNALPROJECT_ADD(Cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(setup Cereal)

EXTERNALPROJECT_ADD(GLM
    GIT_REPOSITORY https://github.com/g-truc/glm
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(setup GLM)

EXTERNALPROJECT_ADD(OGG
    GIT_REPOSITORY https://github.com/soundcloud/ogg
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(setup OGG)

EXTERNALPROJECT_ADD(Catch
    GIT_REPOSITORY https://github.com/philsquared/Catch
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(setup Catch)

EXTERNALPROJECT_ADD(args
    GIT_REPOSITORY https://github.com/Taywee/args
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(setup args)

TARGET_INCLUDE_DIRECTORIES(sbm PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
    ${DEP_DIR}/Source/Cereal/include/
    ${DEP_DIR}/Source/GLM/
    ${DEP_DIR}/Source/args
)
TARGET_INCLUDE_DIRECTORIES(sbm-client PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
    ${DEP_DIR}/Source/Cereal/include/
)
TARGET_INCLUDE_DIRECTORIES(sbm-server PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
)

#===========================
#=====SETUP EXECUTABLES=====
#===========================

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-client src/client.cpp)

IF(UNIX)
    TARGET_LINK_LIBRARIES(sbm-client PUBLIC sbm)
ELSEIF(WIN32)
    TARGET_LINK_LIBRARIES(sbm-client PUBLIC sbm wsock32 ws2_32)
ENDIF()
#What linker flags are required for swordbow-magic-client?
TARGET_LINK_LIBRARIES(swordbow-magic-client PUBLIC
    sbm-client
    pthread
)

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-server src/server.cpp)

#What linker flags are required for swordbow-magic-server?
IF(UNIX)
    TARGET_LINK_LIBRARIES(sbm-server PUBLIC sbm)
ELSEIF(WIN32)
    TARGET_LINK_LIBRARIES(sbm-server PUBLIC sbm wsock32 ws2_32)
ENDIF()
TARGET_LINK_LIBRARIES(swordbow-magic-server PUBLIC sbm-server)

#Create a test-executable using Catch
ADD_EXECUTABLE(swordbow-magic-test
    catch/src/config.cpp
    catch/src/hashgridsystem_test.cpp
    catch/src/rect_test.cpp
    catch/src/rendersystem_test.cpp
)

#What linker flags are required for swordbow-magic-test?
TARGET_LINK_LIBRARIES(swordbow-magic-test PUBLIC
    sbm
    sbm-server
    sbm-client
)

#===========================
#=======SETUP TARGETS=======
#===========================

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
