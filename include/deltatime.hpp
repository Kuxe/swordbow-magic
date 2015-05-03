#ifndef DELTATIME_H
#define DELTATIME_H
#include <chrono>

class DeltaTime {
 private:
 	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point stopTime;
	float deltaTime;

 public:
	void start();
	void stop();
	float delta() const;
};

#endif //DELTATIME_H