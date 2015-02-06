#include "entitymanager.h"
#include "componentmanager.h"
#include "idmanager.h"
#include <iostream>

#include "systemmanager.h"
#include "rendersystem.h"
#include "movesystem.h"
#include "rendercomponent.h"
#include "sizecomponent.h"
#include "gridindexer.h"

using namespace std;

EntityManager::EntityManager(
	SystemManager* systemManager,
	ComponentManager* componentManager,
	IdManager* idManager,
	GridIndexer* gridIndexer) :
	systemManager(systemManager),
	componentManager(componentManager),
	idManager(idManager),
	gridIndexer(gridIndexer) {

}

unsigned long long int* EntityManager::createPlayer() {
	//Get unique ID
	auto id = idManager->getId();

	//Create components from componentmanager
	componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	componentManager->createFlagComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	RenderComponent* renderComponent = componentManager->createRenderComponent(id);
	gridIndexer->add(id);

	//If you'd like to change default initialization-data in a component
	//Just save a pointer to the component like above and modify it like bellow
	renderComponent->imagePath = "./resources/images/player.bmp";
	renderComponent->zindex = 1;

	//Width of a player is 20x20
	sizeComponent->width = 20;
	sizeComponent->height = 20;

	//Tell the entity what systems belongs to
	entities.insert(
		make_pair(
			id,
			vector<ISystem*> {
				systemManager->getSystem("RenderSystem"),
				systemManager->getSystem("MoveSystem"),
				systemManager->getSystem("FlagSystem"),
			}
		)
	);



	//Insert this entity into the systems
	for(auto a : entities.at(id)) {
		a->add(id);
	} 
	return id;
}

unsigned long long int* EntityManager::createTile() {
	auto id = idManager->getId();

	componentManager->createMoveComponent(id);
	componentManager->createTileComponent(id);
	componentManager->createFlagComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	auto rc = componentManager->createRenderComponent(id);
	gridIndexer->add(id);

	rc->imagePath = "./resources/images/grass.bmp";

	sizeComponent->width = 20;
	sizeComponent->height = 20;

	entities.insert(
		make_pair(
			id,
			vector<ISystem*> {
				systemManager->getSystem("RenderSystem"),
				systemManager->getSystem("FlagSystem"),
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

	//Remove from GridIndexer (assuming gridindexer handles removing unadded ids well)
	gridIndexer->remove(id);

	//2. Remove from componentManager (deallocate components)
	componentManager->removeAllComponents(id);
	entities.erase(id);
	delete id;
}