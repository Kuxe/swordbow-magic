#include "eventmanager.h"
#include <SDL2/SDL.h>

EventManager::EventManager(bool* runningPtr)
	: runningPtr(runningPtr) {

}

void EventManager::process() {
	//Fetch all events that ocurred...
	while(SDL_PollEvent(&event) != 0) {
		//And take appropiate action!
		switch(event.type) {
			case SDL_QUIT: {
				*runningPtr = false;
			}
		}
	}
}