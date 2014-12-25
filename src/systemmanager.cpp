#include "systemmanager.h"
#include "isystem.h"
#include <iostream>

using namespace std;

SystemManager::SystemManager(ComponentManager* componentManager, DeltaTime* deltaTime) :
	componentManager(componentManager),
	deltaTime(deltaTime) {

}

void SystemManager::add(ISystem* system) {
	systems.push_front(system);
	system->componentManager = componentManager;
	system->deltaTime = deltaTime;
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

		cout << "out_of_range exception caught in SystemManager::getSystem(string identifier): " << oor.what() << endl;
		cout << "1. Maybe there as a typo in the system-identifier argument, " << identifier << "?" << endl;
		cout << "2. Maybe " << identifier << " wasn't added to this SystemManager?" << endl;
		cout << "3. Maybe " << identifier << " was added, but got removed?" << endl;
		cout << "Things will go wrong from now on!" << endl;
	}


	return systemByIdentifiers.at(identifier);
}