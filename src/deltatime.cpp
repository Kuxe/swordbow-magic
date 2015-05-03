#include <iostream>
#include "deltatime.hpp"

using namespace std;

void DeltaTime::start() {
	startTime = std::chrono::high_resolution_clock::now();
}
void DeltaTime::stop() {
	stopTime = std::chrono::high_resolution_clock::now();
	deltaTime = (stopTime - startTime).count() / 1000000000.0f;
}
float DeltaTime::delta() const {
	return deltaTime;
}
