#ifndef ACTIVATEID_H

#include "icommand.hpp"
#include "systemidentifiers.hpp"
#include "isystem.hpp"

class SystemManager;
typedef unsigned int ID;

/**
 *
 *  Activates an id within a system
 *  This class should be used in conjunction with
 *  OnMoveEventComponent, populating its list.
 *  This allows for different entities to activate
 *  different systems. Just create a OnMoveEventComponent
 *  and fill it's list with different commands. Whenever
 *  something invokes onMoveEvent() for a particular entity
 *  all those commands will get invoked. Populate the list
 *  within OnMoveEventComponent during entity creation.
 *
 **/
class ActivateId : public ICommand {
private:
    const System::Identifier system;
    const ID id;
    SystemManager* const systemManager;

public:
    ActivateId(ID id, System::Identifier system, SystemManager* const systemManager) :
        system(system),
        id(id),
        systemManager(systemManager) { }

    void execute() {
        systemManager->getSystem(system)->activateId(id);
    }
};

#endif //ACTIVATEID_H
