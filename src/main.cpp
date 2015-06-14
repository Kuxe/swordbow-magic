#include <iostream>
#include <SDL2/SDL.h>
#include "animationsystem.hpp"
#include "deltatime.hpp"
#include "keyboardsystem.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "idmanager.hpp"
#include "entitymanager.hpp"
#include "movesystem.hpp"
#include "rendersystem.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "world.hpp"
#include "hashgridsystem.hpp"
#include "collisionsystem.hpp"
#include "textureboundingbox.hpp"
#include "sizeboundingbox.hpp"
#include "soundsystem.hpp"
#include "healthsystem.hpp"
#include "removesystem.hpp"
#include "attacksystem.hpp"
#include "inputsystem.hpp"
#include "camerasystem.hpp"
#include <assert.h>
#include <glm/ext.hpp>

using namespace std;


constexpr void printGeneralInfo() {

	//Print some worst-case memory statistics, assuming
	//all entities have all components
	//(which isn't the case)
	uint64_t bytePerMegabyte = 1048576;
	uint64_t entityByteSize = ComponentManager::sizePerEntity();
	uint64_t allEntitiesByteSize = IdManager::MAX_IDS * entityByteSize;
	uint64_t allEntitiesMegabyteSize = allEntitiesByteSize / bytePerMegabyte;
	cout << "There can be at most " << IdManager::MAX_IDS << " ids";
	cout << " each of size " << entityByteSize << "bytes summing";
	cout << " up to " << allEntitiesByteSize;
	cout << "bytes (" << allEntitiesMegabyteSize;
	cout << "MB)" << endl;
}

int main(int argc, char** argv) {
	printGeneralInfo();

	bool running = true;

	DeltaTime deltaTime;

	ComponentManager componentManager;
	SystemManager systemManager(&componentManager, &deltaTime);

	TextureBoundingBox textureBoundingBox(&componentManager);
	SizeBoundingBox sizeBoundingBox(&componentManager);
	IdManager idManager;

	EntityManager entityManager(&systemManager, &componentManager, &idManager);

	//Declare systems
	HashGridSystem textureHashGridSystem(&componentManager, &textureBoundingBox);
	HashGridSystem sizeHashGridSystem(&componentManager, &sizeBoundingBox);
	MoveSystem moveSystem;
	RenderSystem renderSystem;
	CollisionSystem collisionSystem(&sizeHashGridSystem);
	SoundSystem soundSystem;
	AnimationSystem animationSystem;
	HealthSystem healthSystem;
	RemoveSystem removeSystem(&entityManager);
	AttackSystem attackSystem(&sizeHashGridSystem);
	InputSystem inputSystem;
	CameraSystem cameraSystem(&renderSystem);
	KeyboardSystem keyboardSystem(&running);

	//Couple systems where neccesary
	renderSystem.setCameraSystem(&cameraSystem);

	//Add systems to systemmanager
	systemManager.add(&keyboardSystem);
	systemManager.add(&inputSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&textureHashGridSystem);
	systemManager.add(&sizeHashGridSystem);
	systemManager.add(&renderSystem);
	systemManager.add(&cameraSystem);
	systemManager.add(&collisionSystem);
	systemManager.add(&soundSystem);
	systemManager.add(&animationSystem);
	systemManager.add(&attackSystem);
	systemManager.add(&healthSystem);
	systemManager.add(&removeSystem);

	//Create ids
	auto playerId = entityManager.createFatMan({0.0f, 0});
	//auto botId = entityManager.createFatMan({50, 50});

	//Populate world with... world
	World world(&entityManager);
	world.createDebugWorld();

	//Keystroke events should change the input component of the player
	entityManager.registerIdToSystem("KeyboardSystem", playerId);

	//Camera should revolve around the player
	entityManager.registerIdToSystem("CameraSystem", playerId);

	//Play some sweet music
	soundSystem.playMusic("./resources/sounds/naturesounds.ogg");

	while(running) {
		deltaTime.start();
		systemManager.update();
		deltaTime.stop();
	}

	return 0;
}
