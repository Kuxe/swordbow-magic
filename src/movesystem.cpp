#define NDEBUG

#include "movesystem.h"
#include <iostream>
#include "deltatime.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "inputcomponent.h"
#include "deltatime.h"
#include "systemmanager.h"
#include "rendersystem.h"
#include "hashgridsystem.h"

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

		//Whenever an entity has been moved by something or moved by itself...
		if(!(mc->xpos == mc->oldXpos && mc->ypos == mc->oldYpos)) {
			//then it should render this tick
			static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->makeIdActive(id);

			//and it should be updated within HashGrid (hashgrid with bounding boxes by texture-dimensions)
			//later on another HashGrid with bounding boxes by size-dimensions should be updated here aswell
			//currently there is no HashGrid with bounding boxes - theres only a O(n^2) bruteforce-system..
			static_cast<HashGridSystem*>(systemManager->getSystem("HashGridSystem"))->makeIdActive(id);

		}
	}
}

unsigned int MoveSystem::count() const {
	return ids.size();
}

const string MoveSystem::getIdentifier() const {
	return "MoveSystem";
}
