#include <iostream>
#include <SDL2/SDL.h>
#include "animationsystem.hpp"
#include "deltatime.hpp"
#include "eventmanager.hpp"
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

using namespace std;

int main(int argc, char** argv) {
	bool running = true;

	SDL_Init(0);

	DeltaTime deltaTime;
	EventManager eventManager(&running);

	ComponentManager componentManager;
	SystemManager systemManager(&componentManager, &deltaTime);

	TextureBoundingBox textureBoundingBox(&componentManager);
	SizeBoundingBox sizeBoundingBox(&componentManager);
	HashGridSystem textureHashGridSystem(&componentManager, &textureBoundingBox);
	HashGridSystem sizeHashGridSystem(&componentManager, &sizeBoundingBox);
	IdManager idManager;

	EntityManager entityManager(&systemManager, &componentManager, &idManager);

	MoveSystem moveSystem;
	RenderSystem renderSystem;
	CollisionSystem collisionSystem(&sizeHashGridSystem);
	SoundSystem soundSystem;
	AnimationSystem animationSystem;
	HealthSystem healthSystem;
	RemoveSystem removeSystem(&entityManager);

	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);
	systemManager.add(&collisionSystem);
	systemManager.add(&textureHashGridSystem);
	systemManager.add(&sizeHashGridSystem);
	systemManager.add(&soundSystem);
	systemManager.add(&animationSystem);
	systemManager.add(&healthSystem);
	systemManager.add(&removeSystem);

	auto playerId = entityManager.createFatMan();
	auto botId = entityManager.createFatMan({60, 20});

	World world(&entityManager);

	for(int y = 1; y <= 3; y++) {
		for(int x = 1; x <= 3; x++) {
			auto tree = entityManager.createTree();
			auto& treeMc = componentManager.moveComponents.at(tree);
			treeMc.xpos = 20 + x * 70;
			treeMc.ypos = 20 + y * 70;
			treeMc.oldXpos = 20 + x * 70;
			treeMc.oldYpos = 20 + y * 70;

		}
	}

	//Keystroke events should change the input component of the player
	eventManager.setPlayer(playerId, &componentManager);

	//Set camera to follow player
	renderSystem.setCameraTarget(playerId);

	//Play some sweet music
	soundSystem.playMusic("./resources/sounds/naturesounds.ogg");

	while(running) {
		deltaTime.start();

		systemManager.update();
		eventManager.process();

		deltaTime.stop();
	}

	SDL_Quit();

	return 0;
}
