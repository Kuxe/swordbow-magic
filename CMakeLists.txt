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
    src/lowpolyadaptor.cpp
    src/rendersystem.cpp
    src/oggadaptor.cpp
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
#somewhere inside ${CMAKE_SOURCE_DIR}/contrib/<somehere here or further down>/

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

SET(CONTRIB_DIR "${CMAKE_SOURCE_DIR}/contrib")
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_BASE "${CONTRIB_DIR}")

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

EXTERNALPROJECT_ADD(args
    GIT_REPOSITORY https://github.com/Taywee/args
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(setup args)

SET(LOWPOLY3D_INSTALL_DIR ${CONTRIB_DIR}/Source/lp3d)
SET(LOWPOLY3D_LIBRARY_DIR ${LOWPOLY3D_INSTALL_DIR}/lib)
SET(LOWPOLY3D_INCLUDE_DIR ${LOWPOLY3D_INSTALL_DIR}/include)
SET(LOWPOLY3D_SHADERS_DIR ${LOWPOLY3D_INSTALL_DIR}/shaders)

IF(UNIX AND NOT APPLE)
    EXTERNALPROJECT_ADD(lp3d
        DOWNLOAD_COMMAND wget https://github.com/Kuxe/lowpoly3d/releases/download/0.0.16/lowpoly3d-0.0.16-Linux.tar.gz
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )
ELSEIF(APPLE)
    EXTERNALPROJECT_ADD(lp3d
        URL https://github.com/Kuxe/lowpoly3d/releases/download/0.0.16/lowpoly3d-0.0.16-Darwin.tar.gz
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )
ELSEIF(WIN32)
    EXTERNALPROJECT_ADD(lp3d
        URL https://github.com/Kuxe/lowpoly3d/releases/download/0.0.16/lowpoly3d-0.0.16-win64.zip
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )
ENDIF()

ADD_DEPENDENCIES(setup lp3d)

TARGET_INCLUDE_DIRECTORIES(sbm PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
    ${CONTRIB_DIR}/Source/Cereal/include/
    ${CONTRIB_DIR}/Source/GLM/
    ${CONTRIB_DIR}/Source/args
)
TARGET_INCLUDE_DIRECTORIES(sbm-client PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
    ${CONTRIB_DIR}/Source/Cereal/include/
    ${LOWPOLY3D_INCLUDE_DIR}
)
TARGET_INCLUDE_DIRECTORIES(sbm-server PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
)

#===========================
#=====SETUP EXECUTABLES=====
#===========================

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-client src/client.cpp)

IF(UNIX AND NOT APPLE)
    TARGET_LINK_LIBRARIES(sbm-client PUBLIC sbm
        ${LOWPOLY3D_LIBRARY_DIR}/liblowpoly3d.so
    )
    ADD_CUSTOM_COMMAND(TARGET sbm-client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LOWPOLY3D_LIBRARY_DIR}/liblowpoly3d.so
        ${CMAKE_BINARY_DIR}/liblowpoly3d.so
    )
ELSEIF(APPLE)
    TARGET_LINK_LIBRARIES(sbm-client PUBLIC sbm
        ${LOWPOLY3D_LIBRARY_DIR}/liblowpoly3d.dylib
    )
    ADD_CUSTOM_COMMAND(TARGET sbm-client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LOWPOLY3D_LIBRARY_DIR}/liblowpoly3d.dylib
        ${CMAKE_BINARY_DIR}/liblowpoly3d.dylib
    )
ELSEIF(WIN32)
    TARGET_LINK_LIBRARIES(sbm-client PUBLIC sbm wsock32 ws2_32
        ${LOWPOLY3D_LIBRARY_DIR}/liblowpoly3d.dll
    )
    ADD_CUSTOM_COMMAND(TARGET sbm-client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LOWPOLY3D_LIBRARY_DIR}/liblowpoly3d.dll
        ${CMAKE_BINARY_DIR}/liblowpoly3d.dll
    )
ENDIF()

#Copy lowpoly3d shaders to build dir
ADD_CUSTOM_COMMAND(TARGET sbm-client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${LOWPOLY3D_SHADERS_DIR}
    ${CMAKE_BINARY_DIR}/shaders
)

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
