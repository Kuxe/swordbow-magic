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
                    const auto& screenWidth = client->renderer.getScreenWidth();
                    const auto& screenHeight = client->renderer.getScreenHeight();
                    const Text text = {"Receiving data from server...", screenWidth / 2 - 100, screenHeight / 2 - 15, {200, 200, 200}};

                    client->renderer.fadeOutOverlay(Image::CONNECT_OVERLAY, 0.5);
                    client->clientState = &client->clientReceiveInitialState;
                } break;

                default: {
                    std::ostringstream oss;
                    oss << "Message without proper type received. This is probably a bug.";
                    oss << " Either client-side handling for that message isn't implemented";
                    oss << " or server sent a message with a bogus messagetype";
                    oss << " or the messagetype was wrongly altered somewhere (type: " << type << ")";
                    Logger::log(oss, Log::WARNING);
                };
            }

            /** END OF CRITICAL SECTION **/
            client->componentsMutex.unlock();
        }

        if(client->keepAlive.elapsed() > client->secondsUntilTimeout) {
            client->running = false;
            client->receiveThreadRunning = false;
            std::ostringstream oss;
            oss << "No packets from server received for " << client->keepAlive.elapsed() << "sec, server timeout";
            Logger::log(oss, Log::ERROR);
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
