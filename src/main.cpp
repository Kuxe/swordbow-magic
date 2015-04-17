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
#include "flagsystem.h"
#include "collisionsystem.h"

using namespace std;

int main(int argc, char** argv) {
	bool running = true;

	DeltaTime deltaTime;
	EventManager eventManager(&running);

	ComponentManager componentManager;
	SystemManager systemManager(&componentManager, &deltaTime);
	HashGridSystem hashGridSystem(&componentManager);
	IdManager idManager;

	EntityManager entityManager(&systemManager, &componentManager, &idManager);

	MoveSystem moveSystem;
	RenderSystem renderSystem;
	FlagSystem flagSystem;
	CollisionSystem collisionSystem;

	systemManager.add(&flagSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);
	systemManager.add(&collisionSystem);
	systemManager.add(&hashGridSystem);

	auto playerId = entityManager.createPlayer();
	World world(&entityManager);

	for(int y = 1; y <= 50; y++) {
		for(int x = 1; x <= 50; x++) {
			auto tree = entityManager.createTree();
			auto treeMc = componentManager.moveComponents.at(tree);
			treeMc->xpos = x * 100;
			treeMc->ypos = y * 100;

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
