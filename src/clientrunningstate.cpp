#include "clientrunningstate.hpp"
#include "client.hpp"
#include "inputdata.hpp"
#include "timer.hpp"
#include "logger.hpp"
#include "irenderer.hpp"
#include "isoundengine.hpp"

ClientRunningState::ClientRunningState(Client* client) : client(client) { }

void ClientRunningState::step() {
	client->deltaTime.start();

    //Fetch all events that ocurred...
    client->renderer->pollEvents(this);

    //Poll packets (internally applied packets onto *this)
    client->packetManager.poll(client->clientState);

    //If user either pressed or released a key
    //then send keystrokes to server
    if(mouseIsMoving || !(presses.empty() && releases.empty())) {
        client->send<InputData, MESSAGE_TYPE::INPUTDATA>({presses, releases, mousePos});
    }
    presses.clear();
    releases.clear();
    mouseIsMoving = false;

    /** CRITICAL-SECTION **/
    Timer waitTimer;
    waitTimer.start();
    Logger::verbose("ClientReceiveInitialState locking componentsMutex in step...");
    client->componentsMutex.lock();
    Logger::verbose("ClientReceiveInitialState locked componentsMutex in step");
    const auto elapsed = waitTimer.elapsed();
    if(elapsed > 1.0f/50.0f) {
        std::ostringstream oss;
        oss << "Main-thread waited for " << elapsed << "s on componentsMutex. This is a probable cause of stuttering.";
        Logger::log(oss, Logger::WARNING);
    }

    /** Update client-side ECS **/
    Timer updateTimer;
    updateTimer.start();
    client->systemManager.update();
    const auto updateElapsed = updateTimer.elapsed();
    if(updateElapsed > 1.0f/50.0f) {
        std::ostringstream oss;
        oss << "Updating systems took " << updateElapsed << "s. This is a probable cause of stuttering.";
        Logger::log(oss, Logger::WARNING);
    }

    /** Check for timeout **/
    if(client->keepAlive.elapsed() > client->secondsUntilTimeout) {
        std::ostringstream oss;
        oss << "No packets from server received for " << client->keepAlive.elapsed() << "sec, server timeout";
        Logger::error(oss);
        changeState(&client->clientDisconnectedState);
    }

    /** END OF CRITICAL SECTION **/
    client->componentsMutex.unlock();
    Logger::verbose("ClientReceiveInitialState unlocked componentsMutex in step");

    //Limit client-speed to 60fps (rather 60 tick per second)
    //Check the elapsed time for the current step, if it is lower than
    //16.6666... then it's faster than 60fps so sleep until 16.666ms has passed
    using ms = std::chrono::milliseconds;
    using fdur = std::chrono::duration<float>;
    const float sleep_float = 1.0f/60 - client->deltaTime.elapsed();
    const ms sleep = std::chrono::duration_cast<ms>(fdur(sleep_float));
    std::this_thread::sleep_for(sleep);

    client->deltaTime.stop();
}

void ClientRunningState::changeState(IClientState* state) {
    state->onChange(this);
}

void ClientRunningState::onChange(ClientDisconnectedState* state) {
    Logger::log("Client can't change state from ClientDisconnectedState to ClientRunningState", Logger::WARNING);
}

void ClientRunningState::onChange(ClientReceiveInitialState* state) {
    Logger::log("Client changing state from ClientReceiveInitialState to ClientRunningState", Logger::INFO);
    client->soundEngine->playMusic(Music::NATURE_SOUNDS);
    client->clientState = this;
    client->renderer->fadeOutOverlay(Image::RECEIVING_DATA_OVERLAY, 0.5f);
}

void ClientRunningState::onChange(ClientRunningState* state) {
    Logger::log("Client can't change state from ClientRunningState to ClientRunningState", Logger::WARNING);
}

void ClientRunningState::greet(IPacket* packet) {
    packet->greet(this);
}

void ClientRunningState::handle(const OutdatedData* data) {
    Logger::log("This packet is outdated, to late! Sluggish!", Logger::WARNING);
}

void ClientRunningState::handle(const ServerReplyToConnectData* data) {
    Logger::log("Got camera ID from server", Logger::INFO);

    //Got my id. Tell camerasystem to follow that id.t
    const auto& pair = data->data;
    const auto& id = pair.first;
    client->systemManager.getSystem(pair.second)->add(id);
    client->playerId = id;
}

void ClientRunningState::handle(MoveComponentsDiffData* data) {
    Logger::log("Received MOVECOMPONENTSDIFF packet", Logger::VERBOSE);

    //Diff movecomponents were received - handle it
    client->componentManager.moveComponents.sync(data->data);

    for(auto& pair : data->data) {
        //Try to activate id, but it could be the case that
        //the entity is newly created, in that case activation wont work
        //so just add it instead. But before adding/activating, make sure
        //both components exists

        const auto& id = pair.first;
        const auto& mcs = client->componentManager.moveComponents;
        const auto& rcs = client->componentManager.renderComponents;
        if(mcs.contains(id) && rcs.contains(id)) {
            if(!client->textureHashGridSystem.activateId(id)) {
                client->textureHashGridSystem.add(id);
            }

            if(!client->renderSystem.activateId(id)) {
                client->renderSystem.add(id);
            }
        }
    }
}

void ClientRunningState::handle(RenderComponentsDiffData* data) {
    Logger::log("Received RENDERCOMPONENTSDIFF packet", Logger::VERBOSE);

    //Diff rendercomponents were received - handle it
    client->componentManager.renderComponents.sync(data->data);

    for(auto& pair : data->data) {
        const auto& id = pair.first;
        const auto& mcs = client->componentManager.moveComponents;
        const auto& rcs = client->componentManager.renderComponents;
        if(mcs.contains(id) && rcs.contains(id)) {
            if(!client->textureHashGridSystem.activateId(id)) {
                client->textureHashGridSystem.add(id);
            }

            if(!client->renderSystem.activateId(id)) {
                client->renderSystem.add(id);
            }
        }
    }
}

void ClientRunningState::handle(PlaySoundData* data) {
    Logger::log("Received PLAY_SOUND packet", Logger::VERBOSE);
    client->soundEngine->playSound(data->data);
}

void ClientRunningState::handle(const RegisterIdToSystemData* data) {
    Logger::log("Received REGISTER_ID_TO_SYSTEM packet", Logger::INFO);
    const auto& id = data->data.first;
    const auto& systemIdentifier = data->data.second;
    client->systemManager.getSystem(systemIdentifier)->add(id);
}

void ClientRunningState::handle(const RemoveIdData* data) {
    Logger::log("Received REMOVE_ID packet", Logger::INFO);

    const auto& id = data->data;
    client->systemManager.getSystem(System::RENDER)->remove(id);
    client->systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
    client->componentManager.clearComponents(id);

    //Check if entity controlled by this client were
    //the one being removed. If so, take special actions..
    if(id == client->playerId) {
        client->cameraSystem.remove(client->playerId);
    }
}

void ClientRunningState::handle(const RemoveIdFromSystemData* data) {
    Logger::log("Received REMOVE_ID_FROM_SYSTEM packet", Logger::INFO);
    const auto& id = data->data.first;
    const auto& systemIdentifier = data->data.second;
    client->systemManager.getSystem(systemIdentifier)->remove(id);
}

void ClientRunningState::handle(const RemoveIdFromSystemsData* data) {
    Logger::log("Received REMOVE_ID_FROM_SYSTEMS packet", Logger::INFO);
    const auto& id = data->data;
    client->systemManager.getSystem(System::RENDER)->remove(id);
    client->systemManager.getSystem(System::HASHGRID_TEXTURE)->remove(id);
}

void ClientRunningState::handle(const ActivateIdData* data) {
    Logger::log("Received ACTIVATE_ID packet", Logger::INFO);
    const auto& id = data->data.first;
    const auto& systemIdentifier = data->data.second;
    client->systemManager.getSystem(systemIdentifier)->activateId(id);
}

void ClientRunningState::handle(const KeepAliveData* data) {
    client->keepAlive.start();
}

void ClientRunningState::onEvent(const KeyEvent& evt) {
    std::ostringstream oss;
    oss << "Received KeyEvent " << evt.key << evt.press << " (ClientRunningState)";
    Logger::info(oss);
    if(evt.press) {
        presses.push_back(evt.key);
    } else {
        releases.push_back(evt.key);
    }
}

void ClientRunningState::onEvent(const MouseEvent& evt) {
    mousePos = {evt.x, evt.y};
    mouseIsMoving = true;
}

