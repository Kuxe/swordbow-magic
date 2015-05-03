#ifndef MOVESYSTEM_H
#define MOVESYSTEM_H

#include "isystem.hpp"
#include <unordered_set>
#include <queue>

using namespace std;

class MoveComponent;
class RenderSystem;

class MoveSystem : public ISystem {
 private:
	unordered_set<unsigned long long int*> ids;
    unordered_set<unsigned long long int*> activeIds;

 public:
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const;
	const string getIdentifier() const;
    void activateId(unsigned long long int* id);
};

#endif //MOVESYSTEM_H
