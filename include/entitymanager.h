#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <time.h>
#include <unordered_map>
#include <string>

using namespace std;

/**
	_The_ EntityManager is respnsible for creating objects.
	It requests objects from componentmanager.
**/

class SystemManager;
class ComponentManager;
class IdManager;

class EntityManager {
 public:

	SystemManager* systemManager;
	ComponentManager* componentManager;
	IdManager* idManager;

	EntityManager(SystemManager* systemManager, ComponentManager* componentManager, IdManager* idManager);
	unsigned long long int* createPlayer();
	void removePlayer(unsigned long long int*);
};

#endif //ENTITYMANAGER_H