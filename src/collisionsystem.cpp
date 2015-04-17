#include "collisionsystem.h"
#include "componentmanager.h"
#include "movecomponent.h"
#include "sizecomponent.h"

CollisionSystem::CollisionSystem(SpatialIndexer* spatialIndexer) :
	spatialIndexer(spatialIndexer) {}

void CollisionSystem::add(unsigned long long int* id) {
	ids.insert(id);
}

void CollisionSystem::remove(unsigned long long int* id) {
	ids.erase(id);
}

void CollisionSystem::update() {
	while(!activeIds.empty()) {
		auto id = activeIds.front(); activeIds.pop();

		unordered_set<ID> overlappingIds;
		spatialIndexer->overlaps(overlappingIds, id);
		for(auto overlap : overlappingIds) {
			auto outerMc = componentManager->moveComponents.at(id);
			outerMc->xpos = outerMc->oldXpos;
			outerMc->ypos = outerMc->oldYpos;

			auto innerMc = componentManager->moveComponents.at(overlap);
			innerMc->xpos = innerMc->oldXpos;
			innerMc->ypos = innerMc->oldYpos;
		}
	}
}

unsigned int CollisionSystem::count() const {
	return ids.size();
}

const string CollisionSystem::getIdentifier() const {
	return "CollisionSystem";
}

void CollisionSystem::makeIdActive(ID id) {
	if(ids.find(id) != ids.end()) {
		activeIds.push(id);
	}
}
