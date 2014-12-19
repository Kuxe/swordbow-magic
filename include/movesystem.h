#ifndef MOVESYSTEM_H
#define MOVESYSTEM_H

#include "isystem.h"
#include <unordered_map>

using namespace std;

class MoveComponent;

struct MoveData {
	MoveComponent* moveComponent;
};

class MoveSystem : public ISystem {
 private:
	unordered_map<unsigned long long int*, MoveData> moveDatas;
 public:
	void add(unsigned long long int* id, MoveData data);
	void remove(unsigned long long int* id);
	void update();
	const string getIdentifier() const;
};

#endif //MOVESYSTEM_H