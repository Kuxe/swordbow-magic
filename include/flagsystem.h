#ifndef FLAGSYSTEM_H
#define FLAGSYSTEM_H

#include "isystem.h"
#include <unordered_map>

class FlagComponent;

struct FlagData {
	FlagComponent* flagComponent;
};

class FlagSystem : public ISystem {
 private:
 	unordered_map<unsigned long long int*, FlagData> flagDatas;
 public:
	void add(unsigned long long int* id);
	void remove(unsigned long long int* id);
	void update();
	unsigned int count() const;
	const string getIdentifier() const;
};


#endif //FLAGSYSTEM_H