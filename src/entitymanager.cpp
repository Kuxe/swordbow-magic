#include "entitymanager.hpp"
#include "componentmanager.hpp"
#include "idmanager.hpp"
#include <iostream>

#include "systemmanager.hpp"
#include "rendersystem.hpp"
#include "movesystem.hpp"
#include "rendercomponent.hpp"
#include "sizecomponent.hpp"
#include "movecomponent.hpp"
#include "namecomponent.hpp"
#include "hashgridsystem.hpp"
#include "activateid.hpp"
#include "addidtosystem.hpp"
#include "removeidfromsystem.hpp"
#include "playsound.hpp"
#include "soundsystem.hpp"

using namespace std;

EntityManager::EntityManager(
	SystemManager* systemManager,
	ComponentManager* componentManager,
	IdManager* idManager) :
	systemManager(systemManager),
	componentManager(componentManager),
	idManager(idManager) {

}

const unsigned int& EntityManager::getId() {
	auto id = idManager->acquireId();
	ids.insert(id);
	return *ids.find(id);
}

ID EntityManager::createPlayer() {
	//Get unique ID
	auto id = getId();

	//Create components from componentmanager
	auto moveComponent = componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	auto renderComponent = componentManager->createRenderComponent(id);
	auto nameComponent = componentManager->createNameComponent(id);
	auto moveEventComponent = componentManager->createMoveEventComponent(id);
	auto pressKeyEventComponent = componentManager->createPressKeyEventComponent(id);
	auto releaseKeyEventComponent = componentManager->createReleaseKeyEventComponent(id);
	auto soundComponent = componentManager->createSoundComponent(id);

	//If you'd like to change default initialization-data in a component
	//Just save a pointer to the component like above and modify it like bellow
	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/player.bmp");
	renderComponent->zindex_base = 1;
	renderComponent->xoffset = -10;
	renderComponent->yoffset = -10;

	moveComponent->xpos = 10;
	moveComponent->ypos = 10;

	//Width of a player is 20x20
	sizeComponent->width = 20;
	sizeComponent->height = 20;

	nameComponent->name = "player";

	soundComponent->walk.path = "./resources/sounds/walking.wav";
	soundComponent->walk.duration = 250;

	moveEventComponent->addCommand(new ActivateId(id, "RenderSystem", systemManager));
	moveEventComponent->addCommand(new ActivateId(id, "CollisionSystem", systemManager));
	moveEventComponent->addCommand(new ActivateId(id, "TextureHashGridSystem", systemManager));
	moveEventComponent->addCommand(new ActivateId(id, "SizeHashGridSystem", systemManager));
	moveEventComponent->addCommand(new PlaySound(static_cast<SoundSystem*>(systemManager->getSystem("SoundSystem")), soundComponent->walk));
	
	pressKeyEventComponent->addCommand(new AddIdToSystem(id, "MoveSystem", systemManager));
	releaseKeyEventComponent->addCommand(new RemoveIdFromSystem(id, "MoveSystem", systemManager));

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

ID EntityManager::createTree() {
	auto id = idManager->acquireId();

	auto moveComponent = componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	RenderComponent* renderComponent = componentManager->createRenderComponent(id);
	auto nameComponent = componentManager->createNameComponent(id);

	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/SmallTree.png");
	renderComponent->zindex_base = 1;
	renderComponent->xoffset = -24;
	renderComponent->yoffset = -54;

	moveComponent->xpos = 100;
	moveComponent->ypos = 100;
	moveComponent->oldXpos = 100;
	moveComponent->oldYpos = 100;

	sizeComponent->width = 36;
	sizeComponent->height = 10;

	nameComponent->name = "tree";

	entities.insert(
		make_pair(
			id,
			vector<ISystem*> {
				systemManager->getSystem("RenderSystem"),
				systemManager->getSystem("CollisionSystem"),
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
ID EntityManager::createTile() {
	auto id = idManager->acquireId();

	componentManager->createMoveComponent(id);
	componentManager->createTileComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	auto rc = componentManager->createRenderComponent(id);
	auto nameComponent = componentManager->createNameComponent(id);

	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/grass.bmp");
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

void EntityManager::remove(ID id) {
	//1. Remove from systemManager
	for(auto a : entities.at(id)) {
		a->remove(id);
	}

	//2. Remove from componentManager (TODO: reset components)
	entities.erase(id);

	//FIXME: Figure out how and when to remove an entity from hashGrids

	//3. ReleaseId to idmanager
	idManager->releaseId(id);
}
