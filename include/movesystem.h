#ifndef MOVESYSTEM_H
#define MOVESYSTEM_H

#include "isystem.h"
#include <unordered_map>

using namespace std;

class MoveComponent;
class InputComponent;

struct MoveData {
	MoveComponent* moveComponent;
	InputComponent* inputComponent;
};

class MoveSystem : public ISystem {
 private:
	unordered_map<unsigned long long int*, MoveData> moveDatas;
 public:
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const;
	const string getIdentifier() const;
};

#endif //MOVESYSTEM_H