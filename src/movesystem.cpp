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
#include <math.h>

using namespace std;

void MoveSystem::add(ID id) {
	ids.insert(id);
	activeIds.insert(id);
}

void MoveSystem::remove(ID id) {
	if(ids.erase(id) == 0) {
		cout << "WARNING: MoveSystem tried to erase unpresent ID " << id << ", segfault inc!" << endl;
		return;
	}

	//since ID isnt moving anymore, oldxpos should be equal to xpos
	//but that update wont happen within update() method since
	//this id wont be iterated over within that loop after this call
	//so it is neccesary to force-update oldxpos here
	auto& mc = componentManager->moveComponents.at(id);
	mc.oldPos.x = mc.pos.x;
	mc.oldPos.y = mc.pos.y;
}

void MoveSystem::update() {
	for(auto id : ids) {
		auto& mc = componentManager->moveComponents.at(id);
		auto& ic = componentManager->inputComponents.at(id);
		mc.oldPos.x = mc.pos.x;
		mc.oldPos.y = mc.pos.y;

		mc.dir.x = ic.d - ic.a;
		mc.dir.y = ic.s - ic.w;

		mc.vel = mc.dir;

		//If some input was recieved which caused a move (mc.vel isn't of length 0)
		if(glm::length(mc.vel) > 0) {
			//Set it to appropiate length
			mc.vel = glm::normalize(mc.vel);
			mc.vel *= mc.maxVelLength;

			//Adjust for dt
			mc.vel *= deltaTime->delta();

			//Finally update position
			mc.pos += mc.vel;
		}

		//Whenever an entity has been moved by something or moved by itself...
		if(!(mc.pos.x == mc.oldPos.x && mc.pos.y == mc.oldPos.y)) {
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
