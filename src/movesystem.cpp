#define NDEBUG

#include "movesystem.hpp"
#include <iostream>
#include "deltatime.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "inputcomponent.hpp"
#include "deltatime.hpp"
#include "systemmanager.hpp"
#include "rendersystem.hpp"
#include "hashgridsystem.hpp"
#include "collisionsystem.hpp"

using namespace std;

void MoveSystem::add(ID id) {
	ids.insert(id);
	activeIds.insert(id);
}

void MoveSystem::remove(ID id) {
	if(ids.erase(id) == 0) {
		cout << "WARNING: MoveSystem tried to erase unpresent ID " << id << ", segfault inc!" << endl;
	}
}

void MoveSystem::update() {
	for(auto id : ids) {
		auto& mc = componentManager->moveComponents.at(id);
		auto& ic = componentManager->inputComponents.at(id);
		mc.oldXpos = mc.xpos;
		mc.oldYpos = mc.ypos;
		mc.xpos += ((ic.d * mc.xspeed) - (ic.a * mc.xspeed)) * deltaTime->delta();
		mc.ypos += ((ic.s * mc.yspeed) - (ic.w * mc.yspeed)) * deltaTime->delta();

		//Whenever an entity has been moved by something or moved by itself...
		if(!(mc.xpos == mc.oldXpos && mc.ypos == mc.oldYpos)) {
			//Then it moved. Sherlock
			componentManager->moveEventComponents.at(id).happen();

		}
	}
}

unsigned int MoveSystem::count() const {
	return ids.size();
}

const string MoveSystem::getIdentifier() const {
	return "MoveSystem";
}

void MoveSystem::activateId(ID id) {
	add(id);
}
