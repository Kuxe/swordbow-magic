#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <SDL2/SDL.h>

class EventManager {
 private:
 	SDL_Event event;
 	bool* runningPtr;
 public:
 	EventManager(bool* runningPtr);
	void process();
};

#endif //EVENTMANAGER_H
