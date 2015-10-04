#ifndef MOVESYSTEM_H
#define MOVESYSTEM_H

#include "isystem.hpp"
#include <unordered_set>

typedef unsigned int ID;

class MoveSystem : public ISystem {
 private:
    std::unordered_set<ID> ids;
    std::unordered_set<ID> activeIds;

 public:
	void add(ID id);
	void remove(ID id);
	void update();
	unsigned int count() const;
	const System::Identifier getIdentifier() const;
    bool activateId(ID id);
};

#endif //MOVESYSTEM_H
