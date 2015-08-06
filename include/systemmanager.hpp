#ifndef SYSTEMMANGER_H
#define SYSTEMMANGER_H

#include <unordered_map>
#include "systemidentifiers.hpp"

class ISystem;
class ComponentManager;
class DeltaTime;

class SystemManager {
private:
	/* 	SystemManager need to inform systems about
 		ComponentManager so that systems may access ComponentManager
 		in order to aquire the components
 	*/
	ComponentManager* const componentManager;
 	DeltaTime* const deltaTime;
	std::unordered_map<System::Identifier, ISystem*, std::hash<int>> systems;

public:
	SystemManager(ComponentManager* componentManager, DeltaTime* deltaTime);
	void add(ISystem* system);
	void remove(ISystem* system);
	void update();
	ISystem* getSystem(System::Identifier identifier);
};

#endif //SYSTEMMANGER_H
