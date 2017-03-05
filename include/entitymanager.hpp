#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <glm/glm.hpp>
#include "systemidentifiers.hpp"

/** For network **/
#include "clientdata.hpp"
#include "ipaddress.hpp"

/**
	_The_ EntityManager is respnsible for creating objects.
	It requests objects from componentmanager.
**/

class SystemManager;
class ComponentManager;
class IdManager;
class ISystem;
class PacketManager;

typedef unsigned int ID;

class EntityManager {
 public:
 	/*	Map an ID to some systems. This unordered_map is used when adding an ID to systems
		or when removing ID from systems. Without this list, there would be no way
		of knowing what systems should erase an ID when the ID is removed
		(except for looping through all systems and searching for ID, removing it
		if it exits, but that's a slow bruteforce-approach)
	*/
 	std::unordered_map<unsigned int, std::vector<ISystem*> > entityServerSystemMap;
    std::unordered_set<unsigned int> ids;

	SystemManager* systemManager;
	ComponentManager* componentManager;
	IdManager* idManager;

    std::unordered_map<IpAddress, ClientData>* clients;

    //Required during creation for some commands
    PacketManager* packetManager;

	EntityManager(
        SystemManager* systemManager,
        ComponentManager* componentManager,
        IdManager* idManager,
        std::unordered_map<IpAddress, ClientData>* clients,
        PacketManager* packetManager
    );
    const unsigned int& getId();

	ID createFatMan();
    ID createFatMan(const glm::mat4& transform);
    ID createTree(const glm::mat4& transform);
	ID createGrassTile(const glm::mat4& transform);
    ID createWaterTile(const glm::mat4& transform);
    ID createBloodSplatter(const glm::mat4& transform);
    ID createFlower(const glm::mat4& transform, const char color);
    ID createDummySquare(const glm::mat4& transform);
    ID createStone(const glm::mat4& transform);
    ID createBlueBird(const glm::mat4& transform);
    ID createSwarmPoint(const glm::mat4& transform);
	void remove(ID id);
    bool idExists(ID id);

    //FIXME: This should always be called. system->add should be private but
    //friended by ISystem, or something similar.
    //Never should systemManager->getSystem(System::blabla)->add(id)
    //be called from anything but entitymanager because if registerIdToSystem
    //isnt called, then entitymanager wont know that the entity is part of system
    //so when the entity is erased, it wont get erase from the system and
    //a segfault can occur
    void registerIdToSystem(ID id, System::Identifier system);
};

#endif //ENTITYMANAGER_H
