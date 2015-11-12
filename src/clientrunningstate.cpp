#include "clientrunningstate.hpp"
#include "client.hpp"
#include "inputdata.hpp"
#include "timer.hpp"

ClientRunningState::ClientRunningState(Client* client) : client(client) { }

void ClientRunningState::receive() {

        //Receive data from server...
        Timer waitTimer;
        waitTimer.start();

        client->componentsMutex.lock();

        const auto elapsed = waitTimer.elapsed();
        if(elapsed > 1.0f/50.0f) {
            std::ostringstream oss;
            oss << "Receive-thread waited for " << elapsed << "s on componentsMutex. This is a probable cause of stuttering.";
            Logger::log(oss, Log::WARNING);
         }

        client->packetManager.receive<ClientRunningState>(*this);
        client->componentsMutex.unlock();
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
        client->send<InputData, MESSAGE_TYPE::INPUTDATA>({presses, releases});
    }

    /** CRITICAL-SECTION **/
    Timer waitTimer;
    waitTimer.start();
    client->componentsMutex.lock();
    const auto elapsed = waitTimer.elapsed();
    if(elapsed > 1.0f/50.0f) {
        std::ostringstream oss;
        oss << "Main-thread waited for " << elapsed << "s on componentsMutex. This is a probable cause of stuttering.";
        Logger::log(oss, Log::WARNING);
    }

    /** Update client-side ECS **/
    Timer updateTimer;
    updateTimer.start();
    client->systemManager.update();
    const auto updateElapsed = updateTimer.elapsed();
    if(updateElapsed > 1.0f/50.0f) {
        std::ostringstream oss;
        oss << "Updating systems took " << updateElapsed << "s. This is a probable cause of stuttering.";
        Logger::log(oss, Log::WARNING);
    }

    /** Check for timeout **/
    if(client->keepAlive.elapsed() > client->secondsUntilTimeout) {
        std::ostringstream oss;
        oss << "No packets from server received for " << client->keepAlive.elapsed() << "sec, server timeout";
        Logger::log(oss, Log::ERROR);
        changeState(&client->clientDisconnectedState);
    }

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

void ClientRunningState::changeState(IClientState* state) {
    state->onChange(this);
}

void ClientRunningState::onChange(ClientDisconnectedState* state) {
    Logger::log("Client can't change state from ClientDisconnectedState to ClientRunningState", Log::WARNING);
}

void ClientRunningState::onChange(ClientReceiveInitialState* state) {
    Logger::log("Client changing state from ClientReceiveInitialState to ClientRunningState", Log::INFO);
    client->soundEngine.playMusic(Music::NATURE_SOUNDS);
    client->clientState = this;
    client->renderer.fadeOutOverlay(Image::RECEIVING_DATA_OVERLAY, 0.5f);
}

void ClientRunningState::onChange(ClientRunningState* state) {
    Logger::log("Client can't change state from ClientRunningState to ClientRunningState", Log::WARNING);
}

void ClientRunningState::accept(Packet<OUTDATED_TYPE, MESSAGE_TYPE::OUTDATED>& packet, const IpAddress& sender) {
    Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
}

void ClientRunningState::accept(Packet<CONNECT_TYPE, MESSAGE_TYPE::CONNECT>& packet, const IpAddress& sender) {
    Logger::log("Got camera ID from server", Log::INFO);

    //Got my id. Tell camerasystem to follow that id.t
    const auto& pair = packet.getData();
    const auto& id = pair.first;
    client->systemManager.getSystem(pair.second)->add(id);
    client->playerId = id;
}

void ClientRunningState::accept(Packet<DISCONNECT_TYPE, MESSAGE_TYPE::DISCONNECT>& packet, const IpAddress& sender) {

}

void ClientRunningState::accept(Packet<INPUTDATA_TYPE, MESSAGE_TYPE::INPUTDATA>& packet, const IpAddress& sender) {

}

void ClientRunningState::accept(Packet<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {

}

void ClientRunningState::accept(Packet<INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::INITIAL_COMPONENTS>& packet, const IpAddress& sender) {

}

void ClientRunningState::accept(Packet<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {

}

void ClientRunningState::accept(Packet<MOVECOMPONENTSDIFF_TYPE, MESSAGE_TYPE::MOVECOMPONENTSDIFF>& packet, const IpAddress& sender) {
    Logger::log("Received MOVECOMPONENTSDIFF packet", Log::VERBOSE);

    //Diff movecomponents were received - handle it
    client->componentManager.moveComponents.sync(packet.getData());

    for(auto& pair : packet.getData()) {
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
}

void ClientRunningState::accept(Packet<RENDERCOMPONENTSDIFF_TYPE, MESSAGE_TYPE::RENDERCOMPONENTSDIFF>& packet, const IpAddress& sender) {
    Logger::log("Received RENDERCOMPONENTSDIFF packet", Log::VERBOSE);

    //Diff rendercomponents were received - handle it
    client->componentManager.renderComponents.sync(packet.getData());

    for(auto& pair : packet.getData()) {
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
}

void ClientRunningState::accept(Packet<PLAY_SOUND_TYPE, MESSAGE_TYPE::PLAY_SOUND>& packet, const IpAddress& sender) {
    Logger::log("Received PLAY_SOUND packet", Log::VERBOSE);
    client->soundEngine.playSound(packet.getData());
}

void ClientRunningState::accept(Packet<REGISTER_ID_TO_SYSTEM_TYPE, MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>& packet, const IpAddress& sender) {
    Logger::log("Received REGISTER_ID_TO_SYSTEM packet", Log::INFO);
    const auto& id = packet.getData().first;
    const auto& systemIdentifier = packet.getData().second;
    client->systemManager.getSystem(systemIdentifier)->add(id);
}

void ClientRunningState::accept(Packet<REMOVE_ID_TYPE, MESSAGE_TYPE::REMOVE_ID>& packet, const IpAddress& sender) {
    Logger::log("Received REMOVE_ID packet", Log::INFO);

    const auto& id = packet.getData();
    client->systemManager.getSystem(System::RENDER)->remove(id);
    client->systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
    client->componentManager.clearComponents(id);

    //Check if entity controlled by this client were
    //the one being removed. If so, take special actions..
    if(id == client->playerId) {
        client->cameraSystem.remove(client->playerId);
    }
}

void ClientRunningState::accept(Packet<REMOVE_ID_FROM_SYSTEM_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>& packet, const IpAddress& sender) {
    Logger::log("Received REMOVE_ID_FROM_SYSTEM packet", Log::INFO);
    const auto& id = packet.getData().first;
    const auto& systemIdentifier = packet.getData().second;
    client->systemManager.getSystem(systemIdentifier)->remove(id);
}

void ClientRunningState::accept(Packet<REMOVE_ID_FROM_SYSTEMS_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>& packet, const IpAddress& sender) {
    Logger::log("Received REMOVE_ID_FROM_SYSTEMS packet", Log::INFO);
    const auto& id = packet.getData();
    client->systemManager.getSystem(System::RENDER)->remove(id);
    client->systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
}

void ClientRunningState::accept(Packet<ACTIVATE_ID_TYPE, MESSAGE_TYPE::ACTIVATE_ID>& packet, const IpAddress& sender) {
    Logger::log("Received ACTIVATE_ID packet", Log::INFO);
    const auto& id = packet.getData().first;
    const auto& systemIdentifier = packet.getData().second;
    client->systemManager.getSystem(systemIdentifier)->activateId(id);
}

void ClientRunningState::accept(Packet<CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::CONGESTED_CLIENT>& packet, const IpAddress& sender) {

}

void ClientRunningState::accept(Packet<NOT_CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::NOT_CONGESTED_CLIENT>& packet, const IpAddress& sender) {

}

void ClientRunningState::accept(Packet<KEEP_ALIVE_TYPE, MESSAGE_TYPE::KEEP_ALIVE>& packet, const IpAddress& sender) {
    client->keepAlive.start();
}
