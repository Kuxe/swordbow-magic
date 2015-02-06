#include "catch.hpp"
#include "gridindexer.h"
#include "componentmanager.h"
#include "movecomponent.h"

TEST_CASE("GridIndexer") {
	ComponentManager componentManager;
	GridIndexer gridIndexer(&componentManager);

	//Create an entity with coordinates
	unsigned long long int id = 3;
	auto moveComponent = componentManager.createMoveComponent(&id);
	auto sizeComponent = componentManager.createSizeComponent(&id);

	unsigned long long int otherId = 1337;
	auto otherMoveComponent = componentManager.createMoveComponent(&otherId);
	auto otherSizeComponent = componentManager.createSizeComponent(&otherId);

	SECTION("Initialization") {
		//Shouldn't be any element in the gridindex from start
		REQUIRE(gridIndexer.ids.size() == 0);
		REQUIRE(gridIndexer.cellIndicies.size() == 0);
	}

	SECTION("Add") {
		//Adding an entity with pos (0, 0) should put 1 id in cell[0][0]
		gridIndexer.add(&id);
		REQUIRE(gridIndexer.ids.size() == 1);
		REQUIRE(gridIndexer.cells[0][0].ids.size() == 1);
		//Make sure the id got indexed (that it exists in cellindicies)
		REQUIRE(gridIndexer.cellIndicies.count(&id) > 0);
	}

	SECTION("Remove") {
		//Adding an entity with pos (0, 0) should put 1 id in cell[0][0]
		gridIndexer.add(&id);
		
		gridIndexer.remove(&id);
		//Shouldn't be any element in the gridindexer
		REQUIRE(gridIndexer.ids.size() == 0);
		//Should not remember this id cellindices anymore
		REQUIRE(gridIndexer.cellIndicies.size() == 0);
		//The cell shouldnt contain this id anymore
		REQUIRE(gridIndexer.cells[0][0].ids.size() == 0);
	}

	SECTION("Index") {
		//Adding an entity with pos (0, 0) should put 1 id in cell[0][0]
		gridIndexer.add(&id);
		
		moveComponent->xpos = 49;
		moveComponent->ypos = 80;
		//(49, 80) should put the entity in (39/20, 80/20) = (2, 4)
		//and removed
		gridIndexer.index(&id);
		REQUIRE(gridIndexer.cells[2][4].ids.size() == 1);
		REQUIRE(gridIndexer.cells[0][0].ids.size() == 0);

		//Make sure re-indexing on unchanged-id doesnt change anything
		gridIndexer.index(&id);
		REQUIRE(gridIndexer.cells[2][4].ids.size() == 1);

		//Make sure a cell can handle two (and therefore, probably, several) ids
		gridIndexer.add(&otherId);
		REQUIRE(gridIndexer.cells[2][4].ids.size() == 1);
		REQUIRE(gridIndexer.cells[0][0].ids.size() == 1);
		REQUIRE(gridIndexer.ids.size() == 2);

		//move two ids simultaneously
		moveComponent->xpos = 30;
		moveComponent->ypos = 60;
		otherMoveComponent->xpos = 85;
		otherMoveComponent->ypos = 5;
		gridIndexer.index(&id);
		gridIndexer.index(&otherId);

		REQUIRE(gridIndexer.cells[2][4].ids.size() == 0);
		REQUIRE(gridIndexer.cells[0][0].ids.size() == 0);
		REQUIRE(gridIndexer.cells[1][3].ids.size() == 1);
		REQUIRE(gridIndexer.cells[4][0].ids.size() == 1);
		REQUIRE(gridIndexer.ids.size() == 2);
	}

	SECTION("GetCell") {
		//Adding an entity with pos (0, 0) should put 1 id in cell[0][0]
		gridIndexer.add(&id);
		REQUIRE(gridIndexer.getCell(0, 0).size() == 1);
		//Should be 1 reference to id
		REQUIRE(gridIndexer.getCell(0, 0).count(&id) == 1);
	}

	SECTION("Get overlapping ids") {
		gridIndexer.add(&id);
		gridIndexer.add(&otherId);

		moveComponent->xpos = 30;
		moveComponent->ypos = 30;

		//entity is on cell 1, 1, so return ids in cells within [0, 0] [2, 2]
		//For now only there's only one other id
		//otherentity on [0, 0]
		otherMoveComponent->xpos = 0;
		otherMoveComponent->ypos = 0;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [0, 1]
		otherMoveComponent->xpos = 0;
		otherMoveComponent->ypos = 30;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [0, 2]
		otherMoveComponent->xpos = 0;
		otherMoveComponent->ypos = 50;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [1, 0]
		otherMoveComponent->xpos = 30;
		otherMoveComponent->ypos = 0;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [1, 1]
		otherMoveComponent->xpos = 30;
		otherMoveComponent->ypos = 30;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [1, 2]
		otherMoveComponent->xpos = 30;
		otherMoveComponent->ypos = 50;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [2, 0]
		otherMoveComponent->xpos = 50;
		otherMoveComponent->ypos = 0;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [2, 1]
		otherMoveComponent->xpos = 50;
		otherMoveComponent->ypos = 30;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);

		//otherentity on [2, 2]
		otherMoveComponent->xpos = 50;
		otherMoveComponent->ypos = 50;
		gridIndexer.update();
		REQUIRE(gridIndexer.getOverlappingIds(&id).size() == 1);
	}
}