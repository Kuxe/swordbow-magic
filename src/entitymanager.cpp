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

	cout << "\ndone creating player" << endl;
	return id;
}

void EntityManager::removePlayer(unsigned long long int* id) {
	//1. Remove from systemManager (remove, soon to be, dangling pointers to componenets)
	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->remove(id);
	static_cast<MoveSystem*>(systemManager->getSystem("MoveSystem"))->remove(id);

	//2. Remove from componentManager (deallocate components)
	componentManager->removeAllComponents(id);
	delete id;
}