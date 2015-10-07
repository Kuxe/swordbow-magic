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

/** For inputDataToInputComponent-method **/
#include "inputdata.hpp"

/** For network **/
#include "socket.hpp"
#include "clientdata.hpp"
#include "messagetypes.hpp"

typedef unsigned int ID;

class Server {
private:
    Socket socket;

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

    std::unordered_map<IpAddress, ClientData> clients;

    void inputDataToInputComponent(const IpAddress& ipAddress, InputData& data);

    //Helper method for sending packets
    template<class DataType>
    void send(const IpAddress& ipAddress, DataType data, MESSAGE_TYPE message) {
        auto& clientData = clients.at(ipAddress);

        using PacketType = Packet<DataType>;
        PacketType cameraPacket = {
            stringhash("swordbow-magic"),
            clientData.sequence++,
            message,
            data,
            sizeof(data)
        };

        socket.send<PacketType>(ipAddress, cameraPacket);
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
};


#endif //SERVER_HPP
