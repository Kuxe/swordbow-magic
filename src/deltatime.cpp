#include <iostream>
#include "deltatime.h"

using namespace std;

void DeltaTime::start() {
	startTime = std::chrono::high_resolution_clock::now();
}
void DeltaTime::stop() {
	stopTime = std::chrono::high_resolution_clock::now();
	deltaTime = (stopTime - startTime).count() / 1000000000.0f;
}
float DeltaTime::delta() const {
	cout << deltaTime << endl;
	return deltaTime;
}