#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <string>

using namespace std;

class ComponentManager;

class ISystem {
 public:
	virtual void update() = 0;
	virtual const string getIdentifier() const = 0; //Naming convention is "RenderSystem", "MoveSystem" etc..
};

#endif //ISYSTEM_H