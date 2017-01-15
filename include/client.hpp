#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "deltatime.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "rendersystem.hpp"
#include "hashgridsystem.hpp"
#include "camerasystem.hpp"
#include "textureboundingbox.hpp"
#include "packetmanager.hpp"
#include "messagetypes.hpp"
#include "timer.hpp"

/** States of client **/
#include "clientdisconnectedstate.hpp"
#include "clientreceiveinitialstate.hpp"
#include "clientrunningstate.hpp"

#include <mutex>
#include <thread>

typedef unsigned int ID;

class IRenderer;
class ISoundEngine;
class ClientState;
class Client {
private:

    IRenderer* const renderer;
    ISoundEngine* const soundEngine;

    PacketManager packetManager;
    IpAddress server;
    uint16_t sequence;

    std::thread receiveThread;
    bool receiveThreadRunning;

    DeltaTime deltaTime;
    ComponentManager componentManager;
    SystemManager systemManager;
    TextureBoundingBox textureBoundingBox;
    Timer keepAlive;
    static constexpr float secondsUntilTimeout = 10;

    //Client-side systems
    HashGridSystem textureHashGridSystem;
    CameraSystem cameraSystem;
    RenderSystem renderSystem;

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
    template<class DataType, MESSAGE_TYPE Message>
    void send(DataType data) {
        Packet<DataType, Message> cameraPacket = {
            stringhash("swordbow-magic"),
            sequence++,
            data,
            sizeof(data)
        };

        packetManager.send<DataType>(server, cameraPacket);
    }

    Client(IRenderer* const renderer, ISoundEngine* const soundEngine, unsigned short port);
    ~Client();

    void connect(const IpAddress& server);
    void disconnect();
    void run();
    void step();
    void stop();
};

#endif //CLIENT_HPP
