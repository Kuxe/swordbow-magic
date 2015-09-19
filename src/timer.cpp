#include "timer.hpp"

void Timer::start() {
	startTime = std::chrono::high_resolution_clock::now();
}
void Timer::stop() {
	stopTime = std::chrono::high_resolution_clock::now();
	time = (stopTime - startTime).count() / 1000000000.0f;
}

float Timer::delta() const {
	return time;
}

float Timer::elapsed() const {
	return (std::chrono::high_resolution_clock::now() - startTime).count() / 1000000000.0f;
}