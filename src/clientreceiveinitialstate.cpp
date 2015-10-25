#include "clientreceiveinitialstate.hpp"
#include "client.hpp"

ClientReceiveInitialState::ClientReceiveInitialState(Client* client) : client(client) { }

void ClientReceiveInitialState::receive() {
	//Receive data from server...
	IpAddress server;
	MESSAGE_TYPE type;
    int bytesRead;
    client->socket.receive(server, type, bytesRead);

    if(bytesRead > 0) {

        /** CRITICAL-SECTION **/
        client->componentsMutex.lock();

        switch(type) {
            case MESSAGE_TYPE::OUTDATED: {
                    Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
                } break;
            
            case MESSAGE_TYPE::INITIAL_COMPONENTS: {
            	Logger::log("Received INITIAL_COMPONENTS packet", Log::INFO);
                client->renderer.hideOverlay(Image::CONNECT_OVERLAY);
                receivedSmallContainers++;
                const Text text = {
                    "Receiving data from server (" + std::to_string(receivedSmallContainers) + "/" + std::to_string(client->numberOfInitialSmallContainers) + ")",
                    150,
                    client->renderer.getScreenHeight()/2 - 10
                };
                client->renderer.showOverlay(Image::RECEIVING_DATA_OVERLAY, text);

				using DataType = std::pair<Components<MoveComponent>, Components<RenderComponent>>;
				using PacketType = Packet<DataType>;
				auto typedPacket = client->socket.get<PacketType>(bytesRead);

				client->componentManager.moveComponents.sync(typedPacket.getData().first);
				client->componentManager.renderComponents.sync(typedPacket.getData().second);

                for(auto& pair : typedPacket.getData().first) {
                    client->textureHashGridSystem.add(pair.first);
                    client->renderSystem.add(pair.first);
                }

                client->missingSequences.erase(typedPacket.getSequence());

                //If END_TRANSMITTING_INITIAL_COMPONENTS is already received, that means that
                //this INITIAL_COMPONENTS PACKET was 'late'.. So check if it was last one and if
                //so change to next state as usual
                if(endPacketReceived) {
                    if(receivedSmallContainers == client->numberOfInitialSmallContainers) {
                        changeState(&client->clientRunningState);
                    }
                }

        	} break;

        	case MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS: {
        	    Logger::log("Received END_TRANSMITTING_INITIAL_COMPONENTS packet", Log::INFO);
                endPacketReceived = true;

                if(receivedSmallContainers < client->numberOfInitialSmallContainers) {
                    Logger::log("Did not receive all initial components before END_TRANSMITTING_INITIAL_COMPONENTS (packets lost?)", Log::ERROR);

                    //TODO: At some point, instead of going into disconnect mode,
                    //ask server to resend the lost packets.
                    changeState(&client->clientDisconnectedState);
                } else {
                    changeState(&client->clientRunningState);
                }
        	} break;

            case MESSAGE_TYPE::KEEP_ALIVE: {
                    client->keepAlive.start();
                } break;

        	default: {
        	    std::ostringstream oss;
        	    oss << "Received (unwanted) packet when in receive-initial-state (type: " << type << ")";
        	    Logger::log(oss, Log::WARNING);
        	} break;
        }

        /** END OF CRITICAL-SECTION **/
        client->componentsMutex.unlock();
    }
}

void ClientReceiveInitialState::step() {
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
    client->componentsMutex.lock();
    client->renderer.hideOverlay(Image::RECEIVING_DATA_OVERLAY);
    const Text text = {
        "Receiving data from server (" + std::to_string(receivedSmallContainers) + "/" + std::to_string(client->numberOfInitialSmallContainers) + ")",
        120,
        client->renderer.getScreenHeight()/2 - 10
    };
    client->renderer.showOverlay(Image::RECEIVING_DATA_OVERLAY, text);
    client->renderer.renderOnlyOverlays();

    /** Check for timeout **/
    if(client->keepAlive.elapsed() > client->secondsUntilTimeout) {
        std::ostringstream oss;
        oss << "No packets from server received for " << client->keepAlive.elapsed() << "sec, server timeout";
        Logger::log(oss, Log::ERROR);
        changeState(&client->clientDisconnectedState);
    }

    client->componentsMutex.unlock();

    //If it weren't for 10ms sleep, this thread would lock out
    //the receive-thread to the extent of losing lots of packets
    using namespace std::literals;
    std::this_thread::sleep_for(10ms);
}

void ClientReceiveInitialState::changeState(IClientState* state) {
    state->onChange(this);
}

void ClientReceiveInitialState::onChange(ClientDisconnectedState* state) {
    Logger::log("Client changing state from ClientDisconnectedState to ClientReceiveInitialState", Log::INFO);
    client->clientState = this;
}

void ClientReceiveInitialState::onChange(ClientReceiveInitialState* state) {
    Logger::log("Client can't change state from ClientReceiveInitialState to ClientReceiveInitialState", Log::WARNING);
}

void ClientReceiveInitialState::onChange(ClientRunningState* state) {
    Logger::log("Client can't change state from ClientRunningState to ClientReceiveInitialState", Log::WARNING);
}
