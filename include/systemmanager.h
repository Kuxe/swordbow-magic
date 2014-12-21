#ifndef SYSTEMMANGER_H
#define SYSTEMMANGER_H

#include <forward_list>
#include <unordered_map>

using namespace std;

class ISystem;
class ComponentManager;

class SystemManager {
 public:
 	/* 	SystemManager need to inform systems about
 		ComponentManager so that systems may access ComponentManager
 		in order to aquire the components
 	*/

 	SystemManager(ComponentManager* componentManager);
 	ComponentManager* const componentManager;
	forward_list<ISystem*> systems;
	unordered_map<string, ISystem*> systemByIdentifiers;
	void add(ISystem* system);
	void remove(ISystem* system);
	void update();
	ISystem* getSystem(string identifier);
};

#endif //SYSTEMMANGER_H