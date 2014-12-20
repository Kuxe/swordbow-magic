#include "catch.hpp"
#include "componentmanager.h"
#include "systemmanager.h"
#include "idmanager.h"
#include "entitymanager.h"
#include "movesystem.h"
#include "rendersystem.h"

TEST_CASE("Entitymanager test") {
	SystemManager systemManager;
	ComponentManager componentManager;
	IdManager idManager;

	EntityManager entityManager(&systemManager, &componentManager, &idManager);

	MoveSystem moveSystem;
	RenderSystem renderSystem;

	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);

	/* TESTING: Make sure a player can be created, updated, removed without breaking the systems */
	unsigned long long int* playerId = 0;

	SECTION("Adding player") {
		playerId = entityManager.createPlayer();
	}

	SECTION("Removing player") {
		entityManager.removePlayer(playerId);
	}
}