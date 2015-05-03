#ifndef SYSTEMMANGER_H
#define SYSTEMMANGER_H

#include <forward_list>
#include <unordered_map>

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

 	static constexpr unsigned char MAX_SYSTEMS = 255;

 	SystemManager(ComponentManager* componentManager, DeltaTime* deltaTime);
 	ComponentManager* const componentManager;
 	DeltaTime* const deltaTime;
	//forward_list<ISystem*> systems;
	ISystem* systems[MAX_SYSTEMS] {0};
	unordered_map<string, ISystem*> systemByIdentifiers;
	void add(ISystem* system);
	void remove(ISystem* system);
	void update();
	ISystem* getSystem(string identifier);
};

#endif //SYSTEMMANGER_H