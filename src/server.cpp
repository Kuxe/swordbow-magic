#include "server.hpp"

#include <thread>

/** For world generation **/
#include "world.hpp"

/** For network **/
#include "packet.hpp"
#include "messagetypes.hpp"

/** For logging **/
#include "logger.hpp"

/** For command line argument parsing **/
#include "args.hxx"

/** For keypress parsing **/
#include <GLFW/glfw3.h>

Server::Server(bool smallWorld, float tps) :
		packetManager("swordbow-magic"),
		systemManager(&componentManager, &deltaTime),
		sizeBoundingBox(&componentManager),
		entityManager(&systemManager, &componentManager, &idManager, &clients, &packetManager),
		sizeHashGridSystem(&sizeBoundingBox),
		collisionSystem(&sizeHashGridSystem),
		removeSystem(&entityManager),
		attackSystem(&sizeHashGridSystem, &clients, &packetManager),
		birdSystem(&entityManager),
		tps(tps) {

	Logger::log("Starting server", Logger::INFO);
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
	if(smallWorld) {
		world.createDebugWorld();
	} else {
		world.createWorld();
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
    packetManager.receive();
    decltype(this) thisptr = this;
    packetManager.poll((PacketHandler**)&thisptr);

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
	Logger::log(oss, Logger::INFO);
	auto playerId = entityManager.createFatMan(glm::mat4());
	clients.insert({ipAddress, {1, playerId}});

	//In case a client reconnects, the packetManager shouldn't reject the newly
	//reconnected clients packets since the packetManager knows of latter remoteSequences
	//since the previous session on client machine
	packetManager.resetRemoteSequenceNumber(ipAddress);

	//Client should get a copy of all components
	sendInitial(ipAddress);

	//Make the client aware of its ID and register the ID to client camerasytem
	send<ServerReplyToConnectData, MESSAGE_TYPE::SERVER_REPLY_TO_CONNECT>(ipAddress, {playerId, System::CAMERA});
}

void Server::onDisconnect(const IpAddress& ipAddress) {
	//If this ipaddress isn't a client, do nothing
	if(clients.find(ipAddress) != clients.end()) {

		std::ostringstream oss;
		oss << ipAddress << " disconnected";
		Logger::log(oss, Logger::INFO);

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

	std::pair<Components<MoveComponent>, Components<RenderComponent>> initialComponents = initialComponentsSystem.getInitialComponents();
	const ID componentsPerContainer = 64;
	const auto size = initialComponents.first.size();
	const int numContainers = (size / componentsPerContainer) + ((size % componentsPerContainer) > 0 ? 1 : 0);
	send<BeginTransmittingInitialComponentsData, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>(ipAddress, {numContainers});

	std::ostringstream oss1;
	oss1 << "Sending BEGIN_TRANSMITTING_INITIAL_COMPONENTS-packet to " << ipAddress;
	Logger::log(oss1, Logger::VERBOSE);

	Components<MoveComponent> initialMcs = initialComponents.first;
	Components<RenderComponent> initialRcs = initialComponents.second;
	std::vector<Components<MoveComponent>> smallerMcs = initialMcs.split(componentsPerContainer);
	std::vector<Components<RenderComponent>> smallerRcs = initialRcs.split(componentsPerContainer);

	std::ostringstream oss;
	oss << "About to send #" << smallerMcs.size() << " small containers to client";
	Logger::log(oss, Logger::VERBOSE);

	using namespace std::literals;
	for(int i = 0; i < smallerMcs.size(); i++) {
		send<InitialComponentsData, MESSAGE_TYPE::INITIAL_COMPONENTS>(ipAddress, {smallerMcs[i], smallerRcs[i]});
		std::this_thread::sleep_for(1ms);
	}

	std::ostringstream oss2;
	oss2 << "Sending END_TRANSMITTING_INITIAL_COMPONENTS-packet to " << ipAddress;
	Logger::log(oss2, Logger::VERBOSE);
	send<EndTransmittingInitialComponentsData, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>(ipAddress, {true});
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
		send<MoveComponentsDiffData, MESSAGE_TYPE::MOVECOMPONENTSDIFF>(ipAddress, {movediffs});
		std::ostringstream oss;
		oss << "Sending MOVECOMPONENTSDIFF-packet containing ids: ";
		for(auto pair : movediffs) {
			oss << pair.first << ", ";
		}
		Logger::log(oss, Logger::VERBOSE);
	}

	//Get all rendercomponents of members of renderdiffsystem
	//and store them in a new Components<RendersComponent>
	Components<RenderComponent> renderdiffs;
	for(ID id : renderDiffSystem) {
		renderdiffs.insert({id, componentManager.renderComponents.at(id)});
	}

	if(!renderdiffs.empty()) {
		send<RenderComponentsDiffData, MESSAGE_TYPE::RENDERCOMPONENTSDIFF>(ipAddress, {renderdiffs});
		std::ostringstream oss;
		oss << "Sending RENDERCOMPONENTSDIFF-packet containing ids: ";
		for(auto pair : renderdiffs) {
			oss << pair.first << ", ";
		}
		Logger::log(oss, Logger::VERBOSE);
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
			send<KeepAliveData, MESSAGE_TYPE::KEEP_ALIVE>(ipAddress, {true});
			Logger::log("Sending KEEP_ALIVE-packet", Logger::VERBOSE);
		}
	}
}

void Server::inputDataToInputComponent(const IpAddress& ipAddress, const InputData& data) {
	//Get id of client
	
	auto id = clients.at(ipAddress).id;
	auto& ic = componentManager.inputComponents.at(id);

	ic.presses = data.presses;

	for(auto keypress : data.presses) {
		//Figure out what key was pressed
		switch(keypress) {
			case GLFW_KEY_W: {
				ic.w = true;
				break;
			}
			case GLFW_KEY_A: {
				ic.a = true;
				break;
			}
			case GLFW_KEY_S: {
				ic.s = true;
			break;
			}
			case GLFW_KEY_D: {
				ic.d = true;
				break;
			}
			case GLFW_KEY_E: {
				ic.e = true;
				break;
			}
			case GLFW_KEY_Q: {
				ic.q = true;
				break;
			}
			case GLFW_KEY_SPACE: {
				ic.space = true;
				break;
			}
		}
	}

	ic.releases = data.releases;

	for(auto keyrelease : data.releases) {
		//Figure out what key was released
		switch(keyrelease) {
			case GLFW_KEY_W: {
				ic.w = false;
				break;
			}
			case GLFW_KEY_A: {
				ic.a = false;
				break;
			}
			case GLFW_KEY_S: {
				ic.s = false;
				break;
			}
			case GLFW_KEY_D: {
				ic.d = false;
				break;
			}
			case GLFW_KEY_E: {
				ic.e = false;
				break;
			}
			case GLFW_KEY_Q: {
				ic.q = false;
				break;
			}
			case GLFW_KEY_SPACE: {
				ic.space = false;
				break;
			}
		}
	}

	/** Update position of mouse in input component
		We need to keep the old mouse position in order
		to compute delta mouse position whenever needed **/
	ic.mousePos = data.mousePos;
	
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
	Logger::log(oss, Logger::INFO);
}

std::string Server::name() const {
	return "Server";
}

void Server::greet(IPacket* packet) {
	packet->greet(this);
}

void Server::handle(const OutdatedData*) {
	Logger::log("This packet is outdated, to late! Sluggish!", Logger::WARNING);
}

void Server::handle(const ConnectToServerData* data) {
	Logger::log("Received CONNECT packet", Logger::INFO);
	onConnect(data->ip);
}

void Server::handle(const DisconnectData* data) {
	Logger::log("Received DISCONNECT packet", Logger::INFO);
	onDisconnect(data->ip);
}

void Server::handle(const InputDataData* data) {
	Logger::log("Received INPUTDATA packet", Logger::VERBOSE);

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
	if(clients.find(data->ip) != clients.end() && ics.find(clients.at(data->ip).id) != ics.end()) {
		inputDataToInputComponent(data->ip, data->data);
	}
}

void Server::handle(const CongestedClientData* data) {
	Logger::log("Received CONGESTED_CLIENT packet", Logger::INFO);
	auto& client = clients.at(data->ip);
	client.congested = true;
	client.congestionTimer.start();
}

void Server::handle(const NotCongestedClientData* data) {
	Logger::log("Received NOT_CONGESTED_CLIENT packet", Logger::INFO);
	clients.at(data->ip).congested = false;
}

int main(int argc, char** argv) {

	/** Parse args (using nifty library by Taylor C. Richberger https://github.com/Taywee/args)**/
	args::ArgumentParser parser("swordbow-magic.", "swordbow-magic says bye!.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::Flag printVasFlag(parser, "printvas", "Print integer handle to vertex arrays loaded by renderer", {"printvas"});
	args::ValueFlag<float> gametimeSpeedFactor(parser, "gametime", "Increases gametime speed by a factor", {"gt", "gametimespeedfactor"});
	args::Flag forceDaytime(parser, "forcedaytime", "Disable day/night cycle", {"forcedaytime"});
	args::Flag smallWorldFlag(parser, "smallworld", "Create a tiny world", {"smallworld"});
	args::ValueFlag<float> tpsflag(parser, "tick per second", "Set server tickrate", {"tps"});

    std::unordered_map<std::string, Log> map{
        {"VERBOSE", 1},
        {"INFO", 2},
        {"WARNING", 3},
        {"ERROR", 4},
    };

    args::MapFlag<std::string, uint8_t> logflag(parser, "VERBOSE|INFO|WARNING|ERROR", "Set logging level", {"log"}, map);


    try {
    	parser.ParseCLI(argc, argv);
    } catch (args::Help) {
    	std::cout << parser;
    	return 0;
    } catch (args::ParseError e) {
    	std::cerr << e.what() << std::endl;
    	std::cerr << parser;
    	return -1;
    }

    if(logflag) {
		Logger::openLogfile("serverlog.txt");
		Logger::enable();
		Logger::level = args::get(logflag);
	}

	float tps = 60.0f;
	if(tpsflag) {
		tps = args::get(tpsflag);
	}

	bool smallWorld = false;
	if(smallWorldFlag) {
		smallWorld = args::get(smallWorldFlag);
	}

	Server server(smallWorld, tps);
	server.run();
	Logger::closeLogfile();
	return 0;
}








//
