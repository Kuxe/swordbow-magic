#ifndef ISYSTEM_H
#define ISYSTEM_H

#include "systemidentifiers.hpp"

class ComponentManager;
class DeltaTime;
class SystemManager;

typedef unsigned int ID;

//Not entierly an interface. It's an abstract class.
class ISystem {
 public:
 	DeltaTime* deltaTime;
 	ComponentManager* componentManager;

     virtual ~ISystem() { }

    //Some systems could alter or change how an entity behaves in another system
    //Example: MoveSystem should be able to say:
    //"if this entity is a member of RenderSystem, then this entity should also be rendered since it moved"
    SystemManager* systemManager;
 	virtual void add(ID id) = 0;
 	virtual void remove(ID id) = 0;
	virtual void update() = 0;
	virtual unsigned int count() const = 0; //How many entities are present within the system?
	virtual const System::Identifier getIdentifier() const = 0; //Naming convention is "RenderSystem", "MoveSystem" etc..
    virtual bool activateId(ID id) = 0;
    bool active = true;
};

#endif //ISYSTEM_H
