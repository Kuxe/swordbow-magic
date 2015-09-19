#include "deltatime.hpp"

void DeltaTime::start() {
	timer.start();
}
void DeltaTime::stop() {
	timer.stop();
}
float DeltaTime::delta() const {
	return timer.delta();
}

float DeltaTime::elapsed() const {
	return timer.elapsed();
}
