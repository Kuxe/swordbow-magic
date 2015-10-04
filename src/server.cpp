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
	systemManager.update();

	//Broadcast new gamestate to clients
	sendDiff();

	//Special packet that clients must receive
	//in order to be considered connected
	//Clients will timeout if keepalive aren't
	//received frequently
	sendKeepAlive();

	//Receive data from clients...
    IpAddress clientIp;
    MESSAGE_TYPE type;
    int bytesRead;
    socket.receive(clientIp, type, bytesRead);

	//If any data was received, check its type and take appropiate action
    if(bytesRead > 0) {

        switch(type) {
			case MESSAGE_TYPE::OUTDATED: {
				Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
            } break;

			case MESSAGE_TYPE::CONNECT: {
				Logger::log("Received CONNECT packet", Log::INFO);
				onConnect(clientIp);
			} break;

			case MESSAGE_TYPE::DISCONNECT: {
				Logger::log("Received DISCONNECT packet", Log::INFO);
				onDisconnect(clientIp);
			} break;

			case MESSAGE_TYPE::INPUTDATA: {
				Logger::log("Received INPUTDATA packet", Log::INFO);
				auto typedPacket = socket.get<Packet<InputData>>(bytesRead);

				//Check if client established a connection to the server, ie
				//asked for a entity id. It could be that the client had a connection
				//to another session of the server, in which case the clients id might
				//not be present on this session of the server... Those clients should
				//restart / reconnect. So don't do anything with this packet if
				//the server doesn't think the client has connected. It might be the
				//case that the id is connected currently, but was just killed but hasnt
				//been notified of its death so it keeps trying to move. In that case,
				//its fine to ignore any input trying to steer the deceased entity.
				auto& ics = componentManager.inputComponents;
				if(clients.find(clientIp) != clients.end() && ics.find(clients.at(clientIp).id) != ics.end()) {
					inputDataToInputComponent(clientIp, typedPacket.getData());
				}

			} break;

			case MESSAGE_TYPE::CONGESTED_CLIENT: {
				Logger::log("Received CONGESTED_CLIENT packet", Log::INFO);
				auto& client = clients.at(clientIp);
				client.congested = true;
				client.congestionTimer.start();
			} break;

			case MESSAGE_TYPE::NOT_CONGESTED_CLIENT: {
				Logger::log("Received NOT_CONGESTED_CLIENT packet", Log::INFO);
				clients.at(clientIp).congested = false;
			} break;

			default: {
				std::ostringstream oss;
                oss << "Message without proper type received. This is probably a bug.";
                oss << " Either server-side handling for that message isn't implemented";
                oss << " or a client sent a message with a bogus messagetype";
                oss << " or the messagetype was wrongly altered somewhere (type: " << type << ")";
                Logger::log(oss, Log::WARNING);
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
	auto playerId = entityManager.createFatMan({10.0f, 20.0f});
	clients.insert({ipAddress, {1, playerId}});

	//In case a client reconnects, the socket shouldn't reject the newly
	//reconnected clients packets since the socket knows of latter remoteSequences
	//since the previous session on client machine
	socket.resetRemoteSequenceNumber(ipAddress);

	//Client should get a copy of all components
	sendInitial(ipAddress);

	//Make the client aware of its ID and register the ID to client camerasytem
	using DataType = std::pair<ID, System::Identifier>;
	const DataType data {playerId, System::CAMERA};
	send<DataType>(ipAddress, data, MESSAGE_TYPE::CONNECT);
}

void Server::onDisconnect(const IpAddress& ipAddress) {
	//If this ipaddress isn't a client, do nothing
	if(clients.find(ipAddress) != clients.end()) {

		std::ostringstream oss;
		oss << ipAddress << " disconnected";
		Logger::log(oss, Log::INFO);

		const auto& clientData = clients.at(ipAddress);

		//It could be that the client has no ID,
		//because of recent death or something
		//In that case do nothing
		if(entityManager.idExists(clientData.id)) {
			entityManager.remove(clientData.id);
		}

		clients.erase(ipAddress);
	}
}

void Server::sendInitial(const IpAddress& ipAddress) {

	send<bool>(ipAddress, true, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS);

	using DataType = std::pair<Components<MoveComponent>, Components<RenderComponent>>;

	const ID split = 64;
	const auto& smallerMcs = componentManager.moveComponents.split(split);
	const auto& smallerRcs = componentManager.renderComponents.split(split);

	std::ostringstream oss;
	oss << "About to send #" << smallerMcs.size() << " small containers to client";
	Logger::log(oss, Log::INFO);

	using namespace std::literals;
	for(int i = 0; i < smallerMcs.size(); i++) {
		const DataType& data = {smallerMcs[i], smallerRcs[i]};
		send<DataType>(ipAddress, data, MESSAGE_TYPE::INITIAL_COMPONENTS);
		std::this_thread::sleep_for(1ms);
	}

	send<bool>(ipAddress, true, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS);
}

void Server::sendInitial() {
	for(auto it : clients) {
		sendInitial(it.first);
	}
}

void Server::sendDiff(const IpAddress& ipAddress) {

	//Get all movecomponents of members of movediffsystem
	//and store them in a new Components<MoveComponent>
	Components<MoveComponent> movediffs;
	for(ID id : moveDiffSystem) {
		Logger::disable();
		movediffs.insert({id, componentManager.moveComponents.at(id)});
		Logger::enable();
	}

	if(!movediffs.empty()) {
		send<Components<MoveComponent>>(ipAddress, movediffs, MESSAGE_TYPE::MOVECOMPONENTSDIFF);
		std::ostringstream oss;
		oss << "Sending MOVECOMPONENTSDIFF-packet containing ids: ";
		for(auto pair : movediffs) {
			oss << pair.first << ", ";
		}
		Logger::log(oss, Log::INFO);
	}

	//Get all rendercomponents of members of movediffsystem
	//and store them in a new Components<RendersComponent>
	Components<RenderComponent> renderdiffs;
	for(ID id : moveDiffSystem) {
		Logger::disable();
		renderdiffs.insert({id, componentManager.renderComponents.at(id)});
		Logger::enable();
	}

	for(ID id : renderDiffSystem) {
		Logger::disable();
		renderdiffs.insert({id, componentManager.renderComponents.at(id)});
		Logger::enable();
	}


	if(!renderdiffs.empty()) {
		send<Components<RenderComponent>>(ipAddress, renderdiffs, MESSAGE_TYPE::RENDERCOMPONENTSDIFF);
		std::ostringstream oss;
		oss << "Sending RENDERCOMPONENTSDIFF-packet containing ids: ";
		for(auto pair : renderdiffs) {
			oss << pair.first << ", ";
		}
		Logger::log(oss, Log::INFO);
	}
}

void Server::sendDiff() {
	for(auto it : clients) {
		auto& client(it.second);

		//If the client whined about being congested
		//then check if the congestion-timer has elapsed
		//and if so, reset the congestion timer
		if(client.congested) {
			if(client.congestionTimer.elapsed() >= 100.0f) {
				sendDiff(it.first);
				client.congestionTimer.start();
			}
		} else {
			sendDiff(it.first);
		}
	}

	moveDiffSystem.clear();
	renderDiffSystem.clear();
}

void Server::sendKeepAlive() {
	if(keepAlive.elapsed() > keepAliveInterval) {
		keepAlive.start();
		for(auto it : clients) {
			const auto& client(it.second);
			const auto& ipAddress = it.first;
			send<bool>(ipAddress, true, MESSAGE_TYPE::KEEP_ALIVE);
			Logger::log("Sending KEEP_ALIVE-packet", Log::INFO);
		}
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
	Logger::level = Log::WARNING;
	Server server(argc, argv);
	server.run();
	return 0;
}








//
