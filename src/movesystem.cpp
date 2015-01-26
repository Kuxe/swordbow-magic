#define NDEBUG

#include "movesystem.h"
#include <iostream>
#include "deltatime.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "inputcomponent.h"
#include "deltatime.h"
#include "flagcomponent.h"

using namespace std;

void MoveSystem::add(unsigned long long int* id) {
	moveDatas.insert(
		make_pair(
			id,
			MoveData{
				componentManager->moveComponents.at(id),
				componentManager->inputComponents.at(id),
				componentManager->flagComponents.at(id),
			}
		)
	);
}

void MoveSystem::remove(unsigned long long int* id) {
	if(moveDatas.erase(id) == 0) {
		cout << "WARNING: MoveSystem tried to erase unpresent ID " << *id << ", segfault inc!" << endl;
	}
}

void MoveSystem::update() {
	for(auto idMoveData : moveDatas) {
		MoveComponent* mc = get<1>(idMoveData).moveComponent;
		InputComponent* ic = get<1>(idMoveData).inputComponent;
		mc->oldXpos = mc->xpos;
		mc->oldYpos = mc->ypos;
		mc->xpos += ((ic->d * mc->xspeed) - (ic->a * mc->xspeed)) * deltaTime->delta();
		mc->ypos += ((ic->s * mc->yspeed) - (ic->w * mc->yspeed)) * deltaTime->delta();

		//If this entity moved, set the HAS_CHANGED flag
		if(!(mc->xpos == mc->oldXpos && mc->ypos == mc->oldYpos)) {
			get<1>(idMoveData).flagComponent->flags |= FlagComponent::HAS_CHANGED;
		}
	}
}

unsigned int MoveSystem::count() const {
	return moveDatas.size();
}

const string MoveSystem::getIdentifier() const {
	return "MoveSystem";
}