#ifndef SERVER_HPP
#define SERVER_HPP

#include "server.hpp"
#include <iostream>
#include "deltatime.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "idmanager.hpp"
#include "entitymanager.hpp"
#include "movesystem.hpp"
#include "rendersystem.hpp"
#include "movecomponent.hpp"
#include "world.hpp"
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
#include <unordered_map>
#include "inputdata.hpp"
#include <mutex>
#include "socket.hpp"

typedef unsigned int ID;

class Client;
class Server {
private:
    DeltaTime deltaTime;

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

    std::unordered_map<unsigned int, ID> clients;

    std::mutex componentsMutex;

    Socket socket;

    void inputDataToInputComponent(unsigned int client, InputData& data);

public:
    bool running = true;

    Server(int argc, char** argv);
    ~Server();
    void onConnect(unsigned int client, unsigned short port);
    void onDisconnect(unsigned int client);
    void run();
    void terminate();
    void step();

    void send();
    void send(unsigned int client, unsigned short port);
    void sendDiff();
    void sendDiff(unsigned int client, unsigned short port);

    constexpr void printGeneralInfo() {
    	using std::cout;
    	using std::endl;

    	//Print some worst-case memory statistics, assuming
    	//all entities have all components
    	//(which isn't the case)
    	uint64_t bytePerMegabyte = 1048576;
    	uint64_t entityByteSize = ComponentManager::sizePerEntity();
    	uint64_t allEntitiesByteSize = IdManager::MAX_IDS * entityByteSize;
    	uint64_t allEntitiesMegabyteSize = allEntitiesByteSize / bytePerMegabyte;
    	cout << "There can be at most " << IdManager::MAX_IDS << " ids";
    	cout << " each of size " << entityByteSize << "bytes summing";
    	cout << " up to " << allEntitiesByteSize;
    	cout << "bytes (" << allEntitiesMegabyteSize;
    	cout << "MB)" << endl;
    }
};


#endif //SERVER_HPP
