#include "entitymanager.h"
#include "componentmanager.h"
#include "idmanager.h"
#include <iostream>

#include "systemmanager.h"
#include "rendersystem.h"
#include "movesystem.h"
#include "rendercomponent.h"
#include "sizecomponent.h"
#include "movecomponent.h"
#include "namecomponent.h"
#include "hashgridsystem.h"

using namespace std;

EntityManager::EntityManager(
	SystemManager* systemManager,
	ComponentManager* componentManager,
	IdManager* idManager) :
	systemManager(systemManager),
	componentManager(componentManager),
	idManager(idManager) {

}

unsigned long long int* EntityManager::createPlayer() {
	//Get unique ID
	auto id = idManager->getId();

	//Create components from componentmanager
	auto moveComponent = componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	RenderComponent* renderComponent = componentManager->createRenderComponent(id);
	auto nameComponent = componentManager->createNameComponent(id);

	//If you'd like to change default initialization-data in a component
	//Just save a pointer to the component like above and modify it like bellow
	renderComponent->imagePath = "./resources/images/player.bmp";
	renderComponent->zindex_base = 1;
	renderComponent->xoffset = -10;
	renderComponent->yoffset = -10;

	moveComponent->xpos = 20;
	moveComponent->ypos = 20;

	//Width of a player is 20x20
	sizeComponent->width = 20;
	sizeComponent->height = 20;

	nameComponent->name = "player";

	//Tell the entity what systems belongs to
	entities.insert(
		make_pair(
			id,
			vector<ISystem*> {
				systemManager->getSystem("RenderSystem"),
				systemManager->getSystem("MoveSystem"),
				systemManager->getSystem("CollisionSystem"),
				systemManager->getSystem("TextureHashGridSystem"),
				systemManager->getSystem("SizeHashGridSystem"),
			}
		)
	);



	//Insert this entity into the systems
	for(auto a : entities.at(id)) {
		a->add(id);
	}
	return id;
}

unsigned long long int* EntityManager::createTree() {
	auto id = idManager->getId();

	auto moveComponent = componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	RenderComponent* renderComponent = componentManager->createRenderComponent(id);
	auto nameComponent = componentManager->createNameComponent(id);

	renderComponent->imagePath = "./resources/images/SmallTree.png";
	renderComponent->zindex_base = 1;
	renderComponent->xoffset = -24;
	renderComponent->yoffset = -54;

	moveComponent->xpos = 100;
	moveComponent->ypos = 100;

	sizeComponent->width = 36;
	sizeComponent->height = 10;

	nameComponent->name = "tree";

	entities.insert(
		make_pair(
			id,
			vector<ISystem*> {
				systemManager->getSystem("RenderSystem"),
				systemManager->getSystem("CollisionSystem"),
				systemManager->getSystem("MoveSystem"),
				systemManager->getSystem("TextureHashGridSystem"),
				systemManager->getSystem("SizeHashGridSystem"),
			}
		)
	);

	for(auto a : entities.at(id)) {
		a->add(id);
	}
	return id;
}
unsigned long long int* EntityManager::createTile() {
	auto id = idManager->getId();

	componentManager->createMoveComponent(id);
	componentManager->createTileComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	auto rc = componentManager->createRenderComponent(id);
	auto nameComponent = componentManager->createNameComponent(id);

	rc->imagePath = "./resources/images/grass.bmp";
	rc->zindex_base = 0;

	sizeComponent->width = 20;
	sizeComponent->height = 20;

	nameComponent->name = "tile";

	entities.insert(
		make_pair(
			id,
			vector<ISystem*> {
				systemManager->getSystem("RenderSystem"),
				systemManager->getSystem("TextureHashGridSystem"),
			}
		)
	);

	for(auto a : entities.at(id)) {
		a->add(id);
	}
	return id;
}

void EntityManager::remove(unsigned long long int* id) {
	//1. Remove from systemManager
	for(auto a : entities.at(id)) {
		a->remove(id);
	}

	//2. Remove from componentManager (deallocate components)
	componentManager->removeAllComponents(id);
	entities.erase(id);

	//FIXME: Figure out how and when to remove an entity from hashGrids

	delete id;
}
