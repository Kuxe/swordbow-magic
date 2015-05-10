#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <time.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

using namespace std;

/**
	_The_ EntityManager is respnsible for creating objects.
	It requests objects from componentmanager.
**/

class SystemManager;
class ComponentManager;
class IdManager;
class ISystem;

typedef unsigned int ID;

class EntityManager {
 public:
 	/*	Map an ID to some systems. This unordered_map is used when adding an ID to systems
		or when removing ID from systems. Without this list, there would be no way
		of knowing what systems should erase an ID when the ID is removed
		(except for looping through all systems and searching for ID, removing it
		if it exits, but that's a slow bruteforce-approach)
	*/
 	unordered_map<unsigned int, vector<ISystem*> > entities;
    unordered_set<unsigned int> ids;

	SystemManager* systemManager;
	ComponentManager* componentManager;
	IdManager* idManager;

	EntityManager(SystemManager* systemManager, ComponentManager* componentManager, IdManager* idManager);
    const unsigned int& getId();

    struct FatManData {
        float xpos, ypos;
    };
	ID createFatMan();
    ID createFatMan(FatManData data);
	ID createTree();
	ID createTile();
	void remove(ID id);
};

#endif //ENTITYMANAGER_H
