#ifndef SYSTEMMANGER_H
#define SYSTEMMANGER_H

#include <unordered_map>
#include <vector>

using namespace std;

class ISystem;
class ComponentManager;
class DeltaTime;

class SystemManager {
 public:
 	/* 	SystemManager need to inform systems about
 		ComponentManager so that systems may access ComponentManager
 		in order to aquire the components
 	*/

 	SystemManager(ComponentManager* componentManager, DeltaTime* deltaTime);
 	ComponentManager* const componentManager;
 	DeltaTime* const deltaTime;
	vector<ISystem*> systems;
	unordered_map<string, ISystem*> systemByIdentifiers;
	void add(ISystem* system);
	void remove(ISystem* system);
	void update();
	ISystem* getSystem(string identifier);
};

#endif //SYSTEMMANGER_H
