#include "server.hpp"
#include "client.hpp"

Server::Server(int argc, char** argv) :
		systemManager(&componentManager, &deltaTime),
		sizeBoundingBox(&componentManager),
		entityManager(&systemManager, &componentManager, &idManager, &clients),
		sizeHashGridSystem(&componentManager, &sizeBoundingBox),
		collisionSystem(&sizeHashGridSystem),
		removeSystem(&entityManager),
		animationSystem(&clients),
		attackSystem(&sizeHashGridSystem, &clients) {

	cout << "\n--** STARTING SERVER **--" << endl;

	printGeneralInfo();

	//Add systems to systemmanager
	systemManager.add(&inputSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&animationSystem);
	systemManager.add(&sizeHashGridSystem);
	systemManager.add(&collisionSystem);
	systemManager.add(&attackSystem);
	systemManager.add(&healthSystem);
	systemManager.add(&removeSystem);

	//Populate world with... world
	World world(&entityManager);


	bool bigworld = false;
	for(int i = 0; i < argc; i++) {
		if(strcmp(argv[i], "bigworld") == 0) {
			bigworld = true;
			break;
		}
	}
	if(bigworld) {
		world.createWorld();
	} else {
		world.createDebugWorld();
	}
}

void Server::run() {
    while(running) {
		step();
	}
}

void Server::terminate() {
	running = false;
}

void Server::step() {
	deltaTime.start();

	//Run the entity-component-system
	componentsMutex.lock();
	systemManager.update();
	componentsMutex.unlock();

	//Broadcast new gamestate to clients
	send();

	deltaTime.stop();
}

void Server::onConnect(Client* client) {
	auto fatManId = entityManager.createFatMan({0.0f, 0.0f});
	clients.insert(
		{
			client,
			fatManId
		}
	);

	client->registerIdToSystem(fatManId, "CameraSystem");

	//Whenever a client connects, tell the client what entities
	//should be in what systems on the client-side
	for(auto tuple : entityManager.entityClientSystemMap) {
		auto id = tuple.first;
		vector<std::string>& systems = tuple.second;
		for(std::string systemIdentifier : systems) {
			client->registerIdToSystem(id, systemIdentifier);
		}
	}

	//Client should get a copy of components
	send(client);
}

void Server::onDisconnect(Client* client) {
	entityManager.remove(clients.at(client));
	clients.erase(client);
}

void Server::send(Client* client) {
	auto id = clients.at(client);
	const auto& mc = componentManager.moveComponents.at(id);
	client->recv(
		componentManager.moveComponents,
		componentManager.renderComponents,
		mc.pos
	);
}

void Server::send() {
	for(auto it : clients) {
		send(it.first);
	}
}

void Server::recv(Client* client, InputData inputData) {
	componentsMutex.lock();

	//Alter the inputcomponent of id belonging to client
	inputDataToInputComponent(client, inputData);

	componentsMutex.unlock();
}

void Server::inputDataToInputComponent(Client* client, InputData& data) {
	//Get id of client
	auto id = clients.at(client);
	auto& ic = componentManager.inputComponents.at(id);

	ic.presses = data.presses;

	while(!data.presses.empty()) {
		auto keypress = data.presses.front(); data.presses.pop();

		//Figure out what key was pressed
		switch(keypress) {
			case SDLK_w: {
				ic.w = true;
				break;
			}
			case SDLK_a: {
				ic.a = true;
				break;
			}
			case SDLK_s: {
				ic.s = true;
			break;
			}
			case SDLK_d: {
				ic.d = true;
				break;
			}
			case SDLK_SPACE: {
				ic.space = true;
				break;
			}
		}
	}

	ic.releases = data.releases;

	while(!data.releases.empty()) {
		auto keyrelease = data.releases.front(); data.releases.pop();

		//Figure out what key was released
		switch(keyrelease) {
			case SDLK_w: {
				ic.w = false;
				break;
			}
			case SDLK_a: {
				ic.a = false;
				break;
			}
			case SDLK_s: {
				ic.s = false;
				break;
			}
			case SDLK_d: {
				ic.d = false;
				break;
			}
			case SDLK_SPACE: {
				ic.space = false;
				break;
			}
		}
	}


	//Since this method recv() was called in the first place, the entity
	//of this client should be activated in the inputsystem
	inputSystem.activateId(id);
}












//
