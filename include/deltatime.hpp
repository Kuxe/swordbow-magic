#ifndef DELTATIME_H
#define DELTATIME_H
#include <chrono>
#include "timer.hpp"

class DeltaTime {
 private:
 	Timer timer;

 public:
	void start();
	void stop();
	float delta() const;
    float elapsed() const;
};

#endif //DELTATIME_H
