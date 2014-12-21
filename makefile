
INCLUDE = -I include/
SRC_FILES = src/*.cpp
TEST_SRC_FILES = $(filter-out src/main.cpp, $(wildcard src/*.cpp)) test/*.cpp
FLAGS = -std=c++1y
CC = g++
CFLAGS = $(FLAGS) $(INCLUDE)
LINKER_FLAGS = -lSDL2
BUILD_RELEASE = -o ./build/release/swordbow-magic
BUILD_DEBUG = -g -o ./build/debug/swordbow-magic
BUILD_TEST = -g -o ./test/runtests
RUN_RELEASE = ./build/release/swordbow-magic
RUN_DEBUG = ./build/debug/swordbow-magic

all:
	$(CC) $(CFLAGS) $(SRC_FILES) $(LINKER_FLAGS) $(BUILD_RELEASE)

release:
	$(CC) $(CFLAGS) $(SRC_FILES) $(LINKER_FLAGS) $(BUILD_RELEASE)

runrelease:
	$(RUN_RELEASE)

debug:
	$(CC) $(CFLAGS) $(SRC_FILES) $(LINKER_FLAGS) $(BUILD_DEBUG)

rundebug:
	$(RUN_DEBUG)

tests:
	$(CC) $(CFLAGS) $(TEST_SRC_FILES) $(LINKER_FLAGS) $(BUILD_TEST)
	./test/runtests

profile: debug
	rm -f ./callgrind.out.*
	valgrind --tool=callgrind ./build/debug/swordbow-magic
	kcachegrind callgrind.out.*

clean:
	rm -f ./callgrind.out.*
	rm -rf ./build/*/*
	rm -f ./test/runtests