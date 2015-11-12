#include "clientdisconnectedstate.hpp"
#include "client.hpp"

void ClientDisconnectedState::forceDisconnect() {
    forceDisconnectMutex.lock();
    forceDisconnectBool = true;
    forceDisconnectMutex.unlock();
}

void ClientDisconnectedState::pollForceDisconnect() {
    forceDisconnectMutex.lock();
    if(forceDisconnectBool) {
        client->disconnect();
        forceDisconnectBool = false;
    }
    forceDisconnectMutex.unlock();
}

ClientDisconnectedState::ClientDisconnectedState(Client* client) : client(client) { }

void ClientDisconnectedState::receive() {
        client->componentsMutex.lock();
        client->packetManager.receive<ClientDisconnectedState>(*this);
        client->componentsMutex.unlock();
}

void ClientDisconnectedState::step() {
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
            }
        }
    }
    client->renderer.renderOnlyOverlays();

    //Receivethread can force client to disconnect (from state-transitions usually)
    //In that case, this thread (main-thread) must poll that variable to see if it is set
    pollForceDisconnect();

    //If it weren't for 10ms sleep, this thread would lock out
    //the receive-thread to the extent of losing lots of packets
    using namespace std::literals;
    std::this_thread::sleep_for(10ms);
}

void ClientDisconnectedState::changeState(IClientState* state) {
    state->onChange(this);
}

void ClientDisconnectedState::onChange(ClientDisconnectedState* state) {
    Logger::log("Client changing state from ClientDisconnectedState to ClientDisconnectedState", Log::WARNING);
    client->clientState = this;
    client->renderer.showOverlay(Image::CONNECT_OVERLAY, {"Disconnected from server", 150, client->renderer.getScreenHeight() / 2 - 10});
}

void ClientDisconnectedState::onChange(ClientReceiveInitialState* state) {
    Logger::log("Client changing state from ClientReceiveInitialState to ClientDisconnectedState", Log::INFO);
    client->clientState = this;
    client->renderer.hideOverlay(Image::RECEIVING_DATA_OVERLAY);
    client->renderer.showOverlay(Image::CONNECT_OVERLAY, {"Disconnected from server", 150, client->renderer.getScreenHeight() / 2 - 10});
    forceDisconnect();
}

void ClientDisconnectedState::onChange(ClientRunningState* state) {
    Logger::log("Client changing state from ClientRunningState to ClientDisconnectedState", Log::WARNING);
    client->clientState = this;
    client->soundEngine.stopMusic(Music::NATURE_SOUNDS);
    client->renderer.showOverlay(Image::CONNECT_OVERLAY, {"Disconnected from server", 150, client->renderer.getScreenHeight() / 2 - 10});
    forceDisconnect();
}

void ClientDisconnectedState::accept(Packet<OUTDATED_TYPE, MESSAGE_TYPE::OUTDATED>& packet, const IpAddress& sender) {
    Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<CONNECT_TYPE, MESSAGE_TYPE::CONNECT>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::CONNECT) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<DISCONNECT_TYPE, MESSAGE_TYPE::DISCONNECT>& packet, const IpAddress& sender) {
    //Server will send this to client for a couple of reasons.
    //In any case the client shouldn't be connected to the server.
    Logger::log("Received DISCONNECT packet", Log::INFO);
    client->disconnect();
}

void ClientDisconnectedState::accept(Packet<INPUTDATA_TYPE, MESSAGE_TYPE::INPUTDATA>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::INPUTDATA) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {
    //This message to client will put the client in a special "state"
    //where the client will only receive INITIAL_COMPONENTS-packet
    //and END_TRANSMITTING_INITIAL_COMPONENTS. ECS won't update.
    //This is because large worlds must be split into smaller packets,
    //due to packet limitation size, and then sent separately to client.
    //Client may NOT update ECS because it's not guaranteed that the all
    //required entity-components have been received.
    Logger::log("Received BEGIN_TRANSMITTING_INITIAL_COMPONENTS packet", Log::INFO);
    auto numberOfInitialSmallContainers = packet.getData();
    client->numberOfInitialSmallContainers = numberOfInitialSmallContainers;

    //As per (unwritten) specification, INITIAL_COMPONENTS-packets are directly followed by the
    //BEGIN_TRANSMITTING_INITIAL_COMPONENTS-packet. So it is safe to say that the sequenceNumbers
    //for all INITIAL_COMPONENTS-packets are:
    //BEGIN_TRANSMITTING_INITIAL_COMPONENTS.sequence ... BEGIN_TRANSMITTING_INITIAL_COMPONENTS.sequence + numberOfInitialSmallContainers
    //If the BEGIN_TRANSMITTING_INITIAL_COMPONENTS had sequence 3 and there are 7 small containers,
    //then 4, 5, 6 ..., 10 is expected to be the sequence number of initial components
    const auto firstSequence = packet.getSequence()+1;
    const auto lastSequence = packet.getSequence()+1 + numberOfInitialSmallContainers;
    for(auto sequence = firstSequence; sequence <= lastSequence; sequence++) {
        client->missingSequences.insert(sequence);
    }

    changeState(&client->clientReceiveInitialState);
}

void ClientDisconnectedState::accept(Packet<INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::INITIAL_COMPONENTS>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::INITIAL_COMPONENTS) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<MOVECOMPONENTSDIFF_TYPE, MESSAGE_TYPE::MOVECOMPONENTSDIFF>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::MOVECOMPONENTSDIFF) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<RENDERCOMPONENTSDIFF_TYPE, MESSAGE_TYPE::RENDERCOMPONENTSDIFF>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::RENDERCOMPONENTSDIFF) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<PLAY_SOUND_TYPE, MESSAGE_TYPE::PLAY_SOUND>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::PLAY_SOUND) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<REGISTER_ID_TO_SYSTEM_TYPE, MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<REMOVE_ID_TYPE, MESSAGE_TYPE::REMOVE_ID>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::REMOVE_ID) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<REMOVE_ID_FROM_SYSTEM_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<REMOVE_ID_FROM_SYSTEMS_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<ACTIVATE_ID_TYPE, MESSAGE_TYPE::ACTIVATE_ID>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::ACTIVATE_ID) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::CONGESTED_CLIENT>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::CONGESTED_CLIENT) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<NOT_CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::NOT_CONGESTED_CLIENT>& packet, const IpAddress& sender) {
    Logger::log("Received (unwanted) packet when in disconnected-state (type: " + std::to_string(MESSAGE_TYPE::NOT_CONGESTED_CLIENT) + ")", Log::WARNING);
}

void ClientDisconnectedState::accept(Packet<KEEP_ALIVE_TYPE, MESSAGE_TYPE::KEEP_ALIVE>& packet, const IpAddress& sender) {
    client->keepAlive.start();
}