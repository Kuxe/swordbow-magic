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
#include "createbloodsplatter.hpp"

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

ID EntityManager::createFatMan(const glm::vec2& position) {
	//Get unique ID
	auto id = getId();

	//Create components from componentmanager
	auto& moveComponent = componentManager->createMoveComponent(id);
	auto& inputComponent = componentManager->createInputComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& renderComponent = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);
	auto& soundComponent = componentManager->createSoundComponent(id);
	auto& animationComponent = componentManager->createAnimationComponent(id);
	componentManager->createHealthComponent(id);
	componentManager->createAttackComponent(id);
	auto& commandComponent = componentManager->createCommandComponent(id);

	//If you'd like to change default initialization-data in a component
	//Just save a pointer to the component like above and modify it like bellow
	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/playerv3_front.png");
	renderComponent.zindex_base = 1;
	renderComponent.xoffset = -7;
	renderComponent.yoffset = -7;

	moveComponent.pos = position;
	moveComponent.vel.x = 200;
	moveComponent.vel.y = 200;

	//Width of a player is 20x20
	sizeComponent.width = 20;
	sizeComponent.height = 20;

	nameComponent.name = "player";

	soundComponent.walk.path = "./resources/sounds/walking.wav";
	soundComponent.walk.duration = 250;

	animationComponent.idle.north.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent.idle.northEast.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent.idle.east.frames.push_back("./resources/images/playerv3_right.png");
	animationComponent.idle.southEast.frames.push_back("./resources/images/playerv3_front.png");
	animationComponent.idle.south.frames.push_back("./resources/images/playerv3_front.png");
	animationComponent.idle.southWest.frames.push_back("./resources/images/playerv3_front.png");
	animationComponent.idle.west.frames.push_back("./resources/images/playerv3_left.png");
	animationComponent.idle.northWest.frames.push_back("./resources/images/playerv3_back.png");

	animationComponent.walking.north.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent.walking.northEast.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent.walking.northWest.frames.push_back("./resources/images/playerv3_back.png");
	animationComponent.walking.east.frames.push_back("./resources/images/playerv3_right.png");
	animationComponent.walking.west.frames.push_back("./resources/images/playerv3_left.png");

	animationComponent.walking.southWest.frames.push_back("./resources/images/playerv3_front_run1.png");
	animationComponent.walking.southWest.frames.push_back("./resources/images/playerv3_front_run2.png");
	animationComponent.walking.southWest.frames.push_back("./resources/images/playerv3_front_run3.png");
	animationComponent.walking.southWest.frames.push_back("./resources/images/playerv3_front_run4.png");

	animationComponent.walking.south.frames.push_back("./resources/images/playerv3_front_run1.png");
	animationComponent.walking.south.frames.push_back("./resources/images/playerv3_front_run2.png");
	animationComponent.walking.south.frames.push_back("./resources/images/playerv3_front_run3.png");
	animationComponent.walking.south.frames.push_back("./resources/images/playerv3_front_run4.png");

	animationComponent.walking.southEast.frames.push_back("./resources/images/playerv3_front_run1.png");
	animationComponent.walking.southEast.frames.push_back("./resources/images/playerv3_front_run2.png");
	animationComponent.walking.southEast.frames.push_back("./resources/images/playerv3_front_run3.png");
	animationComponent.walking.southEast.frames.push_back("./resources/images/playerv3_front_run4.png");

	animationComponent.state = &animationComponent.idle.south;

	inputComponent.bindings[119] = CommandComponent::Event::MOVE_UP;
	inputComponent.bindings[100] = CommandComponent::Event::MOVE_RIGHT;
	inputComponent.bindings[97] = CommandComponent::Event::MOVE_DOWN;
	inputComponent.bindings[115] = CommandComponent::Event::MOVE_LEFT;
	inputComponent.bindings[32] = CommandComponent::Event::ATTACK;

	commandComponent[CommandComponent::Event::MOVE_UP] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	commandComponent[CommandComponent::Event::MOVE_RIGHT] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	commandComponent[CommandComponent::Event::MOVE_DOWN] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	commandComponent[CommandComponent::Event::MOVE_LEFT] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	commandComponent[CommandComponent::Event::ATTACK] = {
		new ActivateId(id, "AttackSystem", systemManager),
	};

	commandComponent[CommandComponent::Event::ON_DEATH] = {
		new PlaySound(static_cast<SoundSystem*>(systemManager->getSystem("SoundSystem")), SoundComponent::Sound {"./resources/sounds/bloodsplatter.wav"}),
		new CreateBloodsplatter(this, id),
	};

	commandComponent[CommandComponent::Event::ON_MOVE] = {
		new ActivateId(id, "RenderSystem", systemManager),
		new ActivateId(id, "CollisionSystem", systemManager),
		new ActivateId(id, "TextureHashGridSystem", systemManager),
		new ActivateId(id, "SizeHashGridSystem", systemManager),
		new PlaySound(static_cast<SoundSystem*>(systemManager->getSystem("SoundSystem")), soundComponent.walk),
	};

	//Tell the entity what systems belongs to
	entities[id] = {
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
	};



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

	auto& moveComponent = componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& renderComponent = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/SmallTree.png");
	renderComponent.zindex_base = 1;
	renderComponent.xoffset = -24;
	renderComponent.yoffset = -54;

	moveComponent.pos.x = 100;
	moveComponent.pos.y = 100;
	moveComponent.oldPos.x = 100;
	moveComponent.oldPos.y = 100;

	sizeComponent.width = 36;
	sizeComponent.height = 10;

	nameComponent.name = "tree";

	entities[id] = {
		systemManager->getSystem("RenderSystem"),
		systemManager->getSystem("CollisionSystem"),
		systemManager->getSystem("TextureHashGridSystem"),
		systemManager->getSystem("SizeHashGridSystem"),
	};

	for(auto a : entities.at(id)) {
		a->add(id);
	}
	return id;
}
ID EntityManager::createTile() {
	auto id = idManager->acquireId();

	componentManager->createMoveComponent(id);
	componentManager->createTileComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/grass.bmp");
	rc.zindex_base = 0;

	sizeComponent.width = 20;
	sizeComponent.height = 20;

	nameComponent.name = "tile";

	entities[id] = {
		systemManager->getSystem("RenderSystem"),
		systemManager->getSystem("TextureHashGridSystem"),
	};

	for(auto a : entities.at(id)) {
		a->add(id);
	}
	return id;
}

ID EntityManager::createBloodSplatter(const glm::vec2& position) {
	auto id = idManager->acquireId();
	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& ac = componentManager->createAnimationComponent(id);
	static_cast<RenderSystem*>(systemManager->getSystem("RenderSystem"))->setImage(id, "./resources/images/bloodsplatter1_11.png");

	rc.xoffset = -10;
	rc.yoffset = -10;
	rc.zindex_base = 1;

	mc.pos = position;

	nc.name = "bloodsplatter";

	ac.bloodsplatter.duration = 25;
	ac.bloodsplatter.looping = false;

	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_1.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_2.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_3.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_4.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_5.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_6.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_7.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_8.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_9.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_10.png");
	ac.bloodsplatter.frames.push_back("./resources/images/bloodsplatter1_11.png");

	/** CRASHING ON THIS **/
	ac.state = &ac.bloodsplatter;

	entities[id] = {
		systemManager->getSystem("RenderSystem"),
		systemManager->getSystem("TextureHashGridSystem"),
		systemManager->getSystem("AnimationSystem"),
	};

	for(auto system : entities.at(id)) {
		system->add(id);
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

void EntityManager::registerIdToSystem(const string systemIdentifier, ID id) {
	auto system = systemManager->getSystem(systemIdentifier);
	entities.at(id).push_back(system);
	system->add(id);
}
