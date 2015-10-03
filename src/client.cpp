#include "client.hpp"

/** For logging **/
#include "logger.hpp"

/** For event-handling **/
#include <SDL2/SDL.h>

/** For network **/
#include "messagetypes.hpp"
#include "packet.hpp"
#include "systemidentifiers.hpp"

#include "inputdata.hpp"

/** For parsing **/
#include <string>

Client::Client(int argc, char** argv) :
        socket("swordbow-magic"),
        sequence(1),
        systemManager(&componentManager, &deltaTime),
        renderer(argc, argv),
        textureBoundingBox(&componentManager, &renderer),
        textureHashGridSystem(&textureBoundingBox),
        cameraSystem(&renderer),
        renderSystem(&renderer, &textureHashGridSystem, &cameraSystem) {

    Logger::log("Starting client", Log::INFO);

    //Default port
    short port = 47294;

    //Check if port is set
    for(int i = 0; i < argc; i++) {
		if(strcmp(argv[i], "port") == 0) {
			port = std::atoi(argv[i+1]);
		}
	}

    socket.open(port);

    systemManager.add(&textureHashGridSystem);
    systemManager.add(&renderSystem);
    systemManager.add(&cameraSystem);

    //Play some sweet music
	soundEngine.playMusic(Music::NATURE_SOUNDS);
}

Client::~Client() {
    socket.close();
    Logger::log("Destroying client", Log::INFO);
}

void Client::connect(const IpAddress& server) {
    //Start a thread that checks socket for any recieved data
    receiveThreadRunning = true;
    receiveThread = std::thread(&Client::receive, this);

    this->server = server;

    //Connect-request packet
    send<bool>(true, MESSAGE_TYPE::CONNECT);
    keepAlive.start();
    std::ostringstream oss;
    oss << "Sent connect request to " << server;
    Logger::log(oss, Log::INFO);
}

void Client::disconnect() {

    //Stop receiving data on socket
    receiveThreadRunning = false;
    receiveThread.join();

    //Disconnect-request packet
    send<bool>(true, MESSAGE_TYPE::DISCONNECT);

    keepAlive.stop();

    std::ostringstream oss;
    oss << "Disconnected from " << server;
    Logger::log(oss, Log::INFO);

    //Indicate not connected to any server
    server = IpAddress(0, 0, 0, 0, 0);
}

void Client::receive() {
    while(receiveThreadRunning) {

        //Receive data from server...
        IpAddress server;
        MESSAGE_TYPE type;
        int bytesRead;
        socket.receive(server, type, bytesRead);

        //If any data was received, check its type and take appropiate action
        if(bytesRead > 0) {

            /** CRITICAL-SECTION **/
            componentsMutex.lock();

            switch(type) {
                case MESSAGE_TYPE::OUTDATED: {
                    Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
                } break;

                case MESSAGE_TYPE::CONNECT: {
                    Logger::log("Got camera ID from server", Log::INFO);

                    //Got my id. Tell camerasystem to follow that id.
                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& pair = typedPacket.getData();
                    systemManager.getSystem(pair.second)->add(pair.first);
                } break;

                //Server will send this to client for a couple of reasons.
                //In any case the client shouldn't be connected to the server.
                case MESSAGE_TYPE::DISCONNECT: {
                    Logger::log("Received DISCONNECT packet", Log::INFO);
                    disconnect();
                } break;

                //This message to client will put the client in a special "state"
                //where the client will only receive INITIAL_COMPONENTS-packet
                //and END_TRANSMITTING_INITIAL_COMPONENTS. ECS won't update.
                //This is because large worlds must be split into smaller packets,
                //due to packet limitation size, and then sent separately to client.
                //Client may NOT update ECS because it's not guaranteed that the all
                //required entity-components have been received.
                case MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS: {
                    Logger::log("Received BEGIN_TRANSMITTING_INITIAL_COMPONENTS packet", Log::INFO);
                    bool done = false;
                    while(!done) {
                        //Receive data from server...
                        IpAddress server;
                        MESSAGE_TYPE type;
                        int bytesRead;
                        socket.receive(server, type, bytesRead);

                        if(bytesRead > 0) {
                            switch(type) {
                                case MESSAGE_TYPE::INITIAL_COMPONENTS: {
                                    Logger::log("Received INITIAL_COMPONENTS packet", Log::INFO);

                                    using DataType = std::pair<Components<MoveComponent>, Components<RenderComponent>>;
                                    using PacketType = Packet<DataType>;
                                    auto typedPacket = socket.get<PacketType>(bytesRead);

                                    componentManager.moveComponents.sync(typedPacket.getData().first);
                                    componentManager.renderComponents.sync(typedPacket.getData().second);


                                    for(auto& pair : typedPacket.getData().first) {
                                        textureHashGridSystem.add(pair.first);
                                        renderSystem.add(pair.first);
                                    }
                                } break;

                                case MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS: {
                                    Logger::log("Received END_TRANSMITTING_INITIAL_COMPONENTS packet", Log::INFO);
                                    done = true;
                                } break;

                                default: {
                                    std::ostringstream oss;
                                    oss << "Received packet when in receiving-initial-components-mode (type: " << type << ")";
                                    Logger::log(oss, Log::WARNING);
                                } break;
                            }
                        }
                    }
                } break;

                case MESSAGE_TYPE::MOVECOMPONENTS: {
                    Logger::log("Received MOVECOMPONENTS packet", Log::INFO);

                    //All movecomponents were received - handle it
                    auto typedPacket = socket.get<Packet<Components<MoveComponent>>>(bytesRead);
                    componentManager.moveComponents.sync(typedPacket.getData());

                    //It is assumed that any receieved movecomponent should
                    //be added to texturehashgridsystem and rendersystem,
                    //because if not, why would the server send them in the first
                    //place?
                    for(auto& pair : typedPacket.getData()) {
                        textureHashGridSystem.add(pair.first);
                        renderSystem.add(pair.first);
                    }

                } break;

                case MESSAGE_TYPE::RENDERCOMPONENTS: {
                    Logger::log("Received RENDERCOMPONENTS packet", Log::INFO);

                    //All rendercomponents were received - handle it
                    auto typedPacket = socket.get<Packet<Components<RenderComponent>>>(bytesRead);
                    componentManager.renderComponents.sync(typedPacket.getData());

                    for(auto& pair : typedPacket.getData()) {
                        textureHashGridSystem.add(pair.first);
                        renderSystem.add(pair.first);
                    }

                } break;

                case MESSAGE_TYPE::MOVECOMPONENTSDIFF: {
                    Logger::log("Received MOVECOMPONENTSDIFF packet", Log::INFO);

                    //Diff movecomponents were received - handle it
                    auto typedPacket = socket.get<Packet<Components<MoveComponent>>>(bytesRead);
                    componentManager.moveComponents.sync(typedPacket.getData());

                    //It makes sence to auto-activate newly received ids of the
                    //movecomponents in rendersystem, since the server only
                    //sends movecomponents on actual difference, the client
                    //should redraw that difference. So former ACTIVATE_ID message
                    //could be implicit with any component-message, which has the
                    //benefit of not mistakingly activating ids before components
                    //are received. Since system.add is a superset of system.activateId
                    //we might aswell assume adding received components to systems
                    //which makes REGISTER_ID_TO_SYSTEM-message obsolete
                    for(auto& pair : typedPacket.getData()) {
                        textureHashGridSystem.add(pair.first);
                        renderSystem.add(pair.first);
                    }

                } break;

                case MESSAGE_TYPE::RENDERCOMPONENTSDIFF: {
                    Logger::log("Received RENDERCOMPONENTSDIFF packet", Log::INFO);

                    //Diff rendercomponents were received - handle it
                    auto typedPacket = socket.get<Packet<Components<RenderComponent>>>(bytesRead);
                    componentManager.renderComponents.sync(typedPacket.getData());

                    for(auto& pair : typedPacket.getData()) {
                        textureHashGridSystem.add(pair.first);
                        renderSystem.add(pair.first);
                    }
                } break;

                case MESSAGE_TYPE::PLAY_SOUND: {
                    Logger::log("Received PLAY_SOUND packet", Log::INFO);

                    auto typedPacket = socket.get<Packet<SoundData>>(bytesRead);
                    soundEngine.playSound(typedPacket.getData());
                } break;

                case MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM: {
                    Logger::log("Received REGISTER_ID_TO_SYSTEM packet", Log::INFO);

                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    systemManager.getSystem(systemIdentifier)->add(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID: {
                    Logger::log("Received REMOVE_ID packet", Log::INFO);

                    auto typedPacket = socket.get<Packet<ID>>(bytesRead);
                    const auto& id = typedPacket.getData();
                    systemManager.getSystem(System::RENDER)->remove(id);
                    systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
                    componentManager.clearComponents(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM: {
                    Logger::log("Received REMOVE_ID_FROM_SYSTEM packet", Log::INFO);

                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    systemManager.getSystem(systemIdentifier)->remove(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS: {
                    Logger::log("Received REMOVE_ID_FROM_SYSTEMS packet", Log::INFO);

                    auto typedPacket = socket.get<Packet<ID>>(bytesRead);
                    const auto& id = typedPacket.getData();
                    systemManager.getSystem(System::RENDER)->remove(id);
                    systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
                } break;

                case MESSAGE_TYPE::ACTIVATE_ID: {
                    Logger::log("Received ACTIVATE_ID packet", Log::INFO);

                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    systemManager.getSystem(systemIdentifier)->activateId(id);
                } break;

                case MESSAGE_TYPE::KEEP_ALIVE: {
                    keepAlive.start();
                } break;

                default: {
                    std::ostringstream oss;
                    oss << "Message without proper type received. This is probably a bug.";
                    oss << " Either client-side handling for that message isn't implemented";
                    oss << " or server sent a message with a bogus messagetype";
                    oss << " or the messagetype was wrongly altered somewhere (type: " << type << ")";
                    Logger::log(oss, Log::WARNING);
                };
            }

            /** END OF CRITICAL SECTION **/
            componentsMutex.unlock();
        }

        if(keepAlive.elapsed() > secondsUntilTimeout) {
            running = false;
            receiveThreadRunning = false;
            std::ostringstream oss;
            oss << "No packets from server received for " << keepAlive.elapsed() << "sec, server timeout";
            Logger::log(oss, Log::ERROR);
        }
    }
}

void Client::run() {
    while(running) {
        step();
    }
}

void Client::step() {
    deltaTime.start();

    /** Get input from client **/
    std::queue<int> presses;
    std::queue<int> releases;

    //Fetch all events that ocurred...
    while(SDL_PollEvent(&event) != 0) {
        //Dont consider keyrepeat as keypress
        if(event.key.repeat == 0) {
            //And take appropiate action!
            switch(event.type) {
                case SDL_QUIT: {
                    running = false;
                    break;
                }
                //If this event was a keydown...
                case SDL_KEYDOWN: {
                    presses.push(event.key.keysym.sym);
                } break;

                //If user released a key
                case SDL_KEYUP: {
                    releases.push(event.key.keysym.sym);

                } break;
            }
        }
    }

    //If user either pressed or released a key
    //then send keystrokes to server
    if( !(presses.empty() && releases.empty()) ) {
        send<InputData>({presses, releases}, MESSAGE_TYPE::INPUTDATA);
    }

    /** CRITICAL SECTION **/
    componentsMutex.lock();

    /** Update client-side ECS **/
    systemManager.update();

    /** END OF CRITICAL SECTION **/
    componentsMutex.unlock();

    //Limit client-speed to 60fps (rather 60 tick per second)
    //Check the elapsed time for the current step, if it is lower than
    //16.6666... then it's faster than 60fps so sleep until 16.666ms has passed
    using ms = std::chrono::milliseconds;
    using fdur = std::chrono::duration<float>;
    const float sleep_float = 1.0f/60 - deltaTime.elapsed();
    const ms sleep = std::chrono::duration_cast<ms>(fdur(sleep_float));
    std::this_thread::sleep_for(sleep);

    deltaTime.stop();
}

int main(int argc, char** argv) {
    Logger::level = Log::INFO;
    Client client(argc, argv);

    bool ipParameterSet = false;
    for(int i = 0; i < argc; i++) {
        std::string command(argv[i]);
        size_t pos = command.rfind("--ip=");
        if(pos != std::string::npos) {
            ipParameterSet = true;

            std::string ipstr = command.substr(pos+5);
            pos = ipstr.find_first_of(".");
            unsigned char a = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(".");
            unsigned char b = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(".");
            unsigned char c = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(".");
            unsigned char d = stoi(ipstr.substr(0, pos));

            ipstr = ipstr.substr(pos+1);
            pos = ipstr.find_first_of(":");
            unsigned short port = 47293;
            if(pos != std::string::npos) {
                port = stoi(ipstr.substr(pos+1));
            }

            client.connect({a, b, c, d, port});
        }
    }

    if(!ipParameterSet) {
        client.connect({127, 0, 0, 1, 47293});
    }

    client.run();
    client.disconnect();
}








//
