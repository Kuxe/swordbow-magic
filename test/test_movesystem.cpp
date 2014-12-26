#include "catch.hpp"
#include "componentmanager.h"
#include "deltatime.h"
#include "systemmanager.h"
#include "idmanager.h"
#include "entitymanager.h"
#include "movesystem.h"
#include "rendersystem.h"
#include "movecomponent.h"
#include "inputcomponent.h"

TEST_CASE("MoveSystem") {
	ComponentManager componentManager;
	DeltaTime deltaTime;
	//Get a deltaTime (nothing will move if no time has passed!)
	deltaTime.start();
	deltaTime.stop();
	SystemManager systemManager(&componentManager, &deltaTime);
	IdManager idManager;

	EntityManager entityManager(&systemManager, &componentManager, &idManager);

	MoveSystem moveSystem;
	RenderSystem renderSystem;

	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);

	/* 	Make sure a player is moved on input. Input is simulated in this test.
		This test doesn't check if player moves accuratly or in the right direction
		It checks if player moves at all on keystrokes
	*/
	unsigned long long int* playerId = 0;
	SECTION("Check if player respond to input") {
		playerId = entityManager.createPlayer();
		InputComponent* playerInputComponent = componentManager.inputComponents.at(playerId);
		MoveComponent* playerMoveComponent = componentManager.moveComponents.at(playerId);
		systemManager.update();

		//Remember coordinates before movement
		float currentXpos = playerMoveComponent->xpos;
		float currentYpos = playerMoveComponent->ypos;
		//Press w
		playerInputComponent->w = true;
		systemManager.update();
		//then player be on same X but another Y
		REQUIRE(currentXpos == playerMoveComponent->xpos);
		REQUIRE(currentYpos != playerMoveComponent->ypos);

		//Remember coordinates before movement
		currentXpos = playerMoveComponent->xpos;
		currentYpos = playerMoveComponent->ypos;
		//Release w and press d
		playerInputComponent->w = false;
		playerInputComponent->d = true;
		systemManager.update();
		//then player should be on another X but same Y
		REQUIRE(currentXpos != playerMoveComponent->xpos);
		REQUIRE(currentYpos == playerMoveComponent->ypos);

		//Remember coordinates before movement
		currentXpos = playerMoveComponent->xpos;
		currentYpos = playerMoveComponent->ypos;
		//Release d and press s
		playerInputComponent->d = false;
		playerInputComponent->s = true;
		systemManager.update();	
		//then player should be on same X but another Y
		REQUIRE(currentXpos == playerMoveComponent->xpos);
		REQUIRE(currentYpos != playerMoveComponent->ypos);

		//Remember coordinates before movement
		currentXpos = playerMoveComponent->xpos;
		currentYpos = playerMoveComponent->ypos;
		//Release s and press a
		playerInputComponent->s = false;
		playerInputComponent->a = true;
		systemManager.update();
		//then player should be on another X but same Y
		REQUIRE(currentXpos != playerMoveComponent->xpos);
		REQUIRE(currentYpos == playerMoveComponent->ypos);

		//Remember coordinates before movement
		currentXpos = playerMoveComponent->xpos;
		currentYpos = playerMoveComponent->ypos;
		//Release a and press w
		playerInputComponent->a = false;
		playerInputComponent->w = true;
		systemManager.update();
		//then player should be on same X but another Y
		REQUIRE(currentXpos == playerMoveComponent->xpos);
		REQUIRE(currentYpos != playerMoveComponent->ypos);

		//Remember coordinates before movement
		currentXpos = playerMoveComponent->xpos;
		currentYpos = playerMoveComponent->ypos;
		//Release w
		playerInputComponent->w = false;
		systemManager.update();
		//then player should be on same X and same Y
		REQUIRE(currentXpos == playerMoveComponent->xpos);
		REQUIRE(currentYpos == playerMoveComponent->ypos);

		entityManager.remove(playerId);
	}
}