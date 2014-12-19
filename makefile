
all:
	g++ -o ./build/release/swordbow-magic -I include/ -std=c++1y src/*.cpp

release:
	g++ -o ./build/release/swordbow-magic -I include/ -std=c++1y src/*.cpp

runrelease:
	./build/release/swordbow-magic

debug:
	g++ -g -o ./build/debug/swordbow-magic -I include/ -std=c++1y src/*.cpp

rundebug:
	./build/debug/swordbow-magic

profile:
	rm -f ./callgrind.out.*
	valgrind --tool=callgrind ~/programming/cpp/swordbow-magic/build/debug/swordbow-magic
	kcachegrind callgrind.out.*

clean:
	rm -f ./callgrind.out.*
	rm -rf ./build/*/*