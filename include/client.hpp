#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "components.hpp"
#include "deltatime.hpp"
#include "renderer.hpp"
#include "inputdata.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "rendersystem.hpp"
#include "hashgridsystem.hpp"
#include "camerasystem.hpp"
#include "textureboundingbox.hpp"
#include "soundsystem.hpp"

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>
#include <queue>
#include <mutex>

typedef unsigned int ID;

class Server;
class Client {
private:
    SDL_Event event;

    Server* server;

    DeltaTime deltaTime;
    ComponentManager componentManager;
    SystemManager systemManager;
    TextureBoundingBox textureBoundingBox;

    //Client-side systems
    HashGridSystem textureHashGridSystem;
    RenderSystem renderSystem;
    CameraSystem cameraSystem;
    SoundSystem soundSystem;

    Renderer renderer;

    std::mutex componentsMutex;

public:
    bool running = true;

    Client(int argc, char** argv);

    void connect(Server* server);
    void disconnect(Server* server);
    void run();
    void step();

    //These simulates real send and recv
    void send(InputData inputData);
    void recv(
        Components<MoveComponent> moveComponents,
        Components<RenderComponent> renderComponents,
        Components<SoundComponent> soundComponents,
        glm::vec2 pos);

    void registerIdToSystem(ID id, string);
    void removeIdFromSystem(ID id, string);
    void activateId(ID id, std::string systemIdentifier);

    //How the ECS handles sounds should maybe be redesigned...
    //Redesigning in such a way that sounds are entities to would
    //make this function bellow obsolete. Which is nice.
    void playSound(SoundComponent::Sound sound);
};

#endif //CLIENT_HPP
