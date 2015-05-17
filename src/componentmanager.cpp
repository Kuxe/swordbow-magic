#include "componentmanager.hpp"

void ComponentManager::clearComponents(ID id) {
	moveComponents.erase(id);
	renderComponents.erase(id);
	inputComponents.erase(id);
	tileComponents.erase(id);
	sizeComponents.erase(id);
    nameComponents.erase(id);
    moveEventComponents.erase(id);
    soundComponents.erase(id);
    animationComponents.erase(id);
    healthComponents.erase(id);
    removeComponents.erase(id);
    attackComponents.erase(id);
    commandComponents.erase(id);
};
