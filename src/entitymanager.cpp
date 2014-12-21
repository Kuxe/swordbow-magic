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
	cout << "\ncreating player..." << endl;


	//Get unique ID
	auto id = idManager->getId();

	//Get components from componentmanager
	MoveComponent* moveComponent = componentManager->createMoveComponent(id);
	RenderComponent* renderComponent = componentManager->createRenderComponent(id);

	//Add components to some systems via systemamanger
	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->add(id, {renderComponent, moveComponent});
	static_cast<MoveSystem*>(systemManager->getSystem("MoveSystem"))->add(id, {moveComponent});

	entities.insert(make_pair(
		id,
		vector<ISystem*> {
			systemManager->getSystem("RenderSystem"),
			systemManager->getSystem("MoveSystem")
	}));

	cout << "\ndone creating player" << endl;
	return id;
}

void EntityManager::remove(unsigned long long int* id) {
	//1. Remove from systemManager
	for(auto a : entities.at(id)) {
		a->remove(id);
	}

	//2. Remove from componentManager (deallocate components)
	componentManager->removeAllComponents(id);
	delete id;
}