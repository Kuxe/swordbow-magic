#include "systemmanager.hpp"
#include "isystem.hpp"
#include <iostream>

using namespace std;

SystemManager::SystemManager(ComponentManager* componentManager, DeltaTime* deltaTime) :
	componentManager(componentManager),
	deltaTime(deltaTime) {

}

void SystemManager::add(ISystem* system) {
	systems.push_back(system);
	system->componentManager = componentManager;
	system->deltaTime = deltaTime;
	system->systemManager = this;

	static SYSTEM_IDENTIFIER sysid = 1;
	system->sysid = sysid++;

	systemByIdentifiers.insert({system->getIdentifier(), system});
}
void SystemManager::remove(ISystem* system) {
	//TODO(Kuxe): Implement. Remove from systems and systemByIdentifier
}

void SystemManager::update() {
	for(auto system : systems) {
		if(system->active) {
			system->update();
		}
	}
}

ISystem* SystemManager::getSystem(System::Identifier identifier) {
	ISystem* system;
	try {
		system = systemByIdentifiers.at(identifier);
	} catch(const std::out_of_range& oor) {

		cout << "out_of_range exception caught in SystemManager::getSystem(string identifier): " << oor.what() << endl;
		cout << "1. Maybe there as a typo in the system-identifier argument, " << identifier << "?" << endl;
		cout << "2. Maybe " << identifier << " wasn't added to this SystemManager?" << endl;
		cout << "3. Maybe " << identifier << " was added, but got removed?" << endl;
		cout << "4. Maybe another system didn't implement getIdentifier() (in that case, above error messages are misleading!)" << endl;
		cout << "Things will go wrong from now on!" << endl;
	}


	return system;
}
