#define NDEBUG

#include "movesystem.h"
#include "movecomponent.h"
#include <iostream>
#include "componentmanager.h"

using namespace std;

void MoveSystem::add(unsigned long long int* id) {
	moveDatas.insert(
		make_pair(
			id,
			MoveData{
				componentManager->moveComponents.at(id)
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
	for(auto mc : moveDatas) {
		get<1>(mc).moveComponent->move();
	}
}

unsigned int MoveSystem::count() const {
	return moveDatas.size();
}

const string MoveSystem::getIdentifier() const {
	return "MoveSystem";
}