#include "clientreceiveinitialstate.hpp"
#include "client.hpp"
#include "irenderer.hpp"
#include "isoundengine.hpp"

ClientReceiveInitialState::ClientReceiveInitialState(Client* client) : client(client) { }

void ClientReceiveInitialState::step() {
    //Fetch all events that ocurred...
    client->renderer->pollEvents(this);

    //Poll packets (internally applied packets onto *this)
    client->packetManager.poll(client->clientState);

    client->renderer->hideOverlay(Image::RECEIVING_DATA_OVERLAY);
    const Text text = {
        "Receiving data from server (" + std::to_string(receivedSmallContainers) + "/" + std::to_string(client->numberOfInitialSmallContainers) + ")",
        {120, client->renderer->getWindowResolution().x/2 - 10}, glm::vec3(1.0)
    };
    client->renderer->showOverlay(Image::RECEIVING_DATA_OVERLAY, text);
    client->renderer->renderOnlyOverlays();

    /** Check for timeout **/
    if(client->keepAlive.elapsed() > client->secondsUntilTimeout) {
        std::ostringstream oss;
        oss << "No packets from server received for " << client->keepAlive.elapsed() << "sec, server timeout";
        Logger::error(oss);
        changeState(&client->clientDisconnectedState);
    }

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
    Logger::log("Client changing state from ClientDisconnectedState to ClientReceiveInitialState", Logger::INFO);
    client->clientState = this;
}

void ClientReceiveInitialState::onChange(ClientReceiveInitialState* state) {
    Logger::log("Client can't change state from ClientReceiveInitialState to ClientReceiveInitialState", Logger::WARNING);
}

void ClientReceiveInitialState::onChange(ClientRunningState* state) {
    Logger::log("Client can't change state from ClientRunningState to ClientReceiveInitialState", Logger::WARNING);
}

void ClientReceiveInitialState::greet(IPacket* packet) {
    packet->greet(this);
}

void ClientReceiveInitialState::handle(const OutdatedData* data) {
    Logger::log("This packet is outdated, to late! Sluggish!", Logger::WARNING);
}

void ClientReceiveInitialState::handle(InitialComponentsData* data) {
	Logger::log("Received INITIAL_COMPONENTS packet", Logger::INFO);
	client->renderer->hideOverlay(Image::CONNECT_OVERLAY);
	receivedSmallContainers++;
	const Text text = {
		"Receiving data from server (" + std::to_string(receivedSmallContainers) + "/" + std::to_string(client->numberOfInitialSmallContainers) + ")",
        {120, client->renderer->getWindowResolution().x/2 - 10}, glm::vec3(1.0)
	};
	client->renderer->showOverlay(Image::RECEIVING_DATA_OVERLAY, text);

	client->componentManager.moveComponents.sync(data->mcs);
	client->componentManager.renderComponents.sync(data->rcs);

    /** TODO:   Should it really be mcs here? It seems odd that ids of mcs are added
                to textureHashGridSystem and renderSystem when we got a set of rendercomponents.
                maybe pair.first for all pairs in data->mcs is equivalent to pair.first for all pairs in data->rcs? **/
	for(auto& pair : data->mcs) {
		client->textureHashGridSystem.add(pair.first);
		client->renderSystem.add(pair.first);
	}

	//client->missingSequences.erase(packet.getSequence()); TODO: Re-implement this (now that packet isnt available here anymore)

    //If END_TRANSMITTING_INITIAL_COMPONENTS is already received, that means that
    //this INITIAL_COMPONENTS PACKET was 'late'.. So check if it was last one and if
    //so change to next state as usual
	if(endPacketReceived) {
		if(receivedSmallContainers == client->numberOfInitialSmallContainers) {
			changeState(&client->clientRunningState);
		}
	}
}

void ClientReceiveInitialState::handle(const EndTransmittingInitialComponentsData*) {
	Logger::info("Received END_TRANSMITTING_INITIAL_COMPONENTS packet");
	endPacketReceived = true;

	if(receivedSmallContainers < client->numberOfInitialSmallContainers) {
		Logger::error("Did not receive all initial components before END_TRANSMITTING_INITIAL_COMPONENTS (packets lost?)");

        //TODO: At some point, instead of going into disconnect mode,
        //ask server to resend the lost packets.
		changeState(&client->clientDisconnectedState);
	} else {
		changeState(&client->clientRunningState);
	}
}

void ClientReceiveInitialState::handle(const KeepAliveData*) {
	client->keepAlive.start();
}

void ClientReceiveInitialState::onEvent(const KeyEvent& evt) {

}

void ClientReceiveInitialState::onEvent(const MouseEvent& evt) {
    
}
