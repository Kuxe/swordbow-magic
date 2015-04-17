#ifndef MOVESYSTEM_H
#define MOVESYSTEM_H

#include "isystem.h"
#include <unordered_set>

using namespace std;

class MoveComponent;
class InputComponent;
class RenderSystem;

class MoveSystem : public ISystem {
 private:
	unordered_set<unsigned long long int*> ids;
 public:
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const;
	const string getIdentifier() const;
};

#endif //MOVESYSTEM_H
