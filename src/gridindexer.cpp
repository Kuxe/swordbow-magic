#include "gridindexer.h"
#include "movecomponent.h"
#include "sizecomponent.h"
#include "componentmanager.h"
#include <iostream>

using namespace std;

GridIndexer::GridIndexer(ComponentManager* componentManager)
	: componentManager(componentManager) {

}

void GridIndexer::add(unsigned long long int* id) {
	ids.insert(id);

	const auto moveComponent = componentManager->moveComponents.at(id);

	const unsigned char cellx = moveComponent->xpos / CELL_SIDE;
	const unsigned char celly = moveComponent->ypos / CELL_SIDE;

	//Remember cellindex
	cellIndicies.insert(
		make_pair(
			id,
			CellIndex{
				cellx,
				celly
			}
		)
	);

	//Add id to the cell
	cells[cellx][celly].ids.insert(id);

	index(id);
}

void GridIndexer::remove(unsigned long long int* id) {
	const unsigned char cellX = cellIndicies.at(id).xcell;
	const unsigned char cellY = cellIndicies.at(id).ycell;

	//The cell doesnt contain this id anymore...
	cells[cellX][cellY].ids.erase(id);

	//The gridindexer doesnt need to remember previous cellindex for id
	cellIndicies.erase(id);

	//The gridindexer doesnt contain this id anymore...
	ids.erase(id);
}

void GridIndexer::index(unsigned long long int* id) {
	const auto mc = componentManager->moveComponents.at(id);
	const unsigned char oldCellX = cellIndicies.at(id).xcell;
	const unsigned char oldCellY = cellIndicies.at(id).ycell;
	const unsigned char curCellX = mc->xpos / CELL_SIDE;
	const unsigned char curCellY = mc->ypos / CELL_SIDE;

	//If unit has moved to another cell
	if(!(curCellX == oldCellX && curCellY == oldCellY)) {
		//id has moved into another cell, so needs to re-index

		//Remove from id old cell
		cells[oldCellX][oldCellY].ids.erase(id);

		//Add id to new cell
		cells[curCellX][curCellY].ids.insert(id);

		//update cellindicies
		cellIndicies.at(id).xcell = curCellX;
		cellIndicies.at(id).ycell = curCellY;
	}
}

void GridIndexer::update() {
	for(auto id : ids) {
		index(id);
	}
}

set<unsigned long long int*> GridIndexer::getCell(
	const unsigned char x,
	const unsigned char y) const {
	return cells[x][y].ids;
}

set<unsigned long long int*> GridIndexer::getCell(unsigned long long int* id) const {
	const auto mc = componentManager->moveComponents.at(id);
	return getCell(mc->xpos/CELL_SIDE, mc->ypos/CELL_SIDE);
}

//return union of sets of cells withing id area (calc area from movecomponent and sizecomponent)
//If an entity has top-left corner [2, 3] and bottom right corner [4, 5], return all entities in
// [1, 2][5, 6]
set<unsigned long long int*> GridIndexer::getOverlappingIds(unsigned long long int* id) const {
	const auto mc = componentManager->moveComponents.at(id);
	const auto sc = componentManager->sizeComponents.at(id);

	set<unsigned long long int*> overLappingIds;
	const unsigned int fromx = (mc->xpos/CELL_SIDE) - 1;
	const unsigned int tox = ((mc->xpos + sc->width)/CELL_SIDE) + 1;
	const unsigned int fromy = (mc->ypos/CELL_SIDE) - 1;
	const unsigned int toy = (mc->ypos + sc->height)/CELL_SIDE + 1;

	for(int x = fromx; x <= tox; x++) {
		for(int y = fromy; y <= toy; y++) {
			const auto cellIds = getCell(x, y);
			overLappingIds.insert(cellIds.begin(), cellIds.end());
		}
	}

	//Remove self, id isnt considered to be overlapping itself...
	overLappingIds.erase(id);

	return overLappingIds;
}








