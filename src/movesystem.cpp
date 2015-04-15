#define NDEBUG

#include "movesystem.h"
#include <iostream>
#include "deltatime.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "inputcomponent.h"
#include "deltatime.h"
#include "flagcomponent.h"
#include "systemmanager.h"
#include "rendersystem.h"

using namespace std;

void MoveSystem::add(unsigned long long int* id) {
	ids.insert(id);
}

void MoveSystem::remove(unsigned long long int* id) {
	if(ids.erase(id) == 0) {
		cout << "WARNING: MoveSystem tried to erase unpresent ID " << *id << ", segfault inc!" << endl;
	}
}

void MoveSystem::update() {
	for(auto id : ids) {
		auto mc = componentManager->moveComponents.at(id);
		auto ic = componentManager->inputComponents.at(id);
		mc->oldXpos = mc->xpos;
		mc->oldYpos = mc->ypos;
		mc->xpos += ((ic->d * mc->xspeed) - (ic->a * mc->xspeed)) * deltaTime->delta();
		mc->ypos += ((ic->s * mc->yspeed) - (ic->w * mc->yspeed)) * deltaTime->delta();

		//If this entity moved, set the HAS_CHANGED flag
		if(!(mc->xpos == mc->oldXpos && mc->ypos == mc->oldYpos)) {
			static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->makeIdActive(id);
		}
	}
}

unsigned int MoveSystem::count() const {
	return ids.size();
}

const string MoveSystem::getIdentifier() const {
	return "MoveSystem";
}
