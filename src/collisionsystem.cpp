#include "collisionsystem.hpp"
#include "componentmanager.hpp"
#include "spatialindexer.hpp"
#include "movecomponent.hpp"

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
		//TODO: Convert from 2D to 3D
		/*for(auto overlap : spatialIndexer->overlaps(id)) {
			auto& outerMc = componentManager->moveComponents.at(id);
			outerMc.pos.x = outerMc.oldPos.x;
			outerMc.pos.y = outerMc.oldPos.y;

			auto& innerMc = componentManager->moveComponents.at(overlap);
			innerMc.pos.x = innerMc.oldPos.x;
			innerMc.pos.y = innerMc.oldPos.y;
		} */
	}
}

unsigned int CollisionSystem::count() const {
	return ids.size();
}

const System::Identifier CollisionSystem::getIdentifier() const {
	return System::COLLISION;
}

bool CollisionSystem::activateId(ID id) {
	if(ids.find(id) != ids.end()) {
		activeIds.push(id);
		return true;
	}
	return false;
}
