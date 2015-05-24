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

using namespace std;

int main(int argc, char** argv) {
	bool running = true;

	SDL_Init(0);

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
	auto playerId = entityManager.createFatMan({20, 20});
	auto botId = entityManager.createFatMan({50, 50});

	//Populate world with... world
	World world(&entityManager);

	for(int y = 1; y <= 3; y++) {
		for(int x = 1; x <= 3; x++) {
			auto tree = entityManager.createTree();
			auto& treeMc = componentManager.moveComponents.at(tree);
			treeMc.pos.x = 20 + x * 70;
			treeMc.pos.y = 20 + y * 70;
			treeMc.oldPos.x = 20 + x * 70;
			treeMc.oldPos.y = 20 + y * 70;

		}
	}

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

	SDL_Quit();

	return 0;
}
