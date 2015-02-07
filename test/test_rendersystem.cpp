#include "catch.hpp"
#include "systemmanager.h"
#include "deltatime.h"
#include "componentmanager.h"
#include "idmanager.h"
#include "gridindexer.h"
#include "entitymanager.h"
#include "flagsystem.h"
#include "movesystem.h"
#include "rendersystem.h"
#include "movecomponent.h"
#include "rendercomponent.h"

TEST_CASE("RenderSystem") {
	ComponentManager componentManager;
	DeltaTime deltaTime;
	SystemManager systemManager(
			&componentManager,
			&deltaTime);

	IdManager idManager;
	GridIndexer	gridIndexer(&componentManager);
	EntityManager entityManager(
			&systemManager,
			&componentManager,
			&idManager,
			&gridIndexer);

	FlagSystem flagSystem;
	MoveSystem moveSystem;
	RenderSystem renderSystem(&gridIndexer);

	systemManager.add(&flagSystem);
	systemManager.add(&moveSystem);
	systemManager.add(&renderSystem);

	unsigned long long int* playerId;

	SECTION("Sorting renderDatas") {
		RenderComponent rc1;
		RenderComponent rc2;
		RenderComponent rc3;
		RenderComponent rc4;
		RenderComponent rc5;

		rc1.zindex = 1;
		rc2.zindex = 2;
		rc3.zindex = 3;
		rc4.zindex = 3;
		rc5.zindex = 5;

		const unsigned int SIZE = 5;
		RenderData renderDatas[SIZE] {
			RenderData{&rc2, nullptr},
			RenderData{&rc1, nullptr},
			RenderData{&rc5, nullptr},
			RenderData{&rc4, nullptr},
			RenderData{&rc3, nullptr},	
		};
		renderSystem.sort(renderDatas, SIZE);

		REQUIRE(renderDatas[0].renderComponent->zindex == 1);
		REQUIRE(renderDatas[1].renderComponent->zindex == 2);
		REQUIRE(renderDatas[2].renderComponent->zindex == 3);
		REQUIRE(renderDatas[3].renderComponent->zindex == 3);
		REQUIRE(renderDatas[4].renderComponent->zindex == 5);
	}




}
