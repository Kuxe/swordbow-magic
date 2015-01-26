#include "catch.hpp"
#include "componentmanager.h"
#include "systemmanager.h"
#include "idmanager.h"
#include "entitymanager.h"
#include "movesystem.h"
#include "rendersystem.h"
#include "deltatime.h"
#include "gridindexer.h"
#include "flagsystem.h"

TEST_CASE("EntityManager") {
	ComponentManager componentManager;
	DeltaTime deltaTime;
	SystemManager systemManager(&componentManager, &deltaTime);
	IdManager idManager;
	GridIndexer gridIndexer(&componentManager);
	EntityManager entityManager(&systemManager, &componentManager, &idManager, &gridIndexer);
	FlagSystem flagSystem;

	MoveSystem moveSystem;
	RenderSystem renderSystem(&gridIndexer);

	systemManager.add(&flagSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);

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