#include "eventmanager.h"
#include <SDL2/SDL.h>

#include "inputcomponent.h"

#include <iostream>
using namespace std;

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
				break;
			}

			//If this event was a keydown...
			case SDL_KEYDOWN: {
				//Figure out what key was pressed
				switch(event.key.keysym.sym) {
					case SDLK_w: {
						userInputComponent->w = true;
						break;
					}
					case SDLK_a: {
						userInputComponent->a = true;
						break;
					}
					case SDLK_s: {
						userInputComponent->s = true;
						break;
					}
					case SDLK_d: {
						userInputComponent->d = true;
						break;
					}
				}
			} break;

			//If user released a key
			case SDL_KEYUP: {
				//Figure out what key was released
				switch(event.key.keysym.sym) {
					case SDLK_w: {
						userInputComponent->w = false;
						break;
					}
					case SDLK_a: {
						userInputComponent->a = false;
						break;
					}
					case SDLK_s: {
						userInputComponent->s = false;
						break;
					}
					case SDLK_d: {
						userInputComponent->d = false;
						break;
					}
				}
			} break;
		}

	}
}