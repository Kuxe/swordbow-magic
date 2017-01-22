#include "clientdisconnectedstate.hpp"
#include "client.hpp"
#include "irenderer.hpp"
#include "isoundengine.hpp"

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
    Logger::verbose("ClientDisconnectedState locking componentsMutex in receive...");
    client->componentsMutex.lock();
    Logger::verbose("ClientDisconnectedState locked componentsMutex in receive");
    client->packetManager.receive<ClientDisconnectedState>(*this);
    Logger::verbose("ClientDisconnectedState unlocking componentsMutex in receive...");
    client->componentsMutex.unlock();
    Logger::verbose("ClientDisconnectedState unlocked componentsMutex in receive");
}

void ClientDisconnectedState::step() {
    //Fetch all events that ocurred...
    client->renderer->pollEvents(this);
    client->renderer->renderOnlyOverlays();

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
    Logger::log("Client changing state from ClientDisconnectedState to ClientDisconnectedState", Logger::WARNING);
    client->clientState = this;
    client->renderer->showOverlay(Image::CONNECT_OVERLAY, {"Disconnected from server", glm::ivec2{150, client->renderer->getWindowResolution().y / 2 - 10}, glm::vec3(1.0)});
}

void ClientDisconnectedState::onChange(ClientReceiveInitialState* state) {
    Logger::log("Client changing state from ClientReceiveInitialState to ClientDisconnectedState", Logger::INFO);
    client->clientState = this;
    client->renderer->hideOverlay(Image::RECEIVING_DATA_OVERLAY);
    client->renderer->showOverlay(Image::CONNECT_OVERLAY, {"Disconnected from server", glm::ivec2{150, client->renderer->getWindowResolution().y / 2 - 10}, glm::vec3(1.0)});
    forceDisconnect();
}

void ClientDisconnectedState::onChange(ClientRunningState* state) {
    Logger::log("Client changing state from ClientRunningState to ClientDisconnectedState", Logger::WARNING);
    client->clientState = this;
    client->soundEngine->stopMusic(Music::NATURE_SOUNDS);
    client->renderer->showOverlay(Image::CONNECT_OVERLAY, {"Disconnected from server", glm::ivec2{150, client->renderer->getWindowResolution().y / 2 - 10}, glm::vec3(1.0)});
    forceDisconnect();
}

void ClientDisconnectedState::accept(const OutdatedData& data, const IpAddress& sender) {
    Logger::log("This packet is outdated, to late! Sluggish!", Logger::WARNING);
}

void ClientDisconnectedState::accept(const DisconnectData& data, const IpAddress& sender) {
    //Server will send this to client for a couple of reasons.
    //In any case the client shouldn't be connected to the server.
    Logger::log("Received DISCONNECT packet", Logger::INFO);
    client->disconnect();
}

void ClientDisconnectedState::accept(const BeginTransmittingInitialComponentsData& data, const IpAddress& sender) {
    //This message to client will put the client in a special "state"
    //where the client will only receive INITIAL_COMPONENTS-packet
    //and END_TRANSMITTING_INITIAL_COMPONENTS. ECS won't update.
    //This is because large worlds must be split into smaller packets,
    //due to packet limitation size, and then sent separately to client.
    //Client may NOT update ECS because it's not guaranteed that the all
    //required entity-components have been received.
    Logger::log("Received BEGIN_TRANSMITTING_INITIAL_COMPONENTS packet", Logger::INFO);
    auto numberOfInitialSmallContainers = data.data;
    client->numberOfInitialSmallContainers = numberOfInitialSmallContainers;

    //As per (unwritten) specification, INITIAL_COMPONENTS-packets are directly followed by the
    //BEGIN_TRANSMITTING_INITIAL_COMPONENTS-packet. So it is safe to say that the sequenceNumbers
    //for all INITIAL_COMPONENTS-packets are:
    //BEGIN_TRANSMITTING_INITIAL_COMPONENTS.sequence ... BEGIN_TRANSMITTING_INITIAL_COMPONENTS.sequence + numberOfInitialSmallContainers
    //If the BEGIN_TRANSMITTING_INITIAL_COMPONENTS had sequence 3 and there are 7 small containers,
    //then 4, 5, 6 ..., 10 is expected to be the sequence number of initial components
    /*const auto firstSequence = packet.getSequence()+1;
    const auto lastSequence = packet.getSequence()+1 + numberOfInitialSmallContainers;
    for(auto sequence = firstSequence; sequence <= lastSequence; sequence++) {
        client->missingSequences.insert(sequence);
    }*/

    changeState(&client->clientReceiveInitialState);
}

void ClientDisconnectedState::accept(const KeepAliveData&, const IpAddress& sender) {
    client->keepAlive.start();
}

void ClientDisconnectedState::accept(const auto& data, const IpAddress& sender) {
    Logger::log("Received packet that has no overloaded accept (ClientDisconnectedState)", Logger::WARNING);
}




