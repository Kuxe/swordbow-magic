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

using namespace std;

int main(int argc, char** argv) {
	bool running = true;

	DeltaTime deltaTime;
	EventManager eventManager(&running);

	ComponentManager componentManager;
	SystemManager systemManager(&componentManager, &deltaTime);
	IdManager idManager;

	EntityManager entityManager(&systemManager, &componentManager, &idManager);

	MoveSystem moveSystem;
	RenderSystem renderSystem;

	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);

	World world(&entityManager);
	auto playerId = entityManager.createPlayer();

	//Keystroke events should change the input component of the player
	eventManager.userInputComponent = componentManager.inputComponents.at(playerId);

	while(running) {
		deltaTime.start();

		systemManager.update();
		eventManager.process();

		deltaTime.stop();
	}

	return 0;
}