#include "componentmanager.hpp"

void ComponentManager::clearComponents(ID id) {
    Logger::log("Clearing components of " + std::to_string(id), Log::VERBOSE);
	moveComponents.erase(id);
	renderComponents.erase(id);
	inputComponents.erase(id);
	tileComponents.erase(id);
	sizeComponents.erase(id);
    nameComponents.erase(id);
    soundComponents.erase(id);
    animationComponents.erase(id);
    healthComponents.erase(id);
    attackComponents.erase(id);
    commandComponents.erase(id);
    accelerationComponents.erase(id);
};
