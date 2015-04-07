#include "catch.hpp"
#include "componentmanager.h"
#include "systemmanager.h"
#include "idmanager.h"
#include "entitymanager.h"
#include "movesystem.h"
#include "rendersystem.h"
#include "deltatime.h"
#include "hashgrid.h"
#include "flagsystem.h"
#include "collisionsystem.h"

TEST_CASE("EntityManager") {
	ComponentManager componentManager;
	DeltaTime deltaTime;
	SystemManager systemManager(&componentManager, &deltaTime);
	IdManager idManager;
	HashGrid hashGrid(&componentManager);
	EntityManager entityManager(&systemManager, &componentManager, &idManager, &hashGrid);
	FlagSystem flagSystem;

	MoveSystem moveSystem;
	RenderSystem renderSystem(&hashGrid);
	CollisionSystem collisionSystem;

	systemManager.add(&flagSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);
	systemManager.add(&collisionSystem);

	/* TESTING: Make sure a player can be created, updated, removed without breaking the systems */
	unsigned long long int* playerId = 0;

	SECTION("Adding and removing a player") {
		//Ensure player is within the expected systems
		playerId = entityManager.createPlayer();
		REQUIRE(entityManager.entities.size() == 1);
		REQUIRE(moveSystem.count() == 1);
		REQUIRE(renderSystem.count() == 1);

		//player should no longer exist within systems
		entityManager.remove(playerId);
		REQUIRE(entityManager.entities.size() == 0);
		REQUIRE(moveSystem.count() == 0);
		REQUIRE(renderSystem.count() == 0);
	}
}
