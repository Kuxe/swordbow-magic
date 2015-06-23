#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "deltatime.hpp"
#include "renderer.hpp"
#include "inputdata.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "rendersystem.hpp"
#include "hashgridsystem.hpp"
#include "animationsystem.hpp"
#include "camerasystem.hpp"
#include "textureboundingbox.hpp"

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>
#include <queue>

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
    RenderSystem renderSystem;
    HashGridSystem textureHashGridSystem;
    AnimationSystem animationSystem;
    CameraSystem cameraSystem;


    Renderer renderer;

    SDL_Rect camera;

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
        unordered_map<ID, MoveComponent> moveComponents,
        unordered_map<ID, RenderComponent> renderComponents,
        unordered_map<ID, AnimationComponent> animationComponents,
        glm::vec2& pos);

    void registerIdToSystem(ID id, string);
    void removeIdFromSystem(ID id, string);
    void activateId(ID id, std::string systemIdentifier);
};

#endif //CLIENT_HPP
