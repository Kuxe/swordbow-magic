#include "eventmanager.hpp"
#include <SDL2/SDL.h>

#include "inputcomponent.hpp"
#include "releasekeyeventcomponent.hpp"
#include "componentmanager.hpp"
#include "inputsystem.hpp"

EventManager::EventManager(bool* runningPtr, InputSystem* inputSystem) :
	runningPtr(runningPtr),
	inputSystem(inputSystem) {

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
					case SDLK_SPACE: {
						userInputComponent->space = true;
						break;
					}
				}

				//Save the keypress for later processing within inputsystem
				userInputComponent->presses.push(event.key.keysym.sym);
				inputSystem->activateId(playerId);

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
					case SDLK_SPACE: {
						userInputComponent->space = false;
						break;
					}
				}

				userInputComponent->presses.push(event.key.keysym.sym);

				//TODO: 2015-05-15 Should probably remove this bellow...
				//TODO: This event doesnt make sense. "RelaseKeyEvent" but only
				//if all keys are release.. hmm.. userReleaseKeyEvent does, as of
				//2015-05-03 01:41 remove the player from movesystem, which should
				//only happen if no keys are down. Hence this piece of shitty code.
				if( userInputComponent->w == false && userInputComponent->a == false &&
					userInputComponent->s == false && userInputComponent->d == false) {
						userReleaseKeyEventComponent->happen();
					}
			} break;
		}

	}
}

void EventManager::setPlayer(ID id, ComponentManager* componentManager) {
	userInputComponent = &componentManager->inputComponents.at(id);
	userReleaseKeyEventComponent = &componentManager->releaseKeyEventComponents.at(id);
	playerId = id;
}
