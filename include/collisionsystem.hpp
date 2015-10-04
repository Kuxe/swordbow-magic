#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "isystem.hpp"
#include <unordered_set>
#include <queue>

class MoveComponent;
class SizeComponent;
class SpatialIndexer;

typedef unsigned int ID;

class CollisionSystem : public ISystem {
private:
	std::unordered_set<ID> ids;
	std::queue<ID> activeIds;
	SpatialIndexer* spatialIndexer;

public:
	CollisionSystem(SpatialIndexer* spatialIndexer);
	void add(ID id);
	void remove(ID id);
	void update();
	unsigned int count() const ;
	const System::Identifier getIdentifier() const;
	bool activateId(ID id);

};

#endif //COLLISIONSYSTEM_H
