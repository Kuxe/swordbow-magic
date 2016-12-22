#ifndef SERVER_HPP
#define SERVER_HPP

#include <unordered_map>

#include "deltatime.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "idmanager.hpp"
#include "entitymanager.hpp"
#include "timer.hpp"

/** Server-side systems **/
#include "movesystem.hpp"
#include "hashgridsystem.hpp"
#include "collisionsystem.hpp"
#include "sizeboundingbox.hpp"
#include "healthsystem.hpp"
#include "removesystem.hpp"
#include "attacksystem.hpp"
#include "inputsystem.hpp"
#include "animationsystem.hpp"
#include "movediffsystem.hpp"
#include "renderdiffsystem.hpp"
#include "birdsystem.hpp"
#include "accelerationsystem.hpp"
#include "initialcomponentssystem.hpp"

/** For inputDataToInputComponent-method **/
#include "inputdata.hpp"

/** For network **/
#include "packetmanager.hpp"
#include "clientdata.hpp"
#include "messagetypes.hpp"
#include "messagetypes.hpp"
#include "ipaddress.hpp"

typedef unsigned int ID;

class Server {
private:
    PacketManager packetManager;

    DeltaTime deltaTime;
    float tps = 60.0f; //tick per second

	ComponentManager componentManager;
	SystemManager systemManager;
	SizeBoundingBox sizeBoundingBox;
	IdManager idManager;
	EntityManager entityManager;

    Timer keepAlive;
    static constexpr float keepAliveInterval = 1;

	//Declare systems
	HashGridSystem sizeHashGridSystem;
	MoveSystem moveSystem;
	CollisionSystem collisionSystem;
	HealthSystem healthSystem;
	RemoveSystem removeSystem;
	AttackSystem attackSystem;
	InputSystem inputSystem;
    AnimationSystem animationSystem;
    MoveDiffSystem moveDiffSystem;
    RenderDiffSystem renderDiffSystem;
    BirdSystem birdSystem;
    AccelerationSystem accelerationSystem;
    InitialComponentsSystem initialComponentsSystem;

    std::unordered_map<IpAddress, ClientData> clients;

    void inputDataToInputComponent(const IpAddress& ipAddress, const InputData& data);

    //Helper method for sending packets
    template<class DataType, MESSAGE_TYPE Message>
    void send(const IpAddress& ipAddress, DataType data) {
        auto& clientData = clients.at(ipAddress);

        Packet<DataType, Message> cameraPacket = {
            stringhash("swordbow-magic"),
            clientData.sequence++,
            data,
            sizeof(data)
        };

        packetManager.send<DataType>(ipAddress, cameraPacket);
    }

public:
    bool running = true;

    Server(bool smallWorld, float tps);
    ~Server();
    void onConnect(const IpAddress& ipAddress);
    void onDisconnect(const IpAddress& ipAddress);
    void run();
    void terminate();
    void step();

    //Sends all components to client using MESSAGE_TYPE::INITIAL_COMPONENTS
    void sendInitial();
    void sendInitial(const IpAddress& ipAddress);

    void sendDiff();
    void sendDiff(const IpAddress& ipAddress);

    void sendKeepAlive();

    void printGeneralInfo();

    void accept(const OutdatedData&, const IpAddress& sender);
    void accept(const ConnectToServerData&, const IpAddress& sender);
    void accept(const DisconnectData&, const IpAddress& sender);
    void accept(const InputDataData& data, const IpAddress& sender);
    void accept(const CongestedClientData& data, const IpAddress& sender);
    void accept(const NotCongestedClientData& data, const IpAddress& sender);
    void accept(const auto& data, const IpAddress& sender);
};


#endif //SERVER_HPP
