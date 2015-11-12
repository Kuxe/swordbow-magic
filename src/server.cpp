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
		packetManager("swordbow-magic"),
		systemManager(&componentManager, &deltaTime),
		sizeBoundingBox(&componentManager),
		entityManager(&systemManager, &componentManager, &idManager, &clients, &packetManager),
		sizeHashGridSystem(&sizeBoundingBox),
		collisionSystem(&sizeHashGridSystem),
		removeSystem(&entityManager),
		attackSystem(&sizeHashGridSystem, &clients, &packetManager),
		birdSystem(&entityManager) {

	Logger::log("Starting server", Log::INFO);
	printGeneralInfo();

	constexpr short port = 47293;
	packetManager.open(port);

	//Initialize time in deltatimer to something not zero, just to avoid velocities going to zero
	deltaTime.start();
	deltaTime.stop();

	//Add systems to systemmanager
	systemManager.add(&moveDiffSystem);
	systemManager.add(&renderDiffSystem);
	systemManager.add(&inputSystem);
	systemManager.add(&accelerationSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&birdSystem);
	systemManager.add(&animationSystem);
	systemManager.add(&sizeHashGridSystem);
	systemManager.add(&collisionSystem);
	systemManager.add(&attackSystem);
	systemManager.add(&healthSystem);
	systemManager.add(&removeSystem);
	systemManager.add(&initialComponentsSystem);

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
	packetManager.close();
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
    packetManager.receive(*this);

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

	//In case a client reconnects, the packetManager shouldn't reject the newly
	//reconnected clients packets since the packetManager knows of latter remoteSequences
	//since the previous session on client machine
	packetManager.resetRemoteSequenceNumber(ipAddress);

	//Client should get a copy of all components
	sendInitial(ipAddress);

	//Make the client aware of its ID and register the ID to client camerasytem
	using DataType = std::pair<ID, System::Identifier>;
	const DataType data {playerId, System::CAMERA};
	send<DataType, MESSAGE_TYPE::CONNECT>(ipAddress, data);
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

	const auto& initialComponents = initialComponentsSystem.getInitialComponents();
	const ID componentsPerContainer = 64;
	const auto size = initialComponents.first.size();
	const int numContainers = (size / componentsPerContainer) + ((size % componentsPerContainer) > 0 ? 1 : 0);
	send<int, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>(ipAddress, numContainers);

	std::ostringstream oss1;
	oss1 << "Sending BEGIN_TRANSMITTING_INITIAL_COMPONENTS-packet to " << ipAddress;
	Logger::log(oss1, Log::VERBOSE);

	const auto& initialMcs = initialComponents.first;
	const auto& initialRcs = initialComponents.second;
	const auto& smallerMcs = initialMcs.split(componentsPerContainer);
	const auto& smallerRcs = initialRcs.split(componentsPerContainer);

	std::ostringstream oss;
	oss << "About to send #" << smallerMcs.size() << " small containers to client";
	Logger::log(oss, Log::VERBOSE);

	using namespace std::literals;
	using DataType = const std::pair<const Components<const MoveComponent&>, const Components<const RenderComponent&>>;
	for(int i = 0; i < smallerMcs.size(); i++) {
		send<DataType, MESSAGE_TYPE::INITIAL_COMPONENTS>(ipAddress, {smallerMcs[i], smallerRcs[i]});
		std::this_thread::sleep_for(1ms);
	}

	std::ostringstream oss2;
	oss2 << "Sending END_TRANSMITTING_INITIAL_COMPONENTS-packet to " << ipAddress;
	Logger::log(oss2, Log::VERBOSE);
	send<bool, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>(ipAddress, true);
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
		movediffs.insert({id, componentManager.moveComponents.at(id)});
	}

	if(!movediffs.empty()) {
		send<Components<MoveComponent>, MESSAGE_TYPE::MOVECOMPONENTSDIFF>(ipAddress, movediffs);
		std::ostringstream oss;
		oss << "Sending MOVECOMPONENTSDIFF-packet containing ids: ";
		for(auto pair : movediffs) {
			oss << pair.first << ", ";
		}
		Logger::log(oss, Log::VERBOSE);
	}

	//Get all rendercomponents of members of renderdiffsystem
	//and store them in a new Components<RendersComponent>
	Components<RenderComponent> renderdiffs;
	for(ID id : renderDiffSystem) {
		renderdiffs.insert({id, componentManager.renderComponents.at(id)});
	}

	if(!renderdiffs.empty()) {
		send<Components<RenderComponent>, MESSAGE_TYPE::RENDERCOMPONENTSDIFF>(ipAddress, renderdiffs);
		std::ostringstream oss;
		oss << "Sending RENDERCOMPONENTSDIFF-packet containing ids: ";
		for(auto pair : renderdiffs) {
			oss << pair.first << ", ";
		}
		Logger::log(oss, Log::VERBOSE);
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
			send<bool, MESSAGE_TYPE::KEEP_ALIVE>(ipAddress, true);
			Logger::log("Sending KEEP_ALIVE-packet", Log::VERBOSE);
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

void Server::accept(Packet<OUTDATED_TYPE, MESSAGE_TYPE::OUTDATED>& packet, const IpAddress& sender) {
	Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
}

void Server::accept(Packet<CONNECT_TYPE, MESSAGE_TYPE::CONNECT>& packet, const IpAddress& sender) {
	Logger::log("Received CONNECT packet", Log::INFO);
	onConnect(sender);
}

void Server::accept(Packet<DISCONNECT_TYPE, MESSAGE_TYPE::DISCONNECT>& packet, const IpAddress& sender) {
	Logger::log("Received DISCONNECT packet", Log::INFO);
	onDisconnect(sender);
}

void Server::accept(Packet<INPUTDATA_TYPE, MESSAGE_TYPE::INPUTDATA>& packet, const IpAddress& sender) {
	Logger::log("Received INPUTDATA packet", Log::VERBOSE);

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
	if(clients.find(sender) != clients.end() && ics.find(clients.at(sender).id) != ics.end()) {
		inputDataToInputComponent(sender, packet.getData());
	}
}

void Server::accept(Packet<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::INITIAL_COMPONENTS>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<MOVECOMPONENTSDIFF_TYPE, MESSAGE_TYPE::MOVECOMPONENTSDIFF>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<RENDERCOMPONENTSDIFF_TYPE, MESSAGE_TYPE::RENDERCOMPONENTSDIFF>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<PLAY_SOUND_TYPE, MESSAGE_TYPE::PLAY_SOUND>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<REGISTER_ID_TO_SYSTEM_TYPE, MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<REMOVE_ID_TYPE, MESSAGE_TYPE::REMOVE_ID>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<REMOVE_ID_FROM_SYSTEM_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<REMOVE_ID_FROM_SYSTEMS_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<ACTIVATE_ID_TYPE, MESSAGE_TYPE::ACTIVATE_ID>& packet, const IpAddress& sender) {

}

void Server::accept(Packet<CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::CONGESTED_CLIENT>& packet, const IpAddress& sender) {
	Logger::log("Received CONGESTED_CLIENT packet", Log::INFO);
	auto& client = clients.at(sender);
	client.congested = true;
	client.congestionTimer.start();
}

void Server::accept(Packet<NOT_CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::NOT_CONGESTED_CLIENT>& packet, const IpAddress& sender) {
	Logger::log("Received NOT_CONGESTED_CLIENT packet", Log::INFO);
	clients.at(sender).congested = false;
}

void Server::accept(Packet<KEEP_ALIVE_TYPE, MESSAGE_TYPE::KEEP_ALIVE>& packet, const IpAddress& sender) {
    
}

int main(int argc, char** argv) {
	Logger::openLogfile("serverlog.txt");
	Logger::level = Log::ERROR; 

	/** Begin by parsin passed program arguments **/
	for(int i = 0; i < argc; i++) {
        std::string command(argv[i]);
        size_t pos = command.rfind("--log=");
        if(pos != std::string::npos) {
            std::string logstr = command.substr(pos+6);

            if(!logstr.compare("VERBOSE")) {
            	Logger::level = Log::VERBOSE;
            } else if(!logstr.compare("INFO")) {
                Logger::level = Log::INFO;
            } else if(!logstr.compare("WARNING")) {
                Logger::level = Log::WARNING;
            } else if(!logstr.compare("ERROR")) {
                Logger::level = Log::ERROR;
            } else {
                Logger::log("Not valid value for --log=<VERBOSE|INFO|WARNING|ERROR>", Log::ERROR);
                return -1;
            }
        }
    }

	Server server(argc, argv);
	server.run();
	Logger::closeLogfile();
	return 0;
}








//
