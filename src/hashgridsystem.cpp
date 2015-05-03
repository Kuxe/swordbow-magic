#include "hashgridsystem.hpp"
#include "componentmanager.hpp"
#include <iostream>
#include "movecomponent.hpp"
#include "sizecomponent.hpp"
#include "rendercomponent.hpp"
#include "rendersystem.hpp"
#include "namecomponent.hpp"
#include "boundingbox.hpp"

using std::cout;
using std::endl;

HashGridSystem::HashGridSystem(
	ComponentManager* componentManager,
	BoundingBox* boundingBox,
	const unsigned int worldWidth,
	const unsigned int worldHeight,
	const unsigned int side) :
		componentManager(componentManager),
		boundingBox(boundingBox),
		side(side),
		width(worldWidth / side + (worldWidth % side != 0)),
		height(worldHeight / side + (worldHeight % side != 0)),
		cellsCapacity((worldWidth / side + (worldWidth % side != 0)) * (worldHeight / side + (worldHeight % side != 0))),
		cells(new unordered_set<ID>[width * height])
		 {
}

HashGridSystem::~HashGridSystem() {
	delete[] cells;
}

void HashGridSystem::add(const ID id) {
	ids.insert(id);
	activeIds.push(id);
}

void HashGridSystem::remove(const ID id) {
	ids.erase(id);
	removeFromCells(id);
}

void HashGridSystem::addToCells(const ID id) {
	const auto bb = boundingBox->getBoundingBox(id);

	//Place ID in all cells which partially or completely contains the ID
	for(unsigned int y = bb.y/side; y <= (bb.y + bb.h)/side; y++) {
		for(unsigned int x = bb.x/side; x <= (bb.x + bb.w)/side; x++) {
			if(y*width + x < cellsCapacity) {
				cells[y*width + x].insert(id);
			} else {
				cout << "WARNING: tried to index out of HashGrid cellsCapacity!" << endl;
			}
		}
	}
}

void HashGridSystem::removeFromCells(const ID id) {
	const auto bb = boundingBox->getBoundingBox(id);

	//Remove ID in all cells which previously contained this ID
	for(unsigned int y = bb.y/side; y <= (bb.y + bb.h)/side; y++) {
		for(unsigned int x = bb.x/side; x <= (bb.x + bb.w)/side; x++) {
			if(cells[y*width + x].erase(id) == 0) {
				/*cout << "ERROR: hashgrid tried to erase id " << *id << " from cell " << y*width + x << " but the ID wasn't there." << endl;
				cout << "Strange things may happen from now on, because the ID could remain in cells ";
				cout << "where the ID isnt physically located anymore (hashgrid got desynced)" << endl;*/
			};
		}
	}
}

void HashGridSystem::removeFromCellsOldBoundingBox(const ID id) {
	const auto bb = boundingBox->getBoundingBox(id);

	//Remove ID in all cells which previously contained this ID
	for(unsigned int y = bb.y/side; y <= (bb.y + bb.h)/side; y++) {
		for(unsigned int x = bb.x/side; x <= (bb.x + bb.w)/side; x++) {
			if(cells[y*width + x].erase(id) == 0) {
				/*cout << "ERROR: hashgrid tried to erase id " << *id << " from cell " << y*width + x << " but the ID wasn't there." << endl;
				cout << "Strange things may happen from now on, because the ID could remain in cells ";
				cout << "where the ID isnt physically located anymore (hashgrid got desynced)" << endl;
				cout << "as of 2015-04-16 this error-message occurs during initialization of gameworld," << endl;
				cout << "in that case you need not to worry about the error message. It happens since" << endl;
				cout << "adding to the HashGridSystem is done BEFORE world.cpp sets appropiate coordinates of each tile" << endl;
				cout << "thus when the tile becomes active it tries to remove itself from {0,0}. This is of course harmless" << endl;
				cout << "and can be ignored" << endl;*/
			};
		}
	}
}

//Returns all entities that are fully or partly contained by id's bounding box
void HashGridSystem::overlaps(unordered_set<ID>& overlappingEntities, const ID id) const {
	query(overlappingEntities, boundingBox->getBoundingBox(id));
	overlappingEntities.erase(id);
}

//Return all entities that are fully or partly contained by a queryArea
void HashGridSystem::query(unordered_set<ID>& queryIds, const Rect& queryArea) const {

	//Loop through all cells in which this ID is partly or fully contained
	for(unsigned int y = queryArea.y/side; y <= (queryArea.y + queryArea.h)/side; y++) {
		for(unsigned int x = queryArea.x/side; x <= (queryArea.x + queryArea.w)/side; x++) {

			//For all ids in the same cells as this one..
			for(auto otherId : cells[y*width + x]) {
				if(intersect(queryArea, boundingBox->getBoundingBox(otherId))) {
					queryIds.insert(otherId);
				}
			}
		}
	}
}

void HashGridSystem::getNearbyIds(unordered_set<ID>& nearbyIds, const ID id) const {

}

void HashGridSystem::getBoundingBox(SpatialIndexer::Rect& bb, ID id) const {
	bb = boundingBox->getBoundingBox(id);
}

void HashGridSystem::activateId(const ID id) {
	//if the id is a member of hashgrid...
	if(ids.find(id) != ids.end()) {
		//then it is legit to make it an active id
		activeIds.push(id);
	}
}

void HashGridSystem::update() {
	while(!activeIds.empty()) {
		auto id = activeIds.front(); activeIds.pop();

		//search cells bounded by old bounding box for id
		//and remove id from those cells
		removeFromCellsOldBoundingBox(id);

		//add id to the cells bounded by current bounding box for id
		addToCells(id);
	}
}

unsigned int HashGridSystem::count() const {
	return ids.size();
}
const string HashGridSystem::getIdentifier() const {
	return boundingBox->getSystemName();
}
