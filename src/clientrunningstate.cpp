#include "clientrunningstate.hpp"
#include "client.hpp"
#include "inputdata.hpp"

ClientRunningState::ClientRunningState(Client* client) : client(client) { }

void ClientRunningState::receive() {

        //Receive data from server...
        IpAddress server;
        MESSAGE_TYPE type;
        int bytesRead;
        client->socket.receive(server, type, bytesRead);

        //If any data was received, check its type and take appropiate action
        if(bytesRead > 0) {

            /** CRITICAL-SECTION **/
            client->componentsMutex.lock();

            switch(type) {
                case MESSAGE_TYPE::CONNECT: {
                    Logger::log("Got camera ID from server", Log::INFO);

                    //Got my id. Tell camerasystem to follow that id.
                    auto typedPacket = client->socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& pair = typedPacket.getData();
                    const auto& id = pair.first;
                    client->systemManager.getSystem(pair.second)->add(id);
                    client->playerId = id;
                } break;

				case MESSAGE_TYPE::MOVECOMPONENTSDIFF: {
                    Logger::log("Received MOVECOMPONENTSDIFF packet", Log::INFO);

                    //Diff movecomponents were received - handle it
                    auto typedPacket = client->socket.get<Packet<Components<MoveComponent>>>(bytesRead);
                    client->componentManager.moveComponents.sync(typedPacket.getData());

                    for(auto& pair : typedPacket.getData()) {
                        //Try to activate id, but it could be the case that
                        //the entity is newly created, in that case activation wont work
                        //so just add it instead. But before adding/activating, make sure
                        //both components exists

                        const auto& id = pair.first;
                        const auto& mcs = client->componentManager.moveComponents;
                        const auto& rcs = client->componentManager.renderComponents;
                        if(mcs.contains(id) && rcs.contains(id)) {
                            if(!client->textureHashGridSystem.activateId(id)) {
                                client->textureHashGridSystem.add(id);
                            }

                            if(!client->renderSystem.activateId(id)) {
                                client->renderSystem.add(id);
                            }
                        }
                    }

                } break;

                case MESSAGE_TYPE::RENDERCOMPONENTSDIFF: {
                    Logger::log("Received RENDERCOMPONENTSDIFF packet", Log::INFO);

                    //Diff rendercomponents were received - handle it
                    auto typedPacket = client->socket.get<Packet<Components<RenderComponent>>>(bytesRead);
                    client->componentManager.renderComponents.sync(typedPacket.getData());

                    for(auto& pair : typedPacket.getData()) {
                        const auto& id = pair.first;
                        const auto& mcs = client->componentManager.moveComponents;
                        const auto& rcs = client->componentManager.renderComponents;
                        if(mcs.contains(id) && rcs.contains(id)) {
                            if(!client->textureHashGridSystem.activateId(id)) {
                                client->textureHashGridSystem.add(id);
                            }

                            if(!client->renderSystem.activateId(id)) {
                                client->renderSystem.add(id);
                            }
                        }
                    }
                } break;

                case MESSAGE_TYPE::PLAY_SOUND: {
                    Logger::log("Received PLAY_SOUND packet", Log::INFO);

                    auto typedPacket = client->socket.get<Packet<SoundData>>(bytesRead);
                    client->soundEngine.playSound(typedPacket.getData());
                } break;

                case MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM: {
                    Logger::log("Received REGISTER_ID_TO_SYSTEM packet", Log::INFO);

                    auto typedPacket = client->socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    client->systemManager.getSystem(systemIdentifier)->add(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID: {
                    Logger::log("Received REMOVE_ID packet", Log::INFO);

                    auto typedPacket = client->socket.get<Packet<ID>>(bytesRead);
                    const auto& id = typedPacket.getData();
                    client->systemManager.getSystem(System::RENDER)->remove(id);
                    client->systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
                    client->componentManager.clearComponents(id);

                    //Check if entity controlled by this client were
                    //the one being removed. If so, take special actions..
                    if(id == client->playerId) {
                        client->cameraSystem.remove(client->playerId);
                    }

                } break;

                case MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM: {
                    Logger::log("Received REMOVE_ID_FROM_SYSTEM packet", Log::INFO);

                    auto typedPacket = client->socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    client->systemManager.getSystem(systemIdentifier)->remove(id);
                } break;

                case MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS: {
                    Logger::log("Received REMOVE_ID_FROM_SYSTEMS packet", Log::INFO);

                    auto typedPacket = client->socket.get<Packet<ID>>(bytesRead);
                    const auto& id = typedPacket.getData();
                    client->systemManager.getSystem(System::RENDER)->remove(id);
                    client->systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
                } break;

                case MESSAGE_TYPE::ACTIVATE_ID: {
                    Logger::log("Received ACTIVATE_ID packet", Log::INFO);

                    auto typedPacket = client->socket.get<Packet<std::pair<ID, System::Identifier>>>(bytesRead);
                    const auto& id = typedPacket.getData().first;
                    const auto& systemIdentifier = typedPacket.getData().second;
                    client->systemManager.getSystem(systemIdentifier)->activateId(id);
                } break;

                case MESSAGE_TYPE::KEEP_ALIVE: {
                    client->keepAlive.start();
                } break;
            }

            /** END OF CRITICAL-SECTION **/
            client->componentsMutex.unlock();
        }
}

void ClientRunningState::step() {
	client->deltaTime.start();

    /** Get input from client **/
    std::queue<int> presses;
    std::queue<int> releases;

    //Fetch all events that ocurred...
    while(SDL_PollEvent(&client->event) != 0) {
        //Dont consider keyrepeat as keypress
        if(client->event.key.repeat == 0) {
            //And take appropiate action!
            switch(client->event.type) {
                case SDL_QUIT: {
                    client->running = false;
                    break;
                }
                //If this event was a keydown...
                case SDL_KEYDOWN: {
                    presses.push(client->event.key.keysym.sym);
                } break;

                //If user released a key
                case SDL_KEYUP: {
                    releases.push(client->event.key.keysym.sym);

                } break;
            }
        }
    }

    //If user either pressed or released a key
    //then send keystrokes to server
    if( !(presses.empty() && releases.empty()) ) {
        client->send<InputData>({presses, releases}, MESSAGE_TYPE::INPUTDATA);
    }

    /** CRITICAL SECTION **/
    client->componentsMutex.lock();

    /** Update client-side ECS **/
    client->systemManager.update();

    /** END OF CRITICAL SECTION **/
    client->componentsMutex.unlock();

    //Limit client-speed to 60fps (rather 60 tick per second)
    //Check the elapsed time for the current step, if it is lower than
    //16.6666... then it's faster than 60fps so sleep until 16.666ms has passed
    using ms = std::chrono::milliseconds;
    using fdur = std::chrono::duration<float>;
    const float sleep_float = 1.0f/60 - client->deltaTime.elapsed();
    const ms sleep = std::chrono::duration_cast<ms>(fdur(sleep_float));
    std::this_thread::sleep_for(sleep);

    client->deltaTime.stop();
}