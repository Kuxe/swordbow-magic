#include "entitymanager.hpp"
#include "componentmanager.hpp"
#include "idmanager.hpp"
#include <iostream>
#include <forward_list>

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

ID EntityManager::createFatMan(FatManData data) {
	//Get unique ID
	auto id = getId();

	//Create components from componentmanager
	auto moveComponent = componentManager->createMoveComponent(id);
	auto inputComponent = componentManager->createInputComponent(id);
	auto sizeComponent = componentManager->createSizeComponent(id);
	auto renderComponent = componentManager->createRenderComponent(id);
	auto nameComponent = componentManager->createNameComponent(id);
	auto moveEventComponent = componentManager->createMoveEventComponent(id);
	auto releaseKeyEventComponent = componentManager->createReleaseKeyEventComponent(id);
	auto soundComponent = componentManager->createSoundComponent(id);
	auto animationComponent = componentManager->createAnimationComponent(id);
	componentManager->createHealthComponent(id);
	componentManager->createRemoveComponent(id);
	componentManager->createAttackComponent(id);
	auto commandComponent = componentManager->createCommandComponent(id);

	//If you'd like to change default initialization-data in a component
	//Just save a pointer to the component like above and modify it like bellow
	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/playerv3_front.png");
	renderComponent->zindex_base = 1;
	renderComponent->xoffset = -10;
	renderComponent->yoffset = -10;

	moveComponent->xpos = data.xpos;
	moveComponent->ypos = data.ypos;
	moveComponent->xspeed = 200;
	moveComponent->yspeed = 200;

	//Width of a player is 20x20
	sizeComponent->width = 20;
	sizeComponent->height = 20;

	nameComponent->name = "player";

	soundComponent->walk.path = "./resources/sounds/walking.wav";
	soundComponent->walk.duration = 250;

	animationComponent->idle.north.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent->idle.northEast.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent->idle.east.frames.push_back("./resources/images/playerv3_right.png");
	animationComponent->idle.southEast.frames.push_back("./resources/images/playerv3_front.png");
	animationComponent->idle.south.frames.push_back("./resources/images/playerv3_front.png");
	animationComponent->idle.southWest.frames.push_back("./resources/images/playerv3_front.png");
	animationComponent->idle.west.frames.push_back("./resources/images/playerv3_left.png");
	animationComponent->idle.northWest.frames.push_back("./resources/images/playerv3_back.png");

	animationComponent->walking.north.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent->walking.northEast.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent->walking.northWest.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent->walking.east.frames.push_back("./resources/images/playerv3_right.png");
	animationComponent->walking.west.frames.push_back("./resources/images/playerv3_left.png");

	animationComponent->walking.southWest.frames.push_back("./resources/images/playerv3_front_run1.png");
	animationComponent->walking.southWest.frames.push_back("./resources/images/playerv3_front_run2.png");
	animationComponent->walking.southWest.frames.push_back("./resources/images/playerv3_front_run3.png");
	animationComponent->walking.southWest.frames.push_back("./resources/images/playerv3_front_run4.png");

	animationComponent->walking.south.frames.push_back("./resources/images/playerv3_front_run1.png");
	animationComponent->walking.south.frames.push_back("./resources/images/playerv3_front_run2.png");
	animationComponent->walking.south.frames.push_back("./resources/images/playerv3_front_run3.png");
	animationComponent->walking.south.frames.push_back("./resources/images/playerv3_front_run4.png");

	animationComponent->walking.southEast.frames.push_back("./resources/images/playerv3_front_run1.png");
	animationComponent->walking.southEast.frames.push_back("./resources/images/playerv3_front_run2.png");
	animationComponent->walking.southEast.frames.push_back("./resources/images/playerv3_front_run3.png");
	animationComponent->walking.southEast.frames.push_back("./resources/images/playerv3_front_run4.png");

	moveEventComponent->addCommand(new ActivateId(id, "RenderSystem", systemManager));
	moveEventComponent->addCommand(new ActivateId(id, "CollisionSystem", systemManager));
	moveEventComponent->addCommand(new ActivateId(id, "TextureHashGridSystem", systemManager));
	moveEventComponent->addCommand(new ActivateId(id, "SizeHashGridSystem", systemManager));
	moveEventComponent->addCommand(new PlaySound(static_cast<SoundSystem*>(systemManager->getSystem("SoundSystem")), soundComponent->walk));

	inputComponent->bindings.insert(make_pair(119 /*SDLK_W*/, 1 /*move up*/));
	inputComponent->bindings.insert(make_pair(100 /*SDLK_D*/, 2 /*move right*/));
	inputComponent->bindings.insert(make_pair(97 /*SDLK_S*/, 3 /*move down*/));
	inputComponent->bindings.insert(make_pair(115 /*SDLK_A*/, 4 /*move left*/));
	inputComponent->bindings.insert(make_pair(32 /*SDLK_SPACE*/, 5 /*attack*/));

	commandComponent->commands.insert(make_pair(
		1,
		forward_list<ICommand*> {
			new AddIdToSystem(id, "MoveSystem", systemManager),
		}
	));

	commandComponent->commands.insert(make_pair(
		2,
		forward_list<ICommand*> {
			new AddIdToSystem(id, "MoveSystem", systemManager),
		}
	));

	commandComponent->commands.insert(make_pair(
		3,
		forward_list<ICommand*> {
			new AddIdToSystem(id, "MoveSystem", systemManager),
		}
	));

	commandComponent->commands.insert(make_pair(
		4,
		forward_list<ICommand*> {
			new AddIdToSystem(id, "MoveSystem", systemManager),
		}
	));

	commandComponent->commands.insert(make_pair(
		5,
		forward_list<ICommand*> {
			new ActivateId(id, "AttackSystem", systemManager),
		}
	));

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
				systemManager->getSystem("AnimationSystem"),
				systemManager->getSystem("HealthSystem"),
				systemManager->getSystem("RemoveSystem"),
				systemManager->getSystem("AttackSystem"),
				systemManager->getSystem("InputSystem"),
			}
		)
	);



	//Insert this entity into the systems
	for(auto a : entities.at(id)) {
		a->add(id);
	}
	return id;
}

ID EntityManager::createFatMan() {
	return createFatMan({10, 10});
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
	for(auto system : entities.at(id)) {
		system->remove(id);
	}

	//Remove from entitymanager
	entities.erase(id);

	//2. Remove from componentManager
	componentManager->clearComponents(id);

	//3. ReleaseId to idmanager
	idManager->releaseId(id);
}
