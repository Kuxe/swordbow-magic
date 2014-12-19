#include "systemmanager.h"
#include "isystem.h"
#include <iostream>

using namespace std;


void SystemManager::add(ISystem* system) {
	systems.push_front(system);
	systemByIdentifiers.insert(std::make_pair(system->getIdentifier(), system));
}
void SystemManager::remove(ISystem* system) {
	//TODO(Kuxe): Implement. Remove from systems and systemByIdentifier
}

void SystemManager::update() {
	for(ISystem* system : systems) {
		system->update();
	}
}

ISystem* SystemManager::getSystem(string identifier) {
	ISystem* system;
	try {
		system = systemByIdentifiers.at(identifier);
	} catch(const std::out_of_range& oor) {
		cout << "out_of_range exception caught. " << endl;
		cout << "Maybe there as a typo in the system-identifier, " << identifier << "?" << endl;
		cout << "Things will go wrong from now on!" << endl;
	}


	return systemByIdentifiers.at(identifier);
}