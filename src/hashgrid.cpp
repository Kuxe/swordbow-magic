#include "hashgrid.h"
#include "componentmanager.h"
#include <iostream>
#include "movecomponent.h"
#include "sizecomponent.h"
#include "rendercomponent.h"
#include "rendersystem.h"

using std::cout;
using std::endl;

HashGrid::HashGrid(ComponentManager* componentManager, unsigned int width, unsigned int height, unsigned int side) :
		componentManager(componentManager),
		width(width < 1 ? 1 : width),
		height(height < 1 ? 1 : height),
		side(side < 1 ? 1 : side) {

	if(width < 1) {
		cout << "width in HashGrid constructor cannot be < 1. Forcing width = 1" << endl;	
	}
	if(height < 1) {
		cout << "height in HashGrid constructor cannot be < 1. Forcing height = 1"<< endl;	
	}

	if(side < 1) {
		cout << "side in HashGrid constructor cannot be < 1. Forcing side = 1" << endl;
	}

	if(width % side != 0) {
		cout << "HashGrid CTOR: width(" << this->width << ") isnt divisible by side(" << this->side << "). Crash/glitches incoming!" << endl;
	}
	if(height % side != 0) {
		cout << "HashGrid CTOR: height(" << this->height << ") isnt divisible by side(" << this->side << "). Crash/glitches incoming!" << endl;
	}

	//I feel clever for remembering this-> here. Maybe having
	//same param names as member names is a bad idea... 
	cellsCapacity = this->width/this->side * this->height/side;
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

	//Place ID in all cells which partially or completely contains the ID
	for(unsigned int x = mc->xpos/side; x <= (mc->xpos + sc->width)/side; x++) {
		for(unsigned int y = mc->ypos/side; y <= (mc->ypos + sc->height)/side; y++) {
			cells[y*(width/side) + x].insert(id);	
		}
	}
}

void HashGrid::clear() {
	for(unsigned int i = 0; i < cellsCapacity; i++) {
		cells[i].clear();
	}
}

void HashGrid::overlaps(unordered_set<ID>& overlappingEntities, const ID id) {
	const auto& mc = componentManager->moveComponents.at(id);
	const auto& sc = componentManager->sizeComponents.at(id);
	const auto& rc = componentManager->renderComponents.at(id);

	//Loop through all cells in which this ID is partly or fully contained
	for(unsigned int x = mc->xpos/side; x <= (mc->xpos + sc->width)/side; x++) {
		for(unsigned int y = mc->ypos/side; y <= (mc->ypos + sc->height)/side; y++) {
			
			//For all ids in the same cells as this one...
			for(auto otherId : cells[y*width/side + x]) {
				const auto& omc = componentManager->moveComponents.at(otherId);
				const auto& osc = componentManager->sizeComponents.at(otherId);
				const auto& orc = componentManager->renderComponents.at(otherId);

				//check if their image overlap with the image of id	
				if(intersect(	SpatialIndexer::Rect{
									mc->xpos + rc->xoffset,
									mc->ypos + rc->yoffset,
									rc->textureData.width,
									rc->textureData.height}, 
								SpatialIndexer::Rect{
									omc->xpos + orc->xoffset,
									omc->ypos + orc->yoffset,
									orc->textureData.width,
									orc->textureData.height}
							)
					) {
					overlappingEntities.insert(otherId);
				}
			}	
		}
	}
}

void HashGrid::query(unordered_set<ID>& queryIds, Rect queryArea) {

}

void HashGrid::getNearbyIds(unordered_set<ID>& nearbyIds, const ID id) {

}

void HashGrid::update() {
	clear();
	for(auto id : ids) {
		addToCells(id);
	}	
}



