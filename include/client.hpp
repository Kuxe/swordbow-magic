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
#include "soundengine.hpp"
#include "socket.hpp"

#include <SDL2/SDL.h>
#include <glm/vec2.hpp>
#include <queue>
#include <mutex>

typedef unsigned int ID;

class Server;
class Client {
private:
    Socket socket;
    IpAddress server;

    SDL_Event event;

    DeltaTime deltaTime;
    ComponentManager componentManager;
    SystemManager systemManager;
    TextureBoundingBox textureBoundingBox;

    //Client-side systems
    HashGridSystem textureHashGridSystem;
    RenderSystem renderSystem;
    CameraSystem cameraSystem;
    SoundEngine soundEngine;

    Renderer renderer;

    std::mutex componentsMutex;

public:
    bool running = true;

    Client(int argc, char** argv);
    ~Client();

    void connect(const IpAddress& server);
    void disconnect();
    void run();
    void step();
};

#endif //CLIENT_HPP
