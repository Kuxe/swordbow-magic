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
#include "icommand.hpp"
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
		auto& cc = componentManager->commandComponents.at(id);
		auto& ac = componentManager->animationComponents.at(id);
		mc.oldPos.x = mc.pos.x;
		mc.oldPos.y = mc.pos.y;

		//If no key was pressed
		if(ic.d != 0 || ic.a != 0 || ic.s != 0 || ic.w != 0) {
			mc.dir.x = ic.d - ic.a;
			mc.dir.y = ic.s - ic.w;
		}

		mc.vel = {ic.d - ic.a, ic.s - ic.w};

		//If some input was recieved which caused a move (mc.vel isn't of length 0)
		if(glm::length(mc.vel) > 0) {
			//Set it to appropiate length
			mc.vel = glm::normalize(mc.vel);
			mc.vel *= mc.maxVelLength;

			//Adjust for dt
			mc.vel *= deltaTime->delta();

			//Finally update position
			mc.pos += mc.vel;

			//It might be handy to normalize the dir for other systems
			mc.dir = normalize(mc.dir);
		}

		//create a unique number for each possible direction
		const char dirhash = mc.dir.x * 2 + mc.dir.y * 3;

		//FIXME: This, bellow, assumes that any member of movesystem also is animating it's
		//movement, which isn't quite right... It cant be inside animationsystem
		//though since this piece of logic always selects idle animation if
		//the entity doesn't move, which is unwanted behavior in some situations
		//for entities that doesn't move...

		//If this entity is moving, play animation in correct direction
		if(glm::length(mc.vel) > 0) {
			switch(dirhash) {
				case -3: {
					ac.state = &ac.walking.north;
				} break;
				case -1: {
					ac.state = &ac.walking.northEast;
				} break;
				case 2: {
					ac.state = &ac.walking.east;
				} break;
				case 5: {
					ac.state = &ac.walking.southEast;
				} break;
				case 3: {
					ac.state = &ac.walking.south;
				} break;
				case 1: {
					ac.state = &ac.walking.southWest;
				} break;
				case -2: {
					ac.state = &ac.walking.west;
				} break;
				case -5: {
					ac.state = &ac.walking.northWest;
				} break;
			}
		}

		//Else this entity is standing still. Play idle animation in correct direction
		else {
			switch(dirhash) {
				case -3: {
					ac.state = &ac.idle.north;
				} break;
				case -1: {
					ac.state = &ac.idle.northEast;
				} break;
				case 2: {
					ac.state = &ac.idle.east;
				} break;
				case 5: {
					ac.state = &ac.idle.southEast;
				} break;
				case 3: {
					ac.state = &ac.idle.south;
				} break;
				case 1: {
					ac.state = &ac.idle.southWest;
				} break;
				case -2: {
					ac.state = &ac.idle.west;
				} break;
				case -5: {
					ac.state = &ac.idle.northWest;
				} break;
			}
		}

		//Do something defined by the entity if the entity moved
		if(!(mc.pos.x == mc.oldPos.x && mc.pos.y == mc.oldPos.y)) {
			cc.execute(CommandComponent::Event::ON_MOVE);
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
