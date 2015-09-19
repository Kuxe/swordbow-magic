#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>

class Timer {
private:
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point stopTime;
	float time = 0.0f;

public:
	void start();
	void stop();
	float delta() const;
	float elapsed() const;
};

#endif //TIMER_HPP