#include "hashgrid.h"
#include "componentmanager.h"
#include <iostream>
#include "movecomponent.h"
#include "sizecomponent.h"
#include "rendercomponent.h"
#include "rendersystem.h"

using std::cout;
using std::endl;

HashGrid::HashGrid(ComponentManager* componentManager, unsigned int worldWidth, unsigned int worldHeight, unsigned int side) :
		componentManager(componentManager),
		side(side < 1 ? 1 : side) {

	width = worldWidth / side + (worldWidth % side != 0);
	height = worldHeight / side + (worldHeight % side != 0);

	//I feel clever for remembering this-> here. Maybe having
	//same param names as member names is a bad idea...
	cellsCapacity = width * height;
	cells = new unordered_set<ID>[cellsCapacity];

	for(int i = 0; i < cellsCapacity; i++) {
		cells[i] = unordered_set<ID>();
	}
}

HashGrid::~HashGrid() {
	delete[] cells;
}

void HashGrid::add(const ID id) {
	ids.insert(id);
}

void HashGrid::remove(const ID id) {
	ids.erase(id);
}

void HashGrid::addToCells(const ID id) {
	const auto& mc = componentManager->moveComponents.at(id);
	const auto& sc = componentManager->sizeComponents.at(id);
	const auto& rc = componentManager->renderComponents.at(id);

	unsigned int cellx = (mc->xpos + rc->xoffset)/side;
	unsigned int cellxw = (mc->xpos + rc->xoffset + rc->textureData.width-1)/side;
	unsigned int celly = (mc->ypos + rc->yoffset)/side;
	unsigned int cellyh = (mc->ypos + rc->yoffset + rc->textureData.height-1)/side;

	//Place ID in all cells which partially or completely contains the ID
	for(unsigned int y = celly; y <= cellyh; y++) {
		for(unsigned int x = cellx; x <= cellxw; x++) {
			cells[y*width + x].insert(id);
		}
	}
}

void HashGrid::clear() {
	for(unsigned int i = 0; i < cellsCapacity; i++) {
		cells[i].clear();
	}
}

//Returns all entities that are fully or partly contained by id's texture bounding box
void HashGrid::overlaps(unordered_set<ID>& overlappingEntities, const ID id) {
	const auto& mc = componentManager->moveComponents.at(id);
	const auto& rc = componentManager->renderComponents.at(id);

	const SpatialIndexer::Rect queryRect {
		mc->xpos + rc->xoffset,
		mc->ypos + rc->yoffset,
		rc->textureData.width-1,
		rc->textureData.height-1
	};

	//query using textures bounding box
	query(overlappingEntities, queryRect);

	overlappingEntities.erase(id);
}

//Return all entities that are fully or partly contained by a queryArea
void HashGrid::query(unordered_set<ID>& queryIds, Rect queryArea) {

	//Convert from world-space coordinates to cellCoordinates and store
	//cellCoordinates in cellRect
	const SpatialIndexer::Rect cellRect {
		queryArea.x/side,
		queryArea.y/side,
		queryArea.w/side,
		queryArea.h/side,
	};

	//Loop through all cells in which this ID is partly or fully contained
	for(unsigned int y = cellRect.y; y <= (queryArea.y + queryArea.h)/side; y++) {
		for(unsigned int x = cellRect.x; x <= (queryArea.x + queryArea.w)/side; x++) {

			//For all ids in the same cells as this one..
			for(auto otherId : cells[y*width + x]) {
				const auto& omc = componentManager->moveComponents.at(otherId);
				const auto& osc = componentManager->sizeComponents.at(otherId);
				const auto& orc = componentManager->renderComponents.at(otherId);

				if(intersect(
					queryArea,
					SpatialIndexer::Rect {
						omc->xpos + orc->xoffset,
						omc->ypos + orc->yoffset,
						orc->textureData.width-1,
						orc->textureData.height-1 }
					)
				) {
					queryIds.insert(otherId);
				}
			}
		}
	}
}

void HashGrid::getNearbyIds(unordered_set<ID>& nearbyIds, const ID id) {

}

void HashGrid::update() {
	clear();
	for(auto id : ids) {
		addToCells(id);
	}
}
