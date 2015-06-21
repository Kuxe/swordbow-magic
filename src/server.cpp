#include "server.hpp"

Server::Server(int argc, char** argv) :
		systemManager(&componentManager, &deltaTime),
		textureBoundingBox(&componentManager, &renderSystem),
		sizeBoundingBox(&componentManager),
		entityManager(&systemManager, &componentManager, &idManager),
		textureHashGridSystem(&componentManager, &textureBoundingBox),
		sizeHashGridSystem(&componentManager, &sizeBoundingBox),
		collisionSystem(&sizeHashGridSystem),
		removeSystem(&entityManager),
		attackSystem(&sizeHashGridSystem),
		cameraSystem(&renderSystem),
		keyboardSystem(&running) {

	printGeneralInfo();

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
	systemManager.add(&serverNetworkSystem);

	//Create ids
	auto playerId = entityManager.createFatMan({0.0f, 0});
	//auto botId = entityManager.createFatMan({50, 50});

	//Populate world with... world
	World world(&entityManager);

	if(argc > 1 && strcmp(argv[1], "debug") == 0) {
		world.createDebugWorld();
	} else {
		world.createWorld();
	}

	//Keystroke events should change the input component of the player
	entityManager.registerIdToSystem("KeyboardSystem", playerId);

	//Camera should revolve around the player
	entityManager.registerIdToSystem("CameraSystem", playerId);

	//Play some sweet music
	soundSystem.playMusic("./resources/sounds/naturesounds.ogg");
}

void Server::run() {
    while(running) {
		deltaTime.start();
		systemManager.update();
		deltaTime.stop();
	}
}
