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
#include "packethandler.hpp"

typedef unsigned int ID;

class Server : public PacketHandler{
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

    std::string name() const override;
    void greet(IPacket* packet) override;
    void handle(const OutdatedData*) override;
    void handle(const ConnectToServerData*) override;
    void handle(const DisconnectData*) override;
    void handle(const InputDataData* data) override;
    void handle(const CongestedClientData* data) override;
    void handle(const NotCongestedClientData* data) override;
};


#endif //SERVER_HPP
