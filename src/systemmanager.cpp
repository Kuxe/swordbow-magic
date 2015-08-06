#include "systemmanager.hpp"
#include "isystem.hpp"
#include <iostream>

SystemManager::SystemManager(ComponentManager* componentManager, DeltaTime* deltaTime) :
	componentManager(componentManager),
	deltaTime(deltaTime) {

}

void SystemManager::add(ISystem* system) {
	systems.insert({system->getIdentifier(), system});
	system->componentManager = componentManager;
	system->deltaTime = deltaTime;
	system->systemManager = this;
}

void SystemManager::remove(ISystem* system) {
	//TODO(Kuxe): Implement. Remove from systems and systemByIdentifier
}

void SystemManager::update() {
	for(auto pair : systems) {
		if(pair.second->active) {
			pair.second->update();
		}
	}
}

ISystem* SystemManager::getSystem(System::Identifier identifier) {
	ISystem* system;
	try {
		system = systems.at(identifier);
	} catch(const std::out_of_range& oor) {

		std::cout << "out_of_range exception caught in SystemManager::getSystem(string identifier): " << oor.what() << std::endl;
		std::cout << "1. Maybe there as a typo in the system-identifier argument, " << identifier << "?" << std::endl;
		std::cout << "2. Maybe " << identifier << " wasn't added to this SystemManager?" << std::endl;
		std::cout << "3. Maybe " << identifier << " was added, but got removed?" << std::endl;
		std::cout << "4. Maybe another system didn't implement getIdentifier() (in that case, above error messages are misleading!)" << std::endl;
		std::cout << "Things will go wrong from now on!" << std::endl;
	}


	return system;
}
