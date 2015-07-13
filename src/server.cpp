#include "server.hpp"
#include "client.hpp"
#include "packet.hpp"
#include "messagetypes.hpp"

Server::Server(int argc, char** argv) :
		systemManager(&componentManager, &deltaTime),
		sizeBoundingBox(&componentManager),
		entityManager(&systemManager, &componentManager, &idManager, &clients, &socket),
		sizeHashGridSystem(&componentManager, &sizeBoundingBox),
		collisionSystem(&sizeHashGridSystem),
		removeSystem(&entityManager),
		animationSystem(&clients, &socket),
		attackSystem(&sizeHashGridSystem, &clients, &socket) {

	cout << "\n--** STARTING SERVER **--" << endl;

	printGeneralInfo();

	constexpr short port = 47293;
	socket.open(port);

	//Add systems to systemmanager
	systemManager.add(&inputSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&moveDiffSystem);
	systemManager.add(&renderDiffSystem);
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

Server::~Server() {
	socket.close();
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

	//Receive data from server...
    IpAddress client;
    unsigned char type;
    int bytesRead;
    socket.receive(client, type, bytesRead);

	//If any data was received, check its type and take appropiate action
    if(bytesRead > 0) {
        auto packet = (Packet<unsigned char*>*)socket.getBuffer();

        switch(packet->getType()) {
			case MESSAGE_TYPE::CONNECT: {
				onConnect(client.getAddress(), client.getPort());
			} break;

			case MESSAGE_TYPE::DISCONNECT: {
				onDisconnect(client.getAddress());
			} break;

			case MESSAGE_TYPE::INPUTDATA: {
				const auto typedPacket = (Packet<InputData>*)socket.getBuffer();
				inputDataToInputComponent(client.getAddress(), typedPacket->getData());
			}
        }
	}

	//Run the entity-component-system
	componentsMutex.lock();
	systemManager.update();
	componentsMutex.unlock();

	//Broadcast new gamestate to clients
	sendDiff();

	deltaTime.stop();
}

void Server::onConnect(unsigned int client, unsigned short port) {
	auto fatManId = entityManager.createFatMan({0.0f, 0.0f});
	clients.insert({client, fatManId});

	//Client should get a copy of all components
	send(client, port);

	//Make the client aware of its ID and register the ID to client camerasytem
	const std::pair<ID, System::Identifier> data {fatManId, System::CAMERA};
	auto cameraPacket = Packet<std::pair<ID, System::Identifier>> {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::CONNECT,
		data,
		sizeof(data)
	};

	socket.send({client, port}, &cameraPacket, sizeof(cameraPacket));

	//Whenever a client connects, tell the client what entities
	//should be in what systems on the client-side
	for(auto tuple : entityManager.entityClientSystemMap) {
		auto id = tuple.first;
		vector<System::Identifier>& systems = tuple.second;
		for(auto system : systems) {
			const std::pair<ID, System::Identifier> data {id, system};
			auto packet = Packet<std::pair<ID, System::Identifier>> {
				stringhash("swordbow-magic"),
				MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM,
				data,
				sizeof(data)
			};
			socket.send({client, port}, &packet, sizeof(packet));
		}
	}
}

void Server::onDisconnect(unsigned int client) {
	entityManager.remove(clients.at(client));
	clients.erase(client);
}

void Server::send(unsigned int client, unsigned short port) {
	auto mcpacket = Packet<Components<MoveComponent>> {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::MOVECOMPONENTS,
		componentManager.moveComponents,
		sizeof(componentManager.moveComponents)
	};
	socket.send({client, port}, &mcpacket, sizeof(mcpacket));

	auto rcpacket = Packet<Components<RenderComponent>> {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::RENDERCOMPONENTS,
		componentManager.renderComponents,
		sizeof(componentManager.renderComponents)
	};
	socket.send({client, port}, &rcpacket, sizeof(rcpacket));
}

void Server::send() {
	//TODO: if clients were to hold IpAddress instead of unsigned int (address)
	//clients could have other ports than 47294
	constexpr unsigned short clientPort = 47294;
	for(auto it : clients) {
		send(it.first, clientPort);
	}
}

void Server::sendDiff(unsigned int client, unsigned short port) {
	auto mcpacket = Packet<Components<MoveComponent>> {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::MOVECOMPONENTSDIFF,
		componentManager.moveComponentsDiff,
		sizeof(componentManager.moveComponentsDiff)
	};
	socket.send({client, port}, &mcpacket, sizeof(mcpacket));

	auto rcpacket = Packet<Components<RenderComponent>> {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::RENDERCOMPONENTSDIFF,
		componentManager.renderComponentsDiff,
		sizeof(componentManager.renderComponentsDiff)
	};
	socket.send({client, port}, &rcpacket, sizeof(rcpacket));
}

void Server::sendDiff() {
	//TODO: if clients were to hold IpAddress instead of unsigned int (address)
	//clients could have other ports than 47294
	constexpr unsigned short clientPort = 47294;
	for(auto it : clients) {
		sendDiff(it.first, clientPort);
	}
}

void Server::inputDataToInputComponent(unsigned int client, InputData& data) {
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

	inputSystem.activateId(id);
}

int main(int argc, char** argv) {
	Server server(argc, argv);
	server.run();
	return 0;
}










//
