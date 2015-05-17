#include "keyboardsystem.hpp"
#include <SDL2/SDL.h>

#include "inputcomponent.hpp"
#include "componentmanager.hpp"
#include "inputsystem.hpp"
#include "systemmanager.hpp"

KeyboardSystem::KeyboardSystem(bool* runningPtr) :
	runningPtr(runningPtr) {

}

void KeyboardSystem::add(ID id) {
	this->id = id;
}
void KeyboardSystem::remove(ID id) {
	if(this->id == id) this->id = 0;
}

void KeyboardSystem::update() {

	//If no id is assigned for keyboard control,
	//it should still be possible to quit the game...
	if(id == 0) {
		while(SDL_PollEvent(&event) != 0) {
			switch(event.type) {
				case SDL_QUIT: {
					*runningPtr = false;
					break;
				}
			}
		}
	} else {
		auto& ic = componentManager->inputComponents.at(id);
		auto inputSystem = systemManager->getSystem("InputSystem");

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
							ic.w = true;
							break;
						}
						case SDLK_a: {
							ic.a = true;
							break;
						}
						case SDLK_s: {
							ic.s = true;
							break;
						}
						case SDLK_d: {
							ic.d = true;
							break;
						}
						case SDLK_SPACE: {
							ic.space = true;
							break;
						}
					}

					//Save the keypress for later processing within inputsystem
					ic.presses.push(event.key.keysym.sym);
					inputSystem->activateId(id);

				} break;

				//If user released a key
				case SDL_KEYUP: {
					//Figure out what key was released
					switch(event.key.keysym.sym) {
						case SDLK_w: {
							ic.w = false;
							break;
						}
						case SDLK_a: {
							ic.a = false;
							break;
						}
						case SDLK_s: {
							ic.s = false;
							break;
						}
						case SDLK_d: {
							ic.d = false;
							break;
						}
						case SDLK_SPACE: {
							ic.space = false;
							break;
						}
					}

					//Save the keyrelease for later processing within inputsystem
					ic.releases.push(event.key.keysym.sym);

				} break;
			}
		}
	}
}

unsigned int KeyboardSystem::count() const {
	return id > 0;
}
const std::string KeyboardSystem::getIdentifier() const {
	return "KeyboardSystem";
}
void KeyboardSystem::activateId(ID id) {

}
