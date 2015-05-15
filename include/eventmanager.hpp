#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <SDL2/SDL.h>

class InputComponent;
class ReleaseKeyEventComponent;
class ComponentManager;

typedef unsigned int ID;

class InputSystem;
class EventManager {
 private:
 	SDL_Event event;
 	bool* runningPtr;
    InputComponent* userInputComponent;
    ReleaseKeyEventComponent* userReleaseKeyEventComponent;
    ID playerId;
    InputSystem* inputSystem;
 public:
 	EventManager(bool* runningPtr, InputSystem* inputSystem);
	void process();
    void setPlayer(ID id, ComponentManager* componentManager);
};

#endif //EVENTMANAGER_H
