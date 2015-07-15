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
#include "activateidonclients.hpp"
#include "addidtosystem.hpp"
#include "removeidfromsystem.hpp"
#include "playsound.hpp"
#include "createbloodsplatter.hpp"

#include "client.hpp"

using namespace std;

EntityManager::EntityManager(
	SystemManager* systemManager,
	ComponentManager* componentManager,
	IdManager* idManager,
	std::unordered_map<IpAddress, ID>* clients,
	Socket* socket) :
	systemManager(systemManager),
	componentManager(componentManager),
	idManager(idManager),
	clients(clients),
	socket(socket) {

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
	renderComponent.image = Image::PLAYER_V3_FRONT;
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

	soundComponent.walk.sound = Sound::WALKING;
	soundComponent.walk.duration = 250;

	animationComponent.idle.north.frames.push_back(Image::PLAYER_V3_BACK);
	animationComponent.idle.northEast.frames.push_back(Image::PLAYER_V3_BACK);
	animationComponent.idle.east.frames.push_back(Image::PLAYER_V3_RIGHT);
	animationComponent.idle.southEast.frames.push_back(Image::PLAYER_V3_FRONT);
	animationComponent.idle.south.frames.push_back(Image::PLAYER_V3_FRONT);
	animationComponent.idle.southWest.frames.push_back(Image::PLAYER_V3_FRONT);
	animationComponent.idle.west.frames.push_back(Image::PLAYER_V3_LEFT);
	animationComponent.idle.northWest.frames.push_back(Image::PLAYER_V3_BACK);

	animationComponent.walking.north.frames.push_back(Image::PLAYER_V3_BACK);
	animationComponent.walking.northEast.frames.push_back(Image::PLAYER_V3_BACK);
	animationComponent.walking.northWest.frames.push_back(Image::PLAYER_V3_BACK);
	animationComponent.walking.east.frames.push_back(Image::PLAYER_V3_RIGHT);
	animationComponent.walking.west.frames.push_back(Image::PLAYER_V3_LEFT);

	animationComponent.walking.southWest.frames.push_back(Image::PLAYER_V3_FRONT_RUN1);
	animationComponent.walking.southWest.frames.push_back(Image::PLAYER_V3_FRONT_RUN2);
	animationComponent.walking.southWest.frames.push_back(Image::PLAYER_V3_FRONT_RUN3);
	animationComponent.walking.southWest.frames.push_back(Image::PLAYER_V3_FRONT_RUN4);

	animationComponent.walking.south.frames.push_back(Image::PLAYER_V3_FRONT_RUN1);
	animationComponent.walking.south.frames.push_back(Image::PLAYER_V3_FRONT_RUN2);
	animationComponent.walking.south.frames.push_back(Image::PLAYER_V3_FRONT_RUN3);
	animationComponent.walking.south.frames.push_back(Image::PLAYER_V3_FRONT_RUN4);

	animationComponent.walking.southEast.frames.push_back(Image::PLAYER_V3_FRONT_RUN1);
	animationComponent.walking.southEast.frames.push_back(Image::PLAYER_V3_FRONT_RUN2);
	animationComponent.walking.southEast.frames.push_back(Image::PLAYER_V3_FRONT_RUN3);
	animationComponent.walking.southEast.frames.push_back(Image::PLAYER_V3_FRONT_RUN4);

	animationComponent.state = &animationComponent.idle.south;

	inputComponent.bindings[119] = CommandComponent::Event::MOVE_UP;
	inputComponent.bindings[100] = CommandComponent::Event::MOVE_RIGHT;
	inputComponent.bindings[97] = CommandComponent::Event::MOVE_DOWN;
	inputComponent.bindings[115] = CommandComponent::Event::MOVE_LEFT;
	inputComponent.bindings[32] = CommandComponent::Event::ATTACK;

	commandComponent[CommandComponent::Event::MOVE_UP] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	commandComponent[CommandComponent::Event::MOVE_RIGHT] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	commandComponent[CommandComponent::Event::MOVE_DOWN] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	commandComponent[CommandComponent::Event::MOVE_LEFT] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	commandComponent[CommandComponent::Event::ATTACK] = {
		new ActivateId(id, System::ATTACK, systemManager),
	};

	commandComponent[CommandComponent::Event::ON_DEATH] = {
		new PlaySound(
			SoundComponent::SoundData {Sound::BLOODSPLATTER},
			clients,
			socket
		),
		new CreateBloodsplatter(this, id),
	};

	commandComponent[CommandComponent::Event::ON_MOVE] = {
		//Perhaps use a "new ActivateIdOnClients(id, "RenderSystem")"?
		new ActivateIdOnClients(id, System::RENDER, socket, clients),
		new ActivateId(id, System::COLLISION, systemManager),
		new ActivateIdOnClients(id, System::HASHGRID_TEXTURE, socket, clients),
		new ActivateId(id, System::HASHGRID_SIZE, systemManager),
		new PlaySound(soundComponent.walk, clients, socket),
	};

	registerIdToSystem(id, System::MOVE);
	registerIdToSystem(id, System::COLLISION);
	registerIdToSystem(id, System::HASHGRID_SIZE);
	registerIdToSystem(id, System::HEALTH);
	registerIdToSystem(id, System::REMOVE);
	registerIdToSystem(id, System::ATTACK);
	registerIdToSystem(id, System::INPUT);
	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);
	registerIdToSystem(id, System::ANIMATION);

	return id;
}

ID EntityManager::createFatMan() {
	return createFatMan({10, 10});
}

ID EntityManager::createTree(const glm::vec2& position) {
	auto id = idManager->acquireId();

	auto& moveComponent = componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& renderComponent = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	renderComponent.image = Image::SMALL_TREE;
	renderComponent.zindex_base = 1;
	renderComponent.xoffset = -24;
	renderComponent.yoffset = -54;

	moveComponent.pos = position;
	moveComponent.oldPos = moveComponent.pos;

	sizeComponent.width = 36;
	sizeComponent.height = 10;

	nameComponent.name = "tree";

	registerIdToSystem(id, System::COLLISION);
	registerIdToSystem(id, System::HASHGRID_SIZE);
	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);

	return id;
}
ID EntityManager::createGrassTile(const glm::vec2& position) {
	auto id = idManager->acquireId();

	auto& mc = componentManager->createMoveComponent(id);
	auto& tc = componentManager->createTileComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	rc.image = Image::GRASS;
	rc.zindex_base = 0;

	mc.pos = position;
	mc.oldPos = mc.pos;

	sizeComponent.width = 32;
	sizeComponent.height = 32;

	nameComponent.name = "grasstile";

	//Systems on clients need to be aware of these
	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);

	return id;
}

ID EntityManager::createWaterTile(const glm::vec2& position) {
	auto id = idManager->acquireId();

	auto& mc = componentManager->createMoveComponent(id);
	componentManager->createTileComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	rc.image = Image::WATER;
	rc.zindex_base = 0;

	mc.pos = position;
	mc.oldPos = mc.pos;

	sizeComponent.width = 32;
	sizeComponent.height = 32;

	nameComponent.name = "watertile";

	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);

	return id;
}

ID EntityManager::createBloodSplatter(const glm::vec2& position) {
	auto id = idManager->acquireId();
	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& ac = componentManager->createAnimationComponent(id);

	rc.image = Image::BLOODSPLATTER_1_11;
	rc.xoffset = -10;
	rc.yoffset = -10;
	rc.zindex_base = 1;

	mc.pos = position;

	nc.name = "bloodsplatter";

	ac.bloodsplatter.duration = 25;
	ac.bloodsplatter.looping = false;

	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_1);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_2);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_3);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_4);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_5);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_6);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_7);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_8);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_9);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_10);
	ac.bloodsplatter.frames.push_back(Image::BLOODSPLATTER_1_11);

	ac.state = &ac.bloodsplatter;

	//FIXME: These are getting registered on remote systems.
	//It could be possible that the remote systems iterate
	//over registered entities BEFORE the remote client
	//got the components, which would cause a segfault
	//This is probably the case when killing a character
	//which emits a new entity (blood). That blood is getting
	//registered to client, client iterate over it and
	//fetches movecomponent before
	//server did send movecomponent of that id to client
	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);
	registerIdToSystem(id, System::ANIMATION);

	return id;
}

ID EntityManager::createFlower(const glm::vec2& position, const char color) {
	auto id = idManager->acquireId();
	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nc = componentManager->createNameComponent(id);

	Image::Identifier flowercolor = Image::UNDEFINED;

	//Set image corresponding to color
	switch(color) {
		case 0: {
			flowercolor = Image::YELLOW_FLOWER;
		} break;
		case 1: {
			flowercolor = Image::BLUE_FLOWER;
		} break;
		case 2: {
			flowercolor = Image::VIOLET_FLOWER;
		} break;
		case 3: {
			flowercolor = Image::GREEN_FLOWER;
		} break;
	}

	rc.image = flowercolor;
	rc.xoffset = -1;
	rc.yoffset = -5;
	rc.zindex_base = 1;

	mc.pos = position;

	nc.name = "flower";

	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);

	return id;
}

ID EntityManager::createDummySquare(const glm::vec2& position) {
	auto id = idManager->acquireId();
	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& ic = componentManager->createInputComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& cc = componentManager->createCommandComponent(id);
	auto& ac = componentManager->createAnimationComponent(id);

	rc.image = Image::TEST_SQUARE_1x1;

	mc.maxVelLength = 4.0f;

	rc.xoffset = 0;
	rc.yoffset = 0;
	rc.zindex_base = 1;

	mc.pos = position;

	nc.name = "testsquare1x1";

	cc[CommandComponent::Event::MOVE_UP] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	cc[CommandComponent::Event::MOVE_RIGHT] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	cc[CommandComponent::Event::MOVE_DOWN] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	cc[CommandComponent::Event::MOVE_LEFT] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	cc[CommandComponent::Event::ON_MOVE] = {
		new ActivateIdOnClients(id, System::RENDER, socket, clients),
		new ActivateIdOnClients(id, System::HASHGRID_TEXTURE, socket, clients),
	};

	registerIdToSystem(id, System::MOVE);
	registerIdToSystem(id, System::INPUT);
	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);

	return id;
}

ID EntityManager::createStone(const glm::vec2& position) {
	auto id = idManager->acquireId();
	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& sc = componentManager->createSizeComponent(id);
	auto& nc = componentManager->createNameComponent(id);


	mc.pos = position;
	mc.oldPos = mc.pos;

	rc.image = Image::STONE_1;
	rc.xoffset = -7;
	rc.yoffset = -12;
	rc.zindex_base = 1;

	sc.width = 28;
	sc.height = 15;

	nc.name = "stone";

	registerIdToSystem(id, System::COLLISION);
	registerIdToSystem(id, System::HASHGRID_SIZE);
	registerIdToRemoteSystem(id, System::RENDER);
	registerIdToRemoteSystem(id, System::HASHGRID_TEXTURE);

	return id;
}

void EntityManager::remove(ID id) {
	//1. Remove from server systemManager
	for(auto system : entityServerSystemMap.at(id)) {
		system->remove(id);
	}
	entityServerSystemMap.erase(id);

	//2. Remove from clients systemManagers
	for(auto systemIdentifier : entityClientSystemMap.at(id)) {
		for(auto it : *clients) {
			const std::pair<ID, System::Identifier> data {id, systemIdentifier};

			using Type = Packet<std::pair<ID, System::Identifier>>;
			auto packet = Type {
				stringhash("swordbow-magic"),
				MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM,
				data,
				sizeof(data)
			};
			socket->send<Type>(it.first, packet);
		}
	}
	entityClientSystemMap.erase(id);

	//3. Remove from componentManager
	componentManager->clearComponents(id);

	//4. ReleaseId to idmanager
	idManager->releaseId(id);
}

void EntityManager::registerIdToSystem(ID id, System::Identifier system) {
	auto systemptr = systemManager->getSystem(system);
	entityServerSystemMap[id].push_back(systemptr);
	systemptr->add(id);
}

void EntityManager::registerIdToRemoteSystem(ID id, System::Identifier system) {
	entityClientSystemMap[id].push_back(system);
	for(auto it : *clients) {
		const std::pair<ID, System::Identifier> data {id, system};

		using Type = Packet<std::pair<ID, System::Identifier>>;
		auto packet = Packet<std::pair<ID, System::Identifier>> {
			stringhash("swordbow-magic"),
			MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM,
			data,
			sizeof(data)
		};
		socket->send<Type>(it.first, packet);
	}
}













//
