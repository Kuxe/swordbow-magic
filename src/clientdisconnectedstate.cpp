#include "clientdisconnectedstate.hpp"
#include "client.hpp"

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
                    client->clientState = &client->clientReceiveInitialState;
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
}
