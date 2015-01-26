#include "flagsystem.h"
#include "componentmanager.h"
#include <iostream>
#include "flagcomponent.h"

using namespace std;

void FlagSystem::add(unsigned long long int* id) {
	flagDatas.insert(
		make_pair(
			id,
			FlagData{
				componentManager->flagComponents.at(id)
			}
		)
	);
}
void FlagSystem::remove(unsigned long long int* id) {
	if(flagDatas.erase(id) == 0) {
		cout << "WARNING: MoveSystem tried to erase unpresent ID " << *id << ", segfault inc!" << endl;
	}
}
void FlagSystem::update() {
	for(auto flag : flagDatas) {
		get<1>(flag).flagComponent->flags &= FlagComponent::CLEAR;
	}
}
unsigned int FlagSystem::count() const {
	return flagDatas.size();
}
const string FlagSystem::getIdentifier() const {
	return "FlagSystem";
}