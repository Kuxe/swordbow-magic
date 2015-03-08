#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "isystem.h"
#include "dynamicarray.h"

class MoveComponent;
class SizeComponent;

typedef unsigned long long int* ID_TYPE;

struct CollisionData {
	MoveComponent* mc;
	SizeComponent* sc;

	bool operator==(const CollisionData& rhs) {
		return mc == rhs.mc && sc == rhs.sc;
	}
};

class CollisionSystem : public ISystem {
 private:
	 dynamicarray<CollisionData> collisionDatas;
	 bool colliding(const CollisionData& cd1, const CollisionData& cd2) const;

 public:
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const ;
	const string getIdentifier() const;

};

#endif //COLLISIONSYSTEM_H
