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
	std::unordered_map<unsigned int, ID>* clients,
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
	renderComponent.imagePath = "./resources/images/playerv3_front.png";
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
		new PlaySound(
			SoundComponent::Sound {"./resources/sounds/bloodsplatter.wav"},
			clients,
			socket
		),
		new CreateBloodsplatter(this, id),
	};

	commandComponent[CommandComponent::Event::ON_MOVE] = {
		//Perhaps use a "new ActivateIdOnClients(id, "RenderSystem")"?
		new ActivateIdOnClients(id, "RenderSystem", socket, clients),
		new ActivateId(id, "CollisionSystem", systemManager),
		new ActivateIdOnClients(id, "TextureHashGridSystem", socket, clients),
		new ActivateId(id, "SizeHashGridSystem", systemManager),
		new PlaySound(soundComponent.walk, clients, socket),
	};

	registerIdToSystem(id, "MoveSystem");
	registerIdToSystem(id, "CollisionSystem");
	registerIdToSystem(id, "SizeHashGridSystem");
	registerIdToSystem(id, "HealthSystem");
	registerIdToSystem(id, "RemoveSystem");
	registerIdToSystem(id, "AttackSystem");
	registerIdToSystem(id, "InputSystem");
	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");
	registerIdToSystem(id, "AnimationSystem");

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

	renderComponent.imagePath = "./resources/images/SmallTree.png";
	renderComponent.zindex_base = 1;
	renderComponent.xoffset = -24;
	renderComponent.yoffset = -54;

	moveComponent.pos = position;
	moveComponent.oldPos = moveComponent.pos;

	sizeComponent.width = 36;
	sizeComponent.height = 10;

	nameComponent.name = "tree";

	registerIdToSystem(id, "CollisionSystem");
	registerIdToSystem(id, "SizeHashGridSystem");
	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");

	return id;
}
ID EntityManager::createGrassTile(const glm::vec2& position) {
	auto id = idManager->acquireId();

	auto& mc = componentManager->createMoveComponent(id);
	auto& tc = componentManager->createTileComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	rc.imagePath = "./resources/images/grass.png";
	rc.zindex_base = 0;

	mc.pos = position;
	mc.oldPos = mc.pos;

	sizeComponent.width = 32;
	sizeComponent.height = 32;

	nameComponent.name = "grasstile";

	//Systems on clients need to be aware of these
	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");

	return id;
}

ID EntityManager::createWaterTile(const glm::vec2& position) {
	auto id = idManager->acquireId();

	auto& mc = componentManager->createMoveComponent(id);
	componentManager->createTileComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	rc.imagePath = "./resources/images/water.png";
	rc.zindex_base = 0;

	mc.pos = position;
	mc.oldPos = mc.pos;

	sizeComponent.width = 32;
	sizeComponent.height = 32;

	nameComponent.name = "watertile";

	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");

	return id;
}

ID EntityManager::createBloodSplatter(const glm::vec2& position) {
	auto id = idManager->acquireId();
	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& ac = componentManager->createAnimationComponent(id);

	rc.imagePath = "./resources/images/bloodsplatter1_11.png";
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
	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");
	registerIdToSystem(id, "AnimationSystem");

	return id;
}

ID EntityManager::createFlower(const glm::vec2& position, const char color) {
	auto id = idManager->acquireId();
	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nc = componentManager->createNameComponent(id);

	string flowercolor = "";

	//Set image corresponding to color
	switch(color) {
		case 0: {
			flowercolor = "./resources/images/yellow_flower.png";
		} break;
		case 1: {
			flowercolor = "./resources/images/blue_flower.png";
		} break;
		case 2: {
			flowercolor = "./resources/images/violet_flower.png";
		} break;
		case 3: {
			flowercolor = "./resources/images/green_flower.png";
		} break;
	}

	rc.imagePath = flowercolor;
	rc.xoffset = -1;
	rc.yoffset = -5;
	rc.zindex_base = 1;

	mc.pos = position;

	nc.name = "flower";

	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");

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

	rc.imagePath = "./resources/images/testsquare1x1.png";

	mc.maxVelLength = 4.0f;

	rc.xoffset = 0;
	rc.yoffset = 0;
	rc.zindex_base = 1;

	mc.pos = position;

	nc.name = "testsquare1x1";

	cc[CommandComponent::Event::MOVE_UP] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	cc[CommandComponent::Event::MOVE_RIGHT] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	cc[CommandComponent::Event::MOVE_DOWN] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	cc[CommandComponent::Event::MOVE_LEFT] = {
		new AddIdToSystem(id, "MoveSystem", systemManager),
	};

	cc[CommandComponent::Event::ON_MOVE] = {
		new ActivateIdOnClients(id, "RenderSystem", socket, clients),
		new ActivateIdOnClients(id, "TextureHashGridSystem", socket, clients),
	};

	registerIdToSystem(id, "MoveSystem");
	registerIdToSystem(id, "InputSystem");
	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");

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

	rc.imagePath = "./resources/images/stone1.png";
	rc.xoffset = -7;
	rc.yoffset = -12;
	rc.zindex_base = 1;

	sc.width = 28;
	sc.height = 15;

	nc.name = "stone";

	registerIdToSystem(id, "CollisionSystem");
	registerIdToSystem(id, "SizeHashGridSystem");
	registerIdToRemoteSystem(id, "RenderSystem");
	registerIdToRemoteSystem(id, "TextureHashGridSystem");

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
			constexpr unsigned short port = 47294;
			const std::pair<ID, std::string> data {id, systemIdentifier};
			auto packet = Packet<std::pair<ID, std::string>> {
				stringhash("swordbow-magic"),
				MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM,
				data,
				sizeof(data)
			};
			socket->send({it.first, port}, &packet, sizeof(packet));
		}
	}
	entityClientSystemMap.erase(id);

	//3. Remove from componentManager
	componentManager->clearComponents(id);

	//4. ReleaseId to idmanager
	idManager->releaseId(id);
}

void EntityManager::registerIdToSystem(ID id, const string systemIdentifier) {
	auto system = systemManager->getSystem(systemIdentifier);
	entityServerSystemMap[id].push_back(system);
	system->add(id);
}

void EntityManager::registerIdToRemoteSystem(ID id, const string systemIdentifier) {
	entityClientSystemMap[id].push_back(systemIdentifier);
	for(auto it : *clients) {
		constexpr unsigned short port = 47294;
		const std::pair<ID, std::string> data {id, systemIdentifier};
		auto packet = Packet<std::pair<ID, std::string>> {
			stringhash("swordbow-magic"),
			MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM,
			data,
			sizeof(data)
		};
		socket->send({it.first, port}, &packet, sizeof(packet));
	}
}













//
