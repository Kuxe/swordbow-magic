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

void ClientDisconnectedState::step() {
    //Fetch all events that ocurred...
    client->renderer->pollEvents(this);
    client->renderer->renderOnlyOverlays();

    //Poll packets (internally applied packets onto *this)
    client->packetManager.poll(*this);

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
    std::ostringstream oss;
    oss << "Expecting " << numberOfInitialSmallContainers << " INITIAL_COMPONENTS-packets";
    Logger::info(oss);

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
    /** FIXME: Some packets are applied onto disconnected state albeit a state change already occurred
        this is because we are polling packets in a while-loop and applying packets onto *this instead
        of clientState pointer. So when the state changes, poll() will keep applying packets to *this
        instead of to clientState.

        Two potential solutions:

            1. call client->packetManager.poll(client->clientState) instead of call client->packetManager.poll(*this)
            but then the auto-acceptor stuff break because interface needs to have all accept function virtual which
            scales linearly with number of packets (times number of states, ugh). I could probably use visitor-pattern
            to solve this but I am not sure.

            2. Abort polling if change-state packet occured. This is tremendously ugly.

            3. Designate packets for different states. This is not very maintainable at all **/

    Logger::log("Received packet that has no overloaded accept (ClientDisconnectedState)", Logger::WARNING);
}

void ClientDisconnectedState::onEvent(const KeyEvent& evt) {

}

void ClientDisconnectedState::onEvent(const MouseEvent& evt) {
    
}


