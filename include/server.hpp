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
#include "ipacketacceptor.hpp"

typedef unsigned int ID;

class Server : public IPacketAcceptor {
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

    void inputDataToInputComponent(const IpAddress& ipAddress, InputData& data);

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

    Server(int argc, char** argv);
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

    //This handles the default case (ie any of the cases where accept isnt specialized)
    void accept(Packet<OUTDATED_TYPE,                               MESSAGE_TYPE::OUTDATED>& packet, const IpAddress& sender);
    void accept(Packet<CONNECT_TYPE,                                MESSAGE_TYPE::CONNECT>& packet, const IpAddress& sender);
    void accept(Packet<DISCONNECT_TYPE,                             MESSAGE_TYPE::DISCONNECT>& packet, const IpAddress& sender);
    void accept(Packet<INPUTDATA_TYPE,                              MESSAGE_TYPE::INPUTDATA>& packet, const IpAddress& sender);
    void accept(Packet<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE,  MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender);
    void accept(Packet<INITIAL_COMPONENTS_TYPE,                     MESSAGE_TYPE::INITIAL_COMPONENTS>& packet, const IpAddress& sender);
    void accept(Packet<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE,    MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender);
    void accept(Packet<MOVECOMPONENTSDIFF_TYPE,                     MESSAGE_TYPE::MOVECOMPONENTSDIFF>& packet, const IpAddress& sender);
    void accept(Packet<RENDERCOMPONENTSDIFF_TYPE,                   MESSAGE_TYPE::RENDERCOMPONENTSDIFF>& packet, const IpAddress& sender);
    void accept(Packet<PLAY_SOUND_TYPE,                             MESSAGE_TYPE::PLAY_SOUND>& packet, const IpAddress& sender);
    void accept(Packet<REGISTER_ID_TO_SYSTEM_TYPE,                  MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>& packet, const IpAddress& sender);
    void accept(Packet<REMOVE_ID_TYPE,                              MESSAGE_TYPE::REMOVE_ID>& packet, const IpAddress& sender);
    void accept(Packet<REMOVE_ID_FROM_SYSTEM_TYPE,                  MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>& packet, const IpAddress& sender);
    void accept(Packet<REMOVE_ID_FROM_SYSTEMS_TYPE,                 MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>& packet, const IpAddress& sender);
    void accept(Packet<ACTIVATE_ID_TYPE,                            MESSAGE_TYPE::ACTIVATE_ID>& packet, const IpAddress& sender);
    void accept(Packet<CONGESTED_CLIENT_TYPE,                       MESSAGE_TYPE::CONGESTED_CLIENT>& packet, const IpAddress& sender);
    void accept(Packet<NOT_CONGESTED_CLIENT_TYPE,                   MESSAGE_TYPE::NOT_CONGESTED_CLIENT>& packet, const IpAddress& sender);
    void accept(Packet<KEEP_ALIVE_TYPE,                             MESSAGE_TYPE::KEEP_ALIVE>& packet, const IpAddress& sender);
};


#endif //SERVER_HPP
