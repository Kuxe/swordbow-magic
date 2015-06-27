#include "client.hpp"
#include "server.hpp"
#include <iostream>
#include <SDL2/SDL.h>

Client::Client(int argc, char** argv) :
        systemManager(&componentManager, &deltaTime),
        textureBoundingBox(&componentManager, &renderer),
        textureHashGridSystem(&componentManager, &textureBoundingBox),
        renderSystem(&renderer),
        cameraSystem(&renderer) {

    cout << "\n--** STARTING CLIENT **--" << endl;

    systemManager.add(&textureHashGridSystem);
    systemManager.add(&renderSystem);
    systemManager.add(&animationSystem);
    systemManager.add(&cameraSystem);
    systemManager.add(&soundSystem);

    //Play some sweet music
	soundSystem.playMusic("./resources/sounds/naturesounds.ogg");
}

void Client::connect(Server* server) {
    //Send connect-request to server to connect
    this->server = server;
    server->onConnect(this);
}

void Client::disconnect(Server* server) {
    //Send disconnect-request to server
    server->onDisconnect(this);
}

void Client::run() {
    while(running) {
        step();
    }
    server->terminate(); //If client is shutdown, kill server too for now
}

void Client::step() {
    deltaTime.start();

    /** Get input from client **/
    queue<int> presses;
    queue<int> releases;

    //Fetch all events that ocurred...
    while(SDL_PollEvent(&event) != 0) {
        //And take appropiate action!
        switch(event.type) {
            case SDL_QUIT: {
                running = false;
                break;
            }
            //If this event was a keydown...
            case SDL_KEYDOWN: {
                presses.push(event.key.keysym.sym);
            } break;

            //If user released a key
            case SDL_KEYUP: {
                releases.push(event.key.keysym.sym);

            } break;
        }
    }

    //If user either pressed or released a key
    if( !(presses.empty() && releases.empty()) ) {
        send({presses, releases});
    }

    /** CRITICAL SECTION **/
    componentsMutex.lock();

    /** Update client-side ECS **/
    systemManager.update();

    /** END OF CRITICAL SECTION **/
    componentsMutex.unlock();

    /** Pass output from rendersystem to renderer **/
    renderer.render(renderSystem.getDrawPriorityQueue(), cameraSystem.getCamera());

    deltaTime.stop();
}

void Client::send(InputData inputData) {
    server->recv(this, inputData);
}

void Client::recv(
        Components<MoveComponent> moveComponents,
        Components<RenderComponent> renderComponents,
        Components<AnimationComponent> animationComponents,
        Components<SoundComponent> soundComponents,
        glm::vec2 pos) {

    //Only allow updating components if they're not in use by something else
    componentsMutex.lock();

    //This is much more copying than neccesary. It is sufficient
    //to only pass into recv() some lists:
    //  *A list of movecomponents that changed
    //  *A list of rendercomponents that changed
    //  *A list of animationComponents that changed
    componentManager.moveComponents = moveComponents;
    componentManager.renderComponents = renderComponents;
    componentManager.animationComponents = animationComponents;
    componentManager.soundComponents = soundComponents;

    //Now its OK to update client-ecs!
    componentsMutex.unlock();
}

void Client::registerIdToSystem(ID id, std::string systemIdentifier) {
    componentsMutex.lock();
    systemManager.getSystem(systemIdentifier)->add(id);
    componentsMutex.unlock();
}

void Client::removeIdFromSystem(ID id, std::string systemIdentifier) {
    componentsMutex.lock();
    cout << "client: removing " << id << " from " << systemIdentifier << endl;
    systemManager.getSystem(systemIdentifier)->remove(id);
    componentsMutex.unlock();
}

void Client::activateId(ID id, std::string systemIdentifier) {
    componentsMutex.lock();
    systemManager.getSystem(systemIdentifier)->activateId(id);
    componentsMutex.unlock();
}

void Client::playSound(SoundComponent::Sound sound) {
    soundSystem.playSound(sound);
}










//
