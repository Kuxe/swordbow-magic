#ifndef SERVER_HPP
#define SERVER_HPP

#include "server.hpp"
#include <iostream>
#include <SDL2/SDL.h>
#include "animationsystem.hpp"
#include "deltatime.hpp"
#include "keyboardsystem.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"
#include "idmanager.hpp"
#include "entitymanager.hpp"
#include "movesystem.hpp"
#include "rendersystem.hpp"
#include "movecomponent.hpp"
#include "rendercomponent.hpp"
#include "world.hpp"
#include "hashgridsystem.hpp"
#include "collisionsystem.hpp"
#include "textureboundingbox.hpp"
#include "sizeboundingbox.hpp"
#include "soundsystem.hpp"
#include "healthsystem.hpp"
#include "removesystem.hpp"
#include "attacksystem.hpp"
#include "inputsystem.hpp"
#include "camerasystem.hpp"
#include "servernetworksystem.hpp"
#include <assert.h>
#include <glm/ext.hpp>

class Server {
private:
    bool running = true;

    DeltaTime deltaTime;

	ComponentManager componentManager;
	SystemManager systemManager;
	TextureBoundingBox textureBoundingBox;
	SizeBoundingBox sizeBoundingBox;
	IdManager idManager;
	EntityManager entityManager;

	//Declare systems
	HashGridSystem textureHashGridSystem;
	HashGridSystem sizeHashGridSystem;
	MoveSystem moveSystem;
	RenderSystem renderSystem;
	CollisionSystem collisionSystem;
	SoundSystem soundSystem;
	AnimationSystem animationSystem;
	HealthSystem healthSystem;
	RemoveSystem removeSystem;
	AttackSystem attackSystem;
	InputSystem inputSystem;
	CameraSystem cameraSystem;
	KeyboardSystem keyboardSystem;
	ServerNetworkSystem serverNetworkSystem;

public:
    Server(int argc, char** argv);
    void run();
};


#endif //SERVER_HPP
