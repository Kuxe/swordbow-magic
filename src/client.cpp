#include "client.hpp"
#include "server.hpp"
#include <iostream>
#include <SDL2/SDL.h>
#include "messagetypes.hpp"
#include "packet.hpp"
#include "systemidentifiers.hpp"

Client::Client(int argc, char** argv) :
        socket("swordbow-magic"),
        sequence(1),
        renderer(argc, argv),
        systemManager(&componentManager, &deltaTime),
        textureBoundingBox(&componentManager, &renderer),
        textureHashGridSystem(&componentManager, &textureBoundingBox),
        renderSystem(&renderer),
        cameraSystem(&renderer) {

    cout << "\n--** STARTING CLIENT **--" << endl;

    constexpr short port = 47294;
    socket.open(port);

    systemManager.add(&textureHashGridSystem);
    systemManager.add(&renderSystem);
    systemManager.add(&cameraSystem);

    //Play some sweet music
	soundEngine.playMusic(Music::NATURE_SOUNDS);
}

Client::~Client() {
    socket.close();
}

void Client::connect(const IpAddress& server) {

    //TODO: The server should inform the client of the servers current
    //sequence number, and the client should set it here with some
    //socket.setRemoteSequenceNumber(server)-call. Otherwise clients
    //that connects to a server might figure some packets to be old

    //Start a thread that checks socket for any recieved data
    receiveThreadRunning = true;
    receiveThread = std::thread(&Client::receive, this);

    this->server = server;

    //Connect-request packet
    using Type = Packet<bool>;
    const Type packet {
        stringhash("swordbow-magic"),
        sequence++,
        MESSAGE_TYPE::CONNECT,
        true,
        sizeof(sizeof(bool))
    };

    socket.send<Type>(server, packet);
}

void Client::disconnect() {

    //Stop receiving data on socket
    receiveThreadRunning = false;
    receiveThread.join();

    //Disconnect-request packet
    using Type = Packet<bool>;
    const Type packet {
        stringhash("swordbow-magic"),
        sequence++,
        MESSAGE_TYPE::DISCONNECT,
        true,
        sizeof(sizeof(bool))
    };

    socket.send<Type>(server, packet);

    server = IpAddress(0, 0, 0, 0, 0); //Indicate not connected to any server
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
                    std::cout << "This packet is outdated, to late! Sluggish!" << endl;
                } break;

                case MESSAGE_TYPE::CONNECT: {
                    //Got my id. Tell camerasystem to follow that id.
                    auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& pair = typedPacket.getData();
                    systemManager.getSystem(pair.second)->add(pair.first);
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
                    std::cout << "WARNING: Message without proper type received. This is probably a bug." << std::endl;
                    std::cout << "Either client-side handling for that message isn't implemented";
                    std::cout << " or server sent a message with a bogus messagetype";
                    std::cout << " or the messagetype was wrongly altered somewhere" << std::endl;
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
    queue<int> presses;
    queue<int> releases;

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
    if( !(presses.empty() && releases.empty()) ) {

        //Assemble the InputData that should be sent into a packet
        const InputData inputData{presses, releases};

        using Type = Packet<InputData>;
        const Type packet {
            stringhash("swordbow-magic"),
            sequence++,
            MESSAGE_TYPE::INPUTDATA,
            inputData,
            sizeof(inputData)
        };

        //Send that packet to 127.0.0.1:47293
        constexpr unsigned short SERVER_PORT = 47293;
        socket.send<Type>({127, 0, 0, 1, SERVER_PORT}, packet);
    }

    /** CRITICAL SECTION **/
    componentsMutex.lock();

    /** Update client-side ECS **/
    systemManager.update();

    /** END OF CRITICAL SECTION **/
    componentsMutex.unlock();

    /** Pass output from rendersystem to renderer **/
    renderer.render(renderSystem.getDrawPriorityQueue(), cameraSystem.getCamera());

    deltaTime.stop();
}

int main(int argc, char** argv) {
    Client client(argc, argv);
    client.connect({127, 0, 0, 1, 47293});
    client.run();
    client.disconnect();
}








//
