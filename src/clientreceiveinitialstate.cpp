#include "clientreceiveinitialstate.hpp"
#include "client.hpp"

ClientReceiveInitialState::ClientReceiveInitialState(Client* client) : client(client) { }

void ClientReceiveInitialState::receive() {
    client->componentsMutex.lock();
    client->packetManager.receive<ClientReceiveInitialState>(*this);
    client->componentsMutex.unlock();
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
    //Reset all the data...
    receivedSmallContainers = 0;
    endPacketReceived = false;
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

void ClientReceiveInitialState::accept(Packet<OUTDATED_TYPE, MESSAGE_TYPE::OUTDATED>& packet, const IpAddress& sender) {
    Logger::log("This packet is outdated, to late! Sluggish!", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<CONNECT_TYPE, MESSAGE_TYPE::CONNECT>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::CONNECT) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<DISCONNECT_TYPE, MESSAGE_TYPE::DISCONNECT>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::DISCONNECT) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<INPUTDATA_TYPE, MESSAGE_TYPE::INPUTDATA>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::INPUTDATA) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::INITIAL_COMPONENTS>& packet, const IpAddress& sender) {
	Logger::log("Received INITIAL_COMPONENTS packet", Log::INFO);
	client->renderer.hideOverlay(Image::CONNECT_OVERLAY);
	receivedSmallContainers++;
	const Text text = {
		"Receiving data from server (" + std::to_string(receivedSmallContainers) + "/" + std::to_string(client->numberOfInitialSmallContainers) + ")",
		150,
		client->renderer.getScreenHeight()/2 - 10
	};
	client->renderer.showOverlay(Image::RECEIVING_DATA_OVERLAY, text);

	client->componentManager.moveComponents.sync(packet.getData().first);
	client->componentManager.renderComponents.sync(packet.getData().second);

	for(auto& pair : packet.getData().first) {
		client->textureHashGridSystem.add(pair.first);
		client->renderSystem.add(pair.first);
	}

	client->missingSequences.erase(packet.getSequence());

    //If END_TRANSMITTING_INITIAL_COMPONENTS is already received, that means that
    //this INITIAL_COMPONENTS PACKET was 'late'.. So check if it was last one and if
    //so change to next state as usual
	if(endPacketReceived) {
		if(receivedSmallContainers == client->numberOfInitialSmallContainers) {
			changeState(&client->clientRunningState);
		}
	}
}

void ClientReceiveInitialState::accept(Packet<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) {
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
}

void ClientReceiveInitialState::accept(Packet<MOVECOMPONENTSDIFF_TYPE, MESSAGE_TYPE::MOVECOMPONENTSDIFF>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::MOVECOMPONENTSDIFF) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<RENDERCOMPONENTSDIFF_TYPE, MESSAGE_TYPE::RENDERCOMPONENTSDIFF>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::RENDERCOMPONENTSDIFF) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<PLAY_SOUND_TYPE, MESSAGE_TYPE::PLAY_SOUND>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::PLAY_SOUND) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<REGISTER_ID_TO_SYSTEM_TYPE, MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<REMOVE_ID_TYPE, MESSAGE_TYPE::REMOVE_ID>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::REMOVE_ID) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<REMOVE_ID_FROM_SYSTEM_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<REMOVE_ID_FROM_SYSTEMS_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<ACTIVATE_ID_TYPE, MESSAGE_TYPE::ACTIVATE_ID>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::ACTIVATE_ID) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::CONGESTED_CLIENT>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::CONGESTED_CLIENT) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<NOT_CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::NOT_CONGESTED_CLIENT>& packet, const IpAddress& sender) {
	Logger::log("Received (unwanted) packet when in receive-initial-state (type: " + std::to_string(MESSAGE_TYPE::NOT_CONGESTED_CLIENT) + ")", Log::WARNING);
}

void ClientReceiveInitialState::accept(Packet<KEEP_ALIVE_TYPE, MESSAGE_TYPE::KEEP_ALIVE>& packet, const IpAddress& sender) {
	client->keepAlive.start();
}
