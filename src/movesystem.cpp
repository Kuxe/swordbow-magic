#define NDEBUG

#include "movesystem.h"
#include <iostream>
#include "componentmanager.h"
#include "movecomponent.h"
#include "inputcomponent.h"

using namespace std;

void MoveSystem::add(unsigned long long int* id) {
	moveDatas.insert(
		make_pair(
			id,
			MoveData{
				componentManager->moveComponents.at(id),
				componentManager->inputComponents.at(id)
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
		mc->xpos += (ic->d * mc->xspeed) - (ic->a * mc->xspeed);
		mc->ypos += (ic->s * mc->yspeed) - (ic->w * mc->yspeed);		
	}
}

unsigned int MoveSystem::count() const {
	return moveDatas.size();
}

const string MoveSystem::getIdentifier() const {
	return "MoveSystem";
}