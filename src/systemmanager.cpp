#include "systemmanager.hpp"
#include "isystem.hpp"
#include "logger.hpp"

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
			try {
				pair.second->update();
			} catch (std::out_of_range oor) {
				std::ostringstream oss;
				oss << "System " << pair.second->getIdentifier() << " failed to update";
				oss << ", this is most likely due to a missing component";
			} 
		}
	}
}

ISystem* SystemManager::getSystem(System::Identifier identifier) {
	ISystem* system;
	try {
		system = systems.at(identifier);
	} catch(const std::out_of_range& oor) {
		const std::string logstr(
			"out_of_range exception caught in SystemManager::getSystem(string identifier): " + std::string(oor.what()) +
			"\n1. Maybe there as a typo in the system-identifier argument, " + std::to_string(identifier) + "?" +
			"\n2. Maybe " + std::to_string(identifier) + " wasn't added to this SystemManager?" +
			"\n3. Maybe " + std::to_string(identifier) + " was added, but got removed?" +
			"\n4. Maybe another system didn't implement getIdentifier() (in that case, above error messages are misleading!)" +
			"\nThings will go wrong from now on!\n"
		);

		Logger::log(logstr, Log::ERROR);
	}


	return system;
}
