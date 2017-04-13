#include "entitymanager.hpp"
#include "componentmanager.hpp"
#include "idmanager.hpp"

#include <glm/gtc/matrix_transform.hpp> //glm::translate
 
/** For networking **/
#include "packetmanager.hpp"
#include "packet.hpp"
#include "messagetypes.hpp"

#include "systemmanager.hpp"
#include "activateid.hpp"
#include "addidtosystem.hpp"
#include "removeidfromsystem.hpp"
#include "playsound.hpp"
#include "createbloodsplatter.hpp"

/** For logging **/
#include "logger.hpp"

EntityManager::EntityManager(
	SystemManager* systemManager,
	ComponentManager* componentManager,
	IdManager* idManager,
	std::unordered_map<IpAddress, ClientData>* clients,
	PacketManager* packetManager) :
	systemManager(systemManager),
	componentManager(componentManager),
	idManager(idManager),
	clients(clients),
	packetManager(packetManager) {

}

const unsigned int& EntityManager::getId() {
	auto id = idManager->acquireId();
	ids.insert(id);
	return *ids.find(id);
}

ID EntityManager::createFatMan(const glm::mat4& transform) {
	//Get unique ID
	auto id = getId();

	Logger::log("Creating fatman with id" + std::to_string(id), Logger::INFO);

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
	componentManager->createCameraComponent(id);

	//If you'd like to change default initialization-data in a component
	//Just save a pointer to the component like above and modify it like bellow

	moveComponent.transform = transform;
	moveComponent.velocity = {200, 200, 0};

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
	inputComponent.bindings[69] = CommandComponent::Event::FLY_UP;
	inputComponent.bindings[81] = CommandComponent::Event::FLY_DOWN;
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

	commandComponent[CommandComponent::Event::FLY_UP] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	commandComponent[CommandComponent::Event::FLY_DOWN] = {
		new AddIdToSystem(id, System::MOVE, systemManager),
	};

	commandComponent[CommandComponent::Event::ATTACK] = {
		new ActivateId(id, System::ATTACK, systemManager),
	};

	/* FIXME: The PlaySoundA compilation error
	commandComponent[CommandComponent::Event::ON_DEATH] = {
		new PlaySound(
			SoundData {Sound::BLOODSPLATTER},
			clients,
			packetManager
		),
		new CreateBloodsplatter(this, id),
	};
	*/

	commandComponent[CommandComponent::Event::ON_MOVE] = {
		new ActivateId(id, System::COLLISION, systemManager),
		new ActivateId(id, System::HASHGRID_SIZE, systemManager),
		//new PlaySound(soundComponent.walk, clients, packetManager), //FIXME: The PlaySoundA compilation error
		new AddIdToSystem(id, System::MOVEDIFF, systemManager)
	};

	registerIdToSystem(id, System::MOVE);
	registerIdToSystem(id, System::COLLISION);
	registerIdToSystem(id, System::HASHGRID_SIZE);
	registerIdToSystem(id, System::HEALTH);
	registerIdToSystem(id, System::REMOVE);
	registerIdToSystem(id, System::ATTACK);
	registerIdToSystem(id, System::INPUT);
	registerIdToSystem(id, System::ANIMATION);
	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createFatMan() {
	return createFatMan(glm::mat4());
}

ID EntityManager::createFatMan(const glm::vec2& pos) {
	return createFatMan(glm::translate(glm::mat4(), glm::vec3(pos.x, 0, pos.y)));
}

ID EntityManager::createTree(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating tree with id" + std::to_string(id), Logger::INFO);

	auto& moveComponent = componentManager->createMoveComponent(id);
	componentManager->createInputComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& renderComponent = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	moveComponent.transform = transform;

	sizeComponent.width = 36;
	sizeComponent.height = 10;

	nameComponent.name = "tree";

	registerIdToSystem(id, System::COLLISION);
	registerIdToSystem(id, System::HASHGRID_SIZE);
	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createTree(const glm::vec2& pos) {
	return createTree(glm::translate(glm::mat4(), glm::vec3(pos.x, 0, pos.y)));
}

ID EntityManager::createGrassTile(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating grass with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	auto& tc = componentManager->createTileComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	mc.transform = transform;

	sizeComponent.width = 32;
	sizeComponent.height = 32;

	nameComponent.name = "grasstile";

	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createWaterTile(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating water with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	componentManager->createTileComponent(id);
	auto& sizeComponent = componentManager->createSizeComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nameComponent = componentManager->createNameComponent(id);

	mc.transform = transform;

	sizeComponent.width = 32;
	sizeComponent.height = 32;

	nameComponent.name = "watertile";

	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createBloodSplatter(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating bloodsplatter with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& ac = componentManager->createAnimationComponent(id);

	mc.transform = transform;

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

	registerIdToSystem(id, System::ANIMATION);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createFlower(const glm::mat4& transform, const char color) {
	auto id = getId();

	Logger::log("Creating flower with id" + std::to_string(id), Logger::INFO);

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

	mc.transform = transform;

	nc.name = "flower";

	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createFlower(const glm::vec2& pos, const char color) {
	return createFlower(glm::translate(glm::mat4(), glm::vec3(pos.x, 0, pos.y)), color);
}

ID EntityManager::createDummySquare(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating dummysquare with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& ic = componentManager->createInputComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& cc = componentManager->createCommandComponent(id);
	auto& ac = componentManager->createAnimationComponent(id);

	mc.maxVelLength = 4.0f;

	mc.transform = transform;

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
		new AddIdToSystem(id, System::MOVE, systemManager)
	};

	registerIdToSystem(id, System::MOVE);
	registerIdToSystem(id, System::INPUT);
	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createStone(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating stone with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& sc = componentManager->createSizeComponent(id);
	auto& nc = componentManager->createNameComponent(id);


	mc.transform = transform;

	sc.width = 28;
	sc.height = 15;

	nc.name = "stone";

	registerIdToSystem(id, System::COLLISION);
	registerIdToSystem(id, System::HASHGRID_SIZE);
	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createBlueBird(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating blueBird with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& ac = componentManager->createAnimationComponent(id);
	auto& accelerationComponent = componentManager->createAccelerationComponent(id);
	auto& bc = componentManager->createBirdComponent(id);
	auto& cc = componentManager->createCommandComponent(id);

	mc.transform = transform;
	mc.velocity = {0, 0, 0};
	mc.maxVelLength = 150.0f;

	nc.name = "blueBird";

	ac.walking.west.frames.push_back(Image::BLUE_BIRD_WEST_1);
	ac.walking.west.frames.push_back(Image::BLUE_BIRD_WEST_2);
	ac.walking.west.frames.push_back(Image::BLUE_BIRD_WEST_3);
	ac.walking.west.frames.push_back(Image::BLUE_BIRD_WEST_4);
	ac.walking.west.frames.push_back(Image::BLUE_BIRD_WEST_3);
	ac.walking.west.frames.push_back(Image::BLUE_BIRD_WEST_2);

	ac.walking.east.frames.push_back(Image::BLUE_BIRD_EAST_1);
	ac.walking.east.frames.push_back(Image::BLUE_BIRD_EAST_2);
	ac.walking.east.frames.push_back(Image::BLUE_BIRD_EAST_3);
	ac.walking.east.frames.push_back(Image::BLUE_BIRD_EAST_4);
	ac.walking.east.frames.push_back(Image::BLUE_BIRD_EAST_3);
	ac.walking.east.frames.push_back(Image::BLUE_BIRD_EAST_2);

	ac.state = &ac.walking.west;
	ac.walking.west.duration = 200;

	cc[CommandComponent::Event::ON_MOVE] = {
		new AddIdToSystem(id, System::MOVEDIFF, systemManager)
	};

	registerIdToSystem(id, System::MOVE);
	registerIdToSystem(id, System::ANIMATION);
	registerIdToSystem(id, System::BIRD);
	registerIdToSystem(id, System::ACCELERATION);
	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	registerIdToSystem(id, System::MOVEDIFF);
	registerIdToSystem(id, System::RENDERDIFF);

	return id;
}

ID EntityManager::createBlueBird(const glm::vec2& pos) {
	return createBlueBird(glm::translate(glm::mat4(), glm::vec3(pos.x, 0, pos.y)));
}

ID EntityManager::createSwarmPoint(const glm::mat4& transform) {
	auto id = getId();

	auto& mc = componentManager->createMoveComponent(id);
	auto& ac = componentManager->createAccelerationComponent(id);
	auto& nc = componentManager->createNameComponent(id);

	mc.transform = transform;

	nc.name = "SwarmPoint";

	registerIdToSystem(id, System::ACCELERATION);
	registerIdToSystem(id, System::MOVE);

	return id;
}

ID EntityManager::createSkybox(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating skybox with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	mc.transform = transform;
	nc.name = "Skybox";

	rc.model = "sphere";
	rc.shader = "skybox";

	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	return id;
}

ID EntityManager::createSkybox(const glm::vec3& pos) {
	return createSkybox(glm::translate(glm::mat4(), pos));
}

ID EntityManager::createTerrain(const glm::mat4& transform) {
	auto id = getId();

	Logger::log("Creating terrain with id" + std::to_string(id), Logger::INFO);

	auto& mc = componentManager->createMoveComponent(id);
	auto& nc = componentManager->createNameComponent(id);
	auto& rc = componentManager->createRenderComponent(id);
	mc.transform = transform;
	nc.name = "Terrain";

	rc.model = "terrain";
	rc.shader = "default";

	registerIdToSystem(id, System::INITIAL_COMPONENTS);
	return id;
}

void EntityManager::remove(ID id) {
	Logger::log("Removing id" + std::to_string(id) + " from entitymanager", Logger::INFO);

	//1. Remove from server systemManager
	for(auto system : entityServerSystemMap.at(id)) {
		system->remove(id);
	}
	entityServerSystemMap.erase(id);

	//2. Remove from clients
	//Tell clients to erase id from its systems and component manager.
	//By assumption any id on client systems are present on both rendersystem
	//and texturehashgridsystem
	//So a generic "REMOVE_ID" message is sufficient. This might
	//change in the future.
	for(auto& pair : *clients) {
		auto& clientData = pair.second;

		auto packet = Packet<ID, MESSAGE_TYPE::REMOVE_ID> {
			stringhash("swordbow-magic"),
			clientData.sequence++,
			id,
			sizeof(id)
		};
		packetManager->send<ID>(pair.first, packet);
	}

	//3. Remove from componentManager
	componentManager->clearComponents(id);

	//4. Tell entitymanager that id isn't part of ECS anymore
	ids.erase(id);

	//5. ReleaseId to idmanager
	idManager->releaseId(id);
}

bool EntityManager::idExists(ID id) {
	return ids.find(id) != ids.end();
}

void EntityManager::registerIdToSystem(ID id, System::Identifier system) {
	Logger::log("Registering id " + std::to_string(id) + " to system " + std::to_string(system), Logger::VERBOSE);
	
	auto systemptr = systemManager->getSystem(system);
	entityServerSystemMap[id].push_back(systemptr);
	systemptr->add(id);
}
