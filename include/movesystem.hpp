#ifndef MOVESYSTEM_H
#define MOVESYSTEM_H

#include "isystem.hpp"
#include <unordered_set>
#include <queue>

using namespace std;

class MoveComponent;
class RenderSystem;

typedef unsigned int ID;

class MoveSystem : public ISystem {
 private:
	unordered_set<ID> ids;
    unordered_set<ID> activeIds;

 public:
	void add(ID id);
	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
    void activateId(ID id);
};

#endif //MOVESYSTEM_H
