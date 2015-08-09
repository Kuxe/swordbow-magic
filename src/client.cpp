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
}

void Client::disconnect() {

    //Stop receiving data on socket
    receiveThreadRunning = false;
    receiveThread.join();

    //Disconnect-request packet
    send<bool>(true, MESSAGE_TYPE::DISCONNECT);

    //Indicate not connected to any server
    server = IpAddress(0, 0, 0, 0, 0);
}

void Client::receive() {
    while(receiveThreadRunning) {

        //Receive data from server...
        IpAddress server;
        unsigned char type;
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
                    disconnect();
                } break;

                case MESSAGE_TYPE::INITIAL_COMPONENTS: {
                    using DataType = std::pair<Components<MoveComponent>, Components<RenderComponent>>;
                    using PacketType = Packet<DataType>;
                    auto typedPacket = socket.get<PacketType>(bytesRead);

                    for(auto& pair : typedPacket.getData().first) {
                        textureHashGridSystem.add(pair.first);
                        renderSystem.add(pair.first);
                    }
                } break;

                case MESSAGE_TYPE::MOVECOMPONENTS: {
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
                    //All rendercomponents were received - handle it
                    auto typedPacket = socket.get<Packet<Components<RenderComponent>>>(bytesRead);
                    componentManager.renderComponents.sync(typedPacket.getData());

                    for(auto& pair : typedPacket.getData()) {
                        textureHashGridSystem.add(pair.first);
                        renderSystem.add(pair.first);
                    }

                } break;

                case MESSAGE_TYPE::MOVECOMPONENTSDIFF: {
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
                    //Diff rendercomponents were received - handle it
                    auto typedPacket = socket.get<Packet<Components<RenderComponent>>>(bytesRead);
                    componentManager.renderComponents.sync(typedPacket.getData());

                    for(auto& pair : typedPacket.getData()) {
                        textureHashGridSystem.add(pair.first);
                        renderSystem.add(pair.first);
                    }
                } break;

                case MESSAGE_TYPE::PLAY_SOUND: {
                    auto typedPacket = socket.get<Packet<SoundData>>(bytesRead);
                    soundEngine.playSound(typedPacket.getData());
                } break;

                case MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM: {
                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    systemManager.getSystem(systemIdentifier)->add(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID: {
                    auto typedPacket = socket.get<Packet<ID>>(bytesRead);
                    const auto& id = typedPacket.getData();
                    systemManager.getSystem(System::RENDER)->remove(id);
                    systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
                    componentManager.clearComponents(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM: {
                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    systemManager.getSystem(systemIdentifier)->remove(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS: {
                    auto typedPacket = socket.get<Packet<ID>>(bytesRead);
                    const auto& id = typedPacket.getData();
                    systemManager.getSystem(System::RENDER)->remove(id);
                    systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
                } break;

                case MESSAGE_TYPE::ACTIVATE_ID: {
                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    systemManager.getSystem(systemIdentifier)->activateId(id);
                } break;

                default: {
                    const std::string logstr(
                        std::string("Message without proper type received. This is probably a bug.\n") +
                        std::string("Either client-side handling for that message isn't implemented") +
                        std::string(" or server sent a message with a bogus messagetype") +
                        std::string(" or the messagetype was wrongly altered somewhere\n")
                    );
                    Logger::log(logstr, Log::WARNING);
                };
            }

            /** END OF CRITICAL SECTION **/
            componentsMutex.unlock();
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

    deltaTime.stop();
}

int main(int argc, char** argv) {
    Logger::level = Log::INFO;
    Client client(argc, argv);
    client.connect({127, 0, 0, 1, 47293});
    client.run();
    client.disconnect();
}








//
