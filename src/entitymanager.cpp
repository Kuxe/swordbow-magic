#include "entitymanager.h"
#include "componentmanager.h"
#include "idmanager.h"
#include <iostream>

#include "systemmanager.h"
#include "rendersystem.h"
#include "movesystem.h"
#include "rendercomponent.h"

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
	componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	RenderComponent* renderComponent = componentManager->createRenderComponent(id);

	//If you'd like to change default initialization-data in a component
	//Just save a pointer to the component like above and modify it like bellow
	renderComponent->imagePath = "./resources/images/player.bmp";

	//Tell the entity what systems belongs to
	entities.insert(
		make_pair(
			id,
			vector<ISystem*> {
				systemManager->getSystem("RenderSystem"),
				systemManager->getSystem("MoveSystem"),
			}
		)
	);



	//Insert this entity into the systems
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
	delete id;
}