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
            case MESSAGE_TYPE::INITIAL_COMPONENTS: {
            	Logger::log("Received INITIAL_COMPONENTS packet", Log::INFO);
                client->renderer.showOverlay(Image::RECEIVING_DATA_OVERLAY);

				using DataType = std::pair<Components<MoveComponent>, Components<RenderComponent>>;
				using PacketType = Packet<DataType>;
				auto typedPacket = client->socket.get<PacketType>(bytesRead);

				client->componentManager.moveComponents.sync(typedPacket.getData().first);
				client->componentManager.renderComponents.sync(typedPacket.getData().second);

                for(auto& pair : typedPacket.getData().first) {
                    client->textureHashGridSystem.add(pair.first);
                    client->renderSystem.add(pair.first);
                }

        	} break;

        	case MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS: {
        	    Logger::log("Received END_TRANSMITTING_INITIAL_COMPONENTS packet", Log::INFO);
                client->renderer.fadeOutOverlay(Image::RECEIVING_DATA_OVERLAY, 0.5f);
                client->clientState = &client->clientRunningState;
                //Play some sweet music
                client->soundEngine.playMusic(Music::NATURE_SOUNDS);
        	} break;

        	default: {
        	    std::ostringstream oss;
        	    oss << "Received packet when in receiving-initial-components-mode (type: " << type << ")";
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
    client->renderer.renderOnlyOverlays();
}