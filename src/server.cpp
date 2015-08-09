#include "server.hpp"

#include <thread>

/** For world generation **/
#include "world.hpp"

/** For network **/
#include "packet.hpp"
#include "messagetypes.hpp"

/** For logging **/
#include "logger.hpp"

#include <SDL2/SDL.h> //For SDL_keys

Server::Server(int argc, char** argv) :
		socket("swordbow-magic"),
		systemManager(&componentManager, &deltaTime),
		sizeBoundingBox(&componentManager),
		entityManager(&systemManager, &componentManager, &idManager, &clients, &socket),
		sizeHashGridSystem(&sizeBoundingBox),
		collisionSystem(&sizeHashGridSystem),
		removeSystem(&entityManager),
		attackSystem(&sizeHashGridSystem, &clients, &socket) {

	Logger::log("Starting server", Log::INFO);
	printGeneralInfo();

	constexpr short port = 47293;
	socket.open(port);

	//Add systems to systemmanager
	systemManager.add(&moveDiffSystem);
	systemManager.add(&renderDiffSystem);
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

	for(int i = 0; i < argc; i++) {
		if(strcmp(argv[i], "tps") == 0) {
			tps = atof(argv[i+1]);
			break;
		}
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

	//Run the entity-component-system
	componentsMutex.lock();
	systemManager.update();
	componentsMutex.unlock();

	//Broadcast new gamestate to clients
	sendDiff();

	//Receive data from clients...
    IpAddress client;
    unsigned char type;
    int bytesRead;
    socket.receive(client, type, bytesRead);

	//If any data was received, check its type and take appropiate action
    if(bytesRead > 0) {

        switch(type) {
			case MESSAGE_TYPE::OUTDATED: {
				Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
            } break;

			case MESSAGE_TYPE::CONNECT: {
				onConnect(client);
			} break;

			case MESSAGE_TYPE::DISCONNECT: {
				onDisconnect(client);
			} break;

			case MESSAGE_TYPE::INPUTDATA: {
				auto typedPacket = socket.get<Packet<InputData>>(bytesRead);

				//Check if client established a connection to the server, ie
				//asked for a entity id. It could be that the client had a connection
				//to another session of the server, in which case the clients id might
				//not be present on this session of the server... Those clients should
				//restart / reconnect. So don't do anything with this packet if
				//the server doesn't think the client has connected
				if(clients.find(client) != clients.end()) {
					inputDataToInputComponent(client, typedPacket.getData());
				}

			} break;

			default: {
                std::cout << "WARNING: Message without proper type received. This is probably a bug." << std::endl;
                std::cout << "Either server-side handling for that message isn't implemented";
                std::cout << " or a client sent a message with a bogus messagetype";
                std::cout << " or the messagetype was wrongly altered somewhere" << std::endl;
            };
        }
	}

	//Limit server-speed to 60fps (rather 60 tick per second)
	//Check the elapsed time for the current step, if it is lower than
	//16.6666... then it's faster than 60fps so sleep until 16.666ms has passed
	using ms = std::chrono::milliseconds;
	using fdur = std::chrono::duration<float>;
	const float sleep_float = 1.0f/tps - deltaTime.elapsed();
	const ms sleep = std::chrono::duration_cast<ms>(fdur(sleep_float));
	std::this_thread::sleep_for(sleep);

	deltaTime.stop();
}

void Server::onConnect(const IpAddress& ipAddress) {
	std::ostringstream oss;
	oss << ipAddress << " connected";
	Logger::log(oss, Log::INFO);
	auto fatManId = entityManager.createFatMan({0.0f, 0.0f});
	clients.insert({ipAddress, {1, fatManId}});

	//In case a client reconnects, the socket shouldn't reject the newly
	//reconnected clients packets since the socket knows of latter remoteSequences
	//since the previous session on client machine
	socket.resetRemoteSequenceNumber(ipAddress);

	//Client should get a copy of all components
	send(ipAddress);

	//Make the client aware of its ID and register the ID to client camerasytem
	const std::pair<ID, System::Identifier> data {fatManId, System::CAMERA};

	auto& clientData = clients.at(ipAddress);

	using Type = Packet<std::pair<ID, System::Identifier>>;
	auto cameraPacket = Type {
		stringhash("swordbow-magic"),
		clientData.sequence++,
		MESSAGE_TYPE::CONNECT,
		data,
		sizeof(data)
	};

	socket.send<Type>(ipAddress, cameraPacket);
}

void Server::onDisconnect(const IpAddress& ipAddress) {
	//If this ipaddress isn't a client, do nothing
	if(clients.find(ipAddress) != clients.end()) {

		std::ostringstream oss;
		oss << ipAddress << " disconnected";
		Logger::log(oss, Log::INFO);

		entityManager.remove(clients.at(ipAddress).id);
		clients.erase(ipAddress);
	}
}

void Server::send(const IpAddress& ipAddress) {

	auto& clientData = clients.at(ipAddress);

	using mcType = Packet<Components<MoveComponent>>;
	auto mcpacket = mcType {
		stringhash("swordbow-magic"),
		clients.at(ipAddress).sequence++,
		MESSAGE_TYPE::MOVECOMPONENTS,
		componentManager.moveComponents,
		sizeof(componentManager.moveComponents)
	};
	socket.send<mcType>(ipAddress, mcpacket);

	using rcType = Packet<Components<RenderComponent>>;
	auto rcpacket = rcType {
		stringhash("swordbow-magic"),
		clientData.sequence++,
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

	auto& clientData = clients.at(ipAddress);

	//Get all movecomponents of members of movediffsystem
	//and store them in a new Components<MoveComponent>
	Components<MoveComponent> movediffs;
	for(ID id : moveDiffSystem) {
		Logger::disable();
		movediffs.insert({id, componentManager.moveComponents.at(id)});
		Logger::enable();
	}

	if(!movediffs.empty()) {
		using mcType = Packet<Components<MoveComponent>>;
		auto mcpacket = mcType {
			stringhash("swordbow-magic"),
			clientData.sequence++,
			MESSAGE_TYPE::MOVECOMPONENTSDIFF,
			movediffs,
			sizeof(movediffs)
		};
		socket.send<mcType>(ipAddress, mcpacket);
	}

	//Get all rendercomponents of members of movediffsystem
	//and store them in a new Components<RendersComponent>
	Components<RenderComponent> renderdiffs;
	for(ID id : moveDiffSystem) {
		Logger::disable();
		renderdiffs.insert({id, componentManager.renderComponents.at(id)});
		Logger::enable();
	}


	if(!renderdiffs.empty()) {
		using rcType = Packet<Components<RenderComponent>>;
		auto rcpacket = rcType {
			stringhash("swordbow-magic"),
			clientData.sequence++,
			MESSAGE_TYPE::RENDERCOMPONENTSDIFF,
			renderdiffs,
			sizeof(renderdiffs)
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
	auto id = clients.at(ipAddress).id;
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

void Server::printGeneralInfo() {
	//Print some worst-case memory statistics, assuming
	//all entities have all components
	//(which isn't the case)
	uint64_t bytePerMegabyte = 1048576;
	uint64_t entityByteSize = ComponentManager::sizePerEntity();
	uint64_t allEntitiesByteSize = IdManager::MAX_IDS * entityByteSize;
	uint64_t allEntitiesMegabyteSize = allEntitiesByteSize / bytePerMegabyte;
	std::ostringstream oss;
	oss << "There can be at most " << IdManager::MAX_IDS << " ids";
	oss << " each of size " << entityByteSize << "bytes summing";
	oss << " up to " << allEntitiesByteSize;
	oss << "bytes (" << allEntitiesMegabyteSize;
	oss << "MB)";
	Logger::log(oss, Log::INFO);
}

int main(int argc, char** argv) {
	Logger::level = Log::INFO;
	Server server(argc, argv);
	server.run();
	return 0;
}








//
