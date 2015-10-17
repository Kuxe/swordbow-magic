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

#Need to link to wsock32 on windows
IF(${CMAKE_SYSTEM_NAME} EQUAL "Windows")
    TARGET_LINK_LIBRARIES(sbm PUBLIC
        -lwsock32
    )
ENDIF()

#===========================
#====SETUP DEPENDENCIES=====
#===========================
#NOTE: For lots of dependencies, INSTALL_COMMAND is just set to ""
#this is because they shouldn't be installed on the system (ie in /usr/local/libs or alikes)
#This is fine, because all required files will be
#somewhere inside ${CMAKE_SOURCE_DIR}/deps/<somehere here or further down>/

#Without these, no dependencies can be downloaded
#Hence these must be setup by everyone intended to build swordbow-magic
FIND_PACKAGE(Git REQUIRED)
FIND_PACKAGE(Hg REQUIRED)

SET(DEP_DIR "${CMAKE_SOURCE_DIR}/deps")

EXTERNALPROJECT_ADD(Cereal
    PREFIX ${DEP_DIR}/cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal
    BUILD_COMMAND "" #Cereal is header only
    INSTALL_COMMAND "" #Cereal is header only
)
ADD_DEPENDENCIES(sbm Cereal)

SET(SDL2_INSTALL_PREFIX "${DEP_DIR}/SDL2-install")
FILE(MAKE_DIRECTORY "${SDL2_INSTALL_PREFIX}")

EXTERNALPROJECT_ADD(SDL2_project
    PREFIX ${DEP_DIR}/SDL2
    HG_REPOSITORY http://hg.libsdl.org/SDL
    CONFIGURE_COMMAND ${DEP_DIR}/SDL2/src/SDL2_project/configure --prefix=${SDL2_INSTALL_PREFIX} --quiet
    INSTALL_COMMAND make install --quiet
)
ADD_DEPENDENCIES(sbm SDL2_project)

EXTERNALPROJECT_ADD(SDL2_image_project
    PREFIX ${DEP_DIR}/SDL2_image
    HG_REPOSITORY http://hg.libsdl.org/SDL_image/
    CONFIGURE_COMMAND ${DEP_DIR}/SDL2_image/src/SDL2_image_project/configure --prefix=${SDL2_INSTALL_PREFIX} --with-sdl-prefix=${SDL2_INSTALL_PREFIX} --quiet
    BUILD_COMMAND ""
    INSTALL_COMMAND make install --quiet
)
ADD_DEPENDENCIES(SDL2_image_project SDL2_project)
ADD_DEPENDENCIES(sbm-client SDL2_image_project)

EXTERNALPROJECT_ADD(SDL2_ttf_project
    PREFIX ${DEP_DIR}/SDL2_ttf
    HG_REPOSITORY http://hg.libsdl.org/SDL_ttf/
    CONFIGURE_COMMAND ${DEP_DIR}/SDL2_ttf/src/SDL2_ttf_project/configure --prefix=${SDL2_INSTALL_PREFIX} --with-sdl-prefix=${SDL2_INSTALL_PREFIX} --quiet
    BUILD_COMMAND ""
    INSTALL_COMMAND make install --quiet
)
ADD_DEPENDENCIES(SDL2_ttf_project SDL2_project)
ADD_DEPENDENCIES(sbm-client SDL2_ttf_project)

EXTERNALPROJECT_ADD(SDL2_mixer_project
    PREFIX ${DEP_DIR}/SDL2_mixer
    HG_REPOSITORY http://hg.libsdl.org/SDL_mixer/
    CONFIGURE_COMMAND ${DEP_DIR}/SDL2_mixer/src/SDL2_mixer_project/configure --prefix=${SDL2_INSTALL_PREFIX} --with-sdl-prefix=${SDL2_INSTALL_PREFIX} --quiet
    BUILD_COMMAND ""
    INSTALL_COMMAND make install --quiet
)
ADD_DEPENDENCIES(SDL2_mixer_project SDL2_project)
ADD_DEPENDENCIES(sbm-client SDL2_mixer_project)

EXTERNALPROJECT_ADD(GLM
    PREFIX ${DEP_DIR}/GLM
    GIT_REPOSITORY https://github.com/g-truc/glm
    BUILD_COMMAND "" #header only
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(sbm GLM)

EXTERNALPROJECT_ADD(OGG
    PREFIX ${DEP_DIR}/OGG
    GIT_REPOSITORY https://github.com/soundcloud/ogg
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(sbm-client OGG)

EXTERNALPROJECT_ADD(Catch
    PREFIX ${DEP_DIR}/Catch
    GIT_REPOSITORY https://github.com/philsquared/Catch
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ADD_DEPENDENCIES(sbm Catch)

TARGET_INCLUDE_DIRECTORIES(sbm PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
    ${DEP_DIR}/cereal/src/Cereal/include/
    ${DEP_DIR}/GLM/src/GLM/
)
TARGET_INCLUDE_DIRECTORIES(sbm-client PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
    ${DEP_DIR}/SDL2/src/SDL2_project/include/
    ${DEP_DIR}/SDL2_image/src/SDL2_image_project/
    ${DEP_DIR}/SDL2_ttf/src/SDL2_ttf_project/
    ${DEP_DIR}/SDL2_mixer/src/SDL2_mixer_project/
    ${DEP_DIR}/cereal/src/Cereal/include/
)
TARGET_INCLUDE_DIRECTORIES(sbm-server PUBLIC
    ${CMAKE_SOURCE_DIR}/include/
    ${DEP_DIR}/SDL2/src/SDL2_project/include/
)


SET(SDL_LIBS "${SDL2_INSTALL_PREFIX}/lib")
ADD_LIBRARY(SDL2 SHARED IMPORTED)
ADD_LIBRARY(SDL2_image SHARED IMPORTED)
ADD_LIBRARY(SDL2_ttf SHARED IMPORTED)
ADD_LIBRARY(SDL2_mixer SHARED IMPORTED)
IF(UNIX AND NOT APPLE)
    SET_TARGET_PROPERTIES(SDL2 PROPERTIES IMPORTED_LOCATION "${SDL_LIBS}/libSDL2.so")
    SET_TARGET_PROPERTIES(SDL2_image PROPERTIES IMPORTED_LOCATION "${SDL_LIBS}/libSDL2_image.so")
    SET_TARGET_PROPERTIES(SDL2_ttf PROPERTIES IMPORTED_LOCATION "${SDL_LIBS}/libSDL2_ttf.so")
    SET_TARGET_PROPERTIES(SDL2_mixer PROPERTIES IMPORTED_LOCATION "${SDL_LIBS}/libSDL2_mixer.so")
ENDIF()

TARGET_LINK_LIBRARIES(sbm-client PUBLIC
    sbm
    SDL2
    SDL2_image
    SDL2_ttf
    SDL2_mixer
)

#===========================
#=====SETUP EXECUTABLES=====
#===========================

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-client src/client.cpp)

#What linker flags are required for swordbow-magic-client?
TARGET_LINK_LIBRARIES(swordbow-magic-client PUBLIC
    sbm-client
)

#Create a client-executable
ADD_EXECUTABLE(swordbow-magic-server src/server.cpp)

#What linker flags are required for swordbow-magic-server?
TARGET_LINK_LIBRARIES(sbm-server PUBLIC
    sbm #sbm-server depends on sbm
)
TARGET_LINK_LIBRARIES(swordbow-magic-server PUBLIC
    sbm-server
)

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
