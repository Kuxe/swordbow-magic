#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <SDL2/SDL.h>

class InputComponent;
class PressKeyEventComponent;
class ReleaseKeyEventComponent;
class ComponentManager;

typedef unsigned long long int* ID;

class EventManager {
 private:
 	SDL_Event event;
 	bool* runningPtr;
    InputComponent* userInputComponent;
    PressKeyEventComponent* userPressKeyEventComponent;
    ReleaseKeyEventComponent* userReleaseKeyEventComponent;
 public:
 	EventManager(bool* runningPtr);
	void process();
    void setPlayer(ID id, ComponentManager* componentManager);
};

#endif //EVENTMANAGER_H
