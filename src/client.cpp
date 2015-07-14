#include "client.hpp"
#include "server.hpp"
#include <iostream>
#include <SDL2/SDL.h>
#include "messagetypes.hpp"
#include "packet.hpp"
#include "systemidentifiers.hpp"

Client::Client(int argc, char** argv) :
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

    this->server = server;

    //Connect-request packet
    using Type = Packet<bool>;
    const Type packet {
        stringhash("swordbow-magic"),
        MESSAGE_TYPE::CONNECT,
        true,
        sizeof(sizeof(bool))
    };

    socket.send<Type>(server, packet);
}

void Client::disconnect() {
    //Disconnect-request packet
    using Type = Packet<bool>;
    const Type packet {
        stringhash("swordbow-magic"),
        MESSAGE_TYPE::DISCONNECT,
        true,
        sizeof(sizeof(bool))
    };

    socket.send<Type>(server, packet);

    server = IpAddress(0, 0, 0, 0, 0); //Indicate not connected to any server
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

    //If user either pressed or released a key
    if( !(presses.empty() && releases.empty()) ) {

        //Assemble the InputData that should be sent into a packet
        const InputData inputData{presses, releases};

        using Type = Packet<InputData>;
        const Type packet {
            stringhash("swordbow-magic"),
            MESSAGE_TYPE::INPUTDATA,
            inputData,
            sizeof(inputData)
        };

        //Send that packet to 127.0.0.1:47293
        constexpr unsigned short SERVER_PORT = 47293;
        socket.send<Type>({127, 0, 0, 1, SERVER_PORT}, packet);
    }

    //Receive data from server...
    IpAddress server;
    unsigned char type;
    int bytesRead;
    socket.receive(server, type, bytesRead);

    //If any data was received, check its type and take appropiate action
    if(bytesRead > 0) {
        switch(type) {
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
            } break;

            case MESSAGE_TYPE::RENDERCOMPONENTS: {
                //All rendercomponents were received - handle it
                auto typedPacket = socket.get<Packet<Components<RenderComponent>>>(bytesRead);
                componentManager.renderComponents.sync(typedPacket.getData());
            } break;

            case MESSAGE_TYPE::MOVECOMPONENTSDIFF: {
                //Diff movecomponents were received - handle it
                auto typedPacket = socket.get<Packet<Components<MoveComponent>>>(bytesRead);
                componentManager.moveComponents.sync(typedPacket.getData());
            } break;

            case MESSAGE_TYPE::RENDERCOMPONENTSDIFF: {
                //Diff rendercomponents were received - handle it
                auto typedPacket = socket.get<Packet<Components<RenderComponent>>>(bytesRead);
                componentManager.renderComponents.sync(typedPacket.getData());
            } break;

            case MESSAGE_TYPE::PLAY_SOUND: {
                auto typedPacket = socket.get<Packet<SoundComponent::SoundData>>(bytesRead);
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
            }

            case MESSAGE_TYPE::ACTIVATE_ID: {
                auto typedPacket = socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                const auto& id = typedPacket.getData().first;
                const auto& systemIdentifier = typedPacket.getData().second;
                systemManager.getSystem(systemIdentifier)->activateId(id);
            }
        }
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
