#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <string>

using namespace std;

class ComponentManager;

class ISystem {
 public:
 	ComponentManager* componentManager;
 	virtual void add(unsigned long long int* id) = 0;
 	virtual void remove(unsigned long long int* id) = 0;
	virtual void update() = 0;
	virtual unsigned int count() const = 0; //How many entities are present within the system?
	virtual const string getIdentifier() const = 0; //Naming convention is "RenderSystem", "MoveSystem" etc..
};

#endif //ISYSTEM_H