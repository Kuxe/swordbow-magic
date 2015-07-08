#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <time.h>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "systemidentifiers.hpp"

using namespace std;

/**
	_The_ EntityManager is respnsible for creating objects.
	It requests objects from componentmanager.
**/

class SystemManager;
class ComponentManager;
class IdManager;
class ISystem;
class Client;
class Socket;

typedef unsigned int ID;

class EntityManager {
 public:
 	/*	Map an ID to some systems. This unordered_map is used when adding an ID to systems
		or when removing ID from systems. Without this list, there would be no way
		of knowing what systems should erase an ID when the ID is removed
		(except for looping through all systems and searching for ID, removing it
		if it exits, but that's a slow bruteforce-approach)
	*/
 	unordered_map<unsigned int, vector<ISystem*> > entityServerSystemMap;
    unordered_map<unsigned int, vector<System> > entityClientSystemMap;
    unordered_set<unsigned int> ids;

	SystemManager* systemManager;
	ComponentManager* componentManager;
	IdManager* idManager;

    //If entitymanager creates anything, it need to inform all clients connected
    //to the parent server that a new entity was created and it belongs to
    //"these" systems (eg RenderSystem and TextureHashGridSystem)
    std::unordered_map<unsigned int, ID>* clients;

    //Required during creation for some commands
    Socket* socket;

	EntityManager(
        SystemManager* systemManager,
        ComponentManager* componentManager,
        IdManager* idManager,
        std::unordered_map<unsigned int, ID>* clients,
        Socket* socket
    );
    const unsigned int& getId();

	ID createFatMan();
    ID createFatMan(const glm::vec2& position);
    ID createTree(const glm::vec2& position);
	ID createGrassTile(const glm::vec2& position);
    ID createWaterTile(const glm::vec2& position);
    ID createBloodSplatter(const glm::vec2& position);
    ID createFlower(const glm::vec2& position, const char color);
    ID createDummySquare(const glm::vec2& poisiton);
    ID createStone(const glm::vec2& position);
	void remove(ID id);

    void registerIdToSystem(ID id, System system);
    void registerIdToRemoteSystem(ID id, System system);
};

#endif //ENTITYMANAGER_H
