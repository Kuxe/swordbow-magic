#ifndef SERVER_HPP
#define SERVER_HPP

#include <unordered_map>
#include <mutex>

#include "deltatime.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "idmanager.hpp"
#include "entitymanager.hpp"

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

/** For inputDataToInputComponent-method **/
#include "inputdata.hpp"

/** For network **/
#include "socket.hpp"
#include "clientdata.hpp"

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

    std::unordered_map<IpAddress, ClientData> clients;

    std::mutex componentsMutex;

    void inputDataToInputComponent(const IpAddress& ipAddress, InputData& data);

public:
    bool running = true;

    Server(int argc, char** argv);
    ~Server();
    void onConnect(const IpAddress& ipAddress);
    void onDisconnect(const IpAddress& ipAddress);
    void run();
    void terminate();
    void step();

    void send();
    void send(const IpAddress& ipAddress);
    void sendDiff();
    void sendDiff(const IpAddress& ipAddress);

    void printGeneralInfo();
};


#endif //SERVER_HPP
