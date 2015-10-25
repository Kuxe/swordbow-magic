#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "deltatime.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "renderer.hpp"
#include "rendersystem.hpp"
#include "hashgridsystem.hpp"
#include "camerasystem.hpp"
#include "textureboundingbox.hpp"
#include "soundengine.hpp"
#include "socket.hpp"
#include "messagetypes.hpp"
#include "timer.hpp"

/** States of client **/
#include "clientdisconnectedstate.hpp"
#include "clientreceiveinitialstate.hpp"
#include "clientrunningstate.hpp"

#include <SDL2/SDL.h>
#include <mutex>
#include <thread>

typedef unsigned int ID;

class ClientState;
class Client {
private:
    Socket socket;
    IpAddress server;
    uint16_t sequence;

    std::thread receiveThread;
    bool receiveThreadRunning;

    SDL_Event event;

    DeltaTime deltaTime;
    ComponentManager componentManager;
    SystemManager systemManager;
    Renderer renderer;
    TextureBoundingBox textureBoundingBox;
    Timer keepAlive;
    static constexpr float secondsUntilTimeout = 10;

    //Client-side systems
    HashGridSystem textureHashGridSystem;
    CameraSystem cameraSystem;
    RenderSystem renderSystem;
    SoundEngine soundEngine;

    std::mutex componentsMutex;

    //This variable is received in BEGIN_TRANSMITTING_INITIAL_COMPONENTS from server
    //Client needs this to know how many packets it should get from server
    int numberOfInitialSmallContainers = 0;
    std::unordered_set<uint16_t> missingSequences;

    //ID that this client controlls
    ID playerId;

    //States of the client
    IClientState* clientState;
    friend ClientDisconnectedState;
    ClientDisconnectedState clientDisconnectedState;
    friend ClientReceiveInitialState;
    ClientReceiveInitialState clientReceiveInitialState;
    friend ClientRunningState;
    ClientRunningState clientRunningState;

    void receive();

public:
    bool running = true;

    //Helper method for sending packets
    template<class DataType>
    void send(DataType data, MESSAGE_TYPE message) {

        using PacketType = Packet<DataType>;
        PacketType cameraPacket = {
            stringhash("swordbow-magic"),
            sequence++,
            message,
            data,
            sizeof(data)
        };

        socket.send<PacketType>(server, cameraPacket);
    }

    Client(int argc, char** argv);
    ~Client();

    void connect(const IpAddress& server);
    void disconnect();
    void run();
    void step();
    void stop();
};

#endif //CLIENT_HPP
