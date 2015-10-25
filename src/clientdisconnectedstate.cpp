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
                case MESSAGE_TYPE::OUTDATED: {
                    Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
                } break;

                //Server will send this to client for a couple of reasons.
                //In any case the client shouldn't be connected to the server.
                case MESSAGE_TYPE::DISCONNECT: {
                    Logger::log("Received DISCONNECT packet", Log::INFO);
                    client->disconnect();
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
                    using DataType = int;
                    using PacketType = Packet<DataType>;
                    auto typedPacket = client->socket.get<PacketType>(bytesRead);
                    auto numberOfInitialSmallContainers = typedPacket.getData();
                    client->numberOfInitialSmallContainers = numberOfInitialSmallContainers;

                    //As per (unwritten) specification, INITIAL_COMPONENTS-packets are directly followed by the
                    //BEGIN_TRANSMITTING_INITIAL_COMPONENTS-packet. So it is safe to say that the sequenceNumbers
                    //for all INITIAL_COMPONENTS-packets are:
                    //BEGIN_TRANSMITTING_INITIAL_COMPONENTS.sequence ... BEGIN_TRANSMITTING_INITIAL_COMPONENTS.sequence + numberOfInitialSmallContainers
                    //If the BEGIN_TRANSMITTING_INITIAL_COMPONENTS had sequence 3 and there are 7 small containers,
                    //then 4, 5, 6 ..., 10 is expected to be the sequence number of initial components
                    const auto firstSequence = typedPacket.getSequence()+1;
                    const auto lastSequence = typedPacket.getSequence()+1 + numberOfInitialSmallContainers;
                    for(auto sequence = firstSequence; sequence <= lastSequence; sequence++) {
                        client->missingSequences.insert(sequence);
                    }

                    changeState(&client->clientReceiveInitialState);
                } break;

                case MESSAGE_TYPE::KEEP_ALIVE: {
                    client->keepAlive.start();
                } break;

                default: {
                    std::ostringstream oss;
                    oss << "Received (unwanted) packet when in disconnected-state (type: " << type << ")";
                    Logger::log(oss, Log::WARNING);
                };
            }

            /** END OF CRITICAL SECTION **/
            client->componentsMutex.unlock();
        }
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

