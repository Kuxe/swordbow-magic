#include "collisionsystem.hpp"
#include "componentmanager.hpp"
#include "movecomponent.hpp"
#include "sizecomponent.hpp"

CollisionSystem::CollisionSystem(SpatialIndexer* spatialIndexer) :
	spatialIndexer(spatialIndexer) {}

void CollisionSystem::add(ID id) {
	ids.insert(id);
	activeIds.push(id);
}

void CollisionSystem::remove(ID id) {
	ids.erase(id);
}

void CollisionSystem::update() {
	while(!activeIds.empty()) {
		auto id = activeIds.front(); activeIds.pop();

		for(auto overlap : spatialIndexer->overlaps(id)) {
			auto& outerMc = componentManager->moveComponents.at(id);
			outerMc.xpos = outerMc.oldXpos;
			outerMc.ypos = outerMc.oldYpos;

			auto& innerMc = componentManager->moveComponents.at(overlap);
			innerMc.xpos = innerMc.oldXpos;
			innerMc.ypos = innerMc.oldYpos;
		}
	}
}

unsigned int CollisionSystem::count() const {
	return ids.size();
}

const string CollisionSystem::getIdentifier() const {
	return "CollisionSystem";
}

void CollisionSystem::activateId(ID id) {
	if(ids.find(id) != ids.end()) {
		activeIds.push(id);
	}
}
