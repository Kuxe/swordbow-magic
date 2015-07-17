#include "server.hpp"
#include "client.hpp"
#include "packet.hpp"
#include "messagetypes.hpp"

Server::Server(int argc, char** argv) :
		socket(Socket("swordbow-magic")),
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

        switch(type) {
			case MESSAGE_TYPE::OUTDATED: {
                std::cout << "This packet is outdated, to late! Sluggish!" << endl;
            } break;

			case MESSAGE_TYPE::CONNECT: {
				onConnect(client);
			} break;

			case MESSAGE_TYPE::DISCONNECT: {
				onDisconnect(client);
			} break;

			case MESSAGE_TYPE::INPUTDATA: {
				auto typedPacket = socket.get<Packet<InputData>>(bytesRead);
				inputDataToInputComponent(client, typedPacket.getData());
			} break;

			default: {
                std::cout << "WARNING: Message without proper type received. This is probably a bug." << std::endl;
                std::cout << "Either server-side handling for that message isn't implemented";
                std::cout << " or a client sent a message with a bogus messagetype";
                std::cout << " or the messagetype was wrongly altered somewhere" << std::endl;
            };
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

void Server::onConnect(const IpAddress& ipAddress) {
	auto fatManId = entityManager.createFatMan({0.0f, 0.0f});
	clients.insert({ipAddress, fatManId});

	//Client should get a copy of all components
	send(ipAddress);

	//Make the client aware of its ID and register the ID to client camerasytem
	const std::pair<ID, System::Identifier> data {fatManId, System::CAMERA};

	using Type = Packet<std::pair<ID, System::Identifier>>;
	auto cameraPacket = Type {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::CONNECT,
		data,
		sizeof(data)
	};

	socket.send<Type>(ipAddress, cameraPacket);

	//Whenever a client connects, tell the client what entities
	//should be in what systems on the client-side
	for(auto tuple : entityManager.entityClientSystemMap) {
		auto id = tuple.first;
		vector<System::Identifier>& systems = tuple.second;
		for(auto system : systems) {
			const std::pair<ID, System::Identifier> data {id, system};

			using Type = Packet<std::pair<ID, System::Identifier>>;
			auto packet = Type {
				stringhash("swordbow-magic"),
				MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM,
				data,
				sizeof(data)
			};
			socket.send<Type>(ipAddress, packet);
		}
	}
}

void Server::onDisconnect(const IpAddress& ipAddress) {
	entityManager.remove(clients.at(ipAddress));
	clients.erase(ipAddress);
}

void Server::send(const IpAddress& ipAddress) {
	using mcType = Packet<Components<MoveComponent>>;
	auto mcpacket = mcType {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::MOVECOMPONENTS,
		componentManager.moveComponents,
		sizeof(componentManager.moveComponents)
	};
	socket.send<mcType>(ipAddress, mcpacket);

	using rcType = Packet<Components<RenderComponent>>;
	auto rcpacket = rcType {
		stringhash("swordbow-magic"),
		MESSAGE_TYPE::RENDERCOMPONENTS,
		componentManager.renderComponents,
		sizeof(componentManager.renderComponents)
	};
	socket.send<rcType>(ipAddress, rcpacket);
}

void Server::send() {
	for(auto it : clients) {
		send(it.first);
	}
}

void Server::sendDiff(const IpAddress& ipAddress) {

	//Only send movecomponentsdiff if there actually was a difference
	//If not for this if-check, then the server would DDOS clients with
	//emtpy MOVECOMPONENTSDIFF-packets...
	if(!componentManager.moveComponentsDiff.empty()) {
		using mcType = Packet<Components<MoveComponent>>;
		auto mcpacket = mcType {
			stringhash("swordbow-magic"),
			MESSAGE_TYPE::MOVECOMPONENTSDIFF,
			componentManager.moveComponentsDiff,
			sizeof(componentManager.moveComponentsDiff)
		};
		socket.send<mcType>(ipAddress, mcpacket);
	}

	if(!componentManager.renderComponentsDiff.empty()) {
		using rcType = Packet<Components<RenderComponent>>;
		auto rcpacket = rcType {
			stringhash("swordbow-magic"),
			MESSAGE_TYPE::RENDERCOMPONENTSDIFF,
			componentManager.renderComponentsDiff,
			sizeof(componentManager.renderComponentsDiff)
		};
		socket.send<rcType>(ipAddress, rcpacket);
	}
}

void Server::sendDiff() {
	for(auto it : clients) {
		sendDiff(it.first);
	}
}

void Server::inputDataToInputComponent(const IpAddress& ipAddress, InputData& data) {
	//Get id of client
	auto id = clients.at(ipAddress);
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
