#ifndef ISYSTEM_H
#define ISYSTEM_H

#include <string>

using namespace std;

class ComponentManager;
class DeltaTime;
class SystemManager;

//Not entierly an interface. It's an abstract class.
class ISystem {
 public:
 	DeltaTime* deltaTime;
 	ComponentManager* componentManager;

    //Some systems could alter or change how an entity behaves in another system
    //Example: MoveSystem should be able to say:
    //"if this entity is a member of RenderSystem, then this entity should also be rendered since it moved"
    SystemManager* systemManager;
 	virtual void add(unsigned long long int* id) = 0;
 	virtual void remove(unsigned long long int* id) = 0;
	virtual void update() = 0;
	virtual unsigned int count() const = 0; //How many entities are present within the system?
	virtual const string getIdentifier() const = 0; //Naming convention is "RenderSystem", "MoveSystem" etc..
    virtual void activateId(unsigned long long int* id) = 0;
    bool active = true;
};

#endif //ISYSTEM_H
