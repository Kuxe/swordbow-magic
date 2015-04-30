#include <iostream>
#include "deltatime.h"
#include "eventmanager.h"
#include "componentmanager.h"
#include "systemmanager.h"
#include "idmanager.h"
#include "entitymanager.h"
#include "movesystem.h"
#include "rendersystem.h"
#include "movecomponent.h"
#include "rendercomponent.h"
#include "world.h"
#include "hashgridsystem.h"
#include "collisionsystem.h"
#include "textureboundingbox.h"
#include "sizeboundingbox.h"

using namespace std;

int main(int argc, char** argv) {
	bool running = true;

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

	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);
	systemManager.add(&collisionSystem);
	systemManager.add(&textureHashGridSystem);
	systemManager.add(&sizeHashGridSystem);

	auto playerId = entityManager.createPlayer();
	World world(&entityManager);

	for(int y = 1; y <= 250; y++) {
		for(int x = 1; x <= 250; x++) {
			auto tree = entityManager.createTree();
			auto treeMc = componentManager.moveComponents.at(tree);
			treeMc->xpos = 20 + x * 40;
			treeMc->ypos = 20 + y * 40;
			treeMc->oldXpos = 20 + x * 40;
			treeMc->oldYpos = 20 + y * 40;

		}
	}

	//Keystroke events should change the input component of the player
	eventManager.userInputComponent = componentManager.inputComponents.at(playerId);

	//Set camera to follow player
	renderSystem.setCameraTarget(playerId);

	while(running) {
		deltaTime.start();

		systemManager.update();
		eventManager.process();

		deltaTime.stop();
	}

	return 0;
}
