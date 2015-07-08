#ifndef REMOVEIDFROMSYSTEM_H
#define REMOVEIDFROMSYSTEM_H

#include "icommand.hpp"
#include "systemidentifiers.hpp"

class RemoveIdFromSystem : public ICommand {
private:
    const System system;
    const ID id;
    SystemManager* const systemManager;

public:
    RemoveIdFromSystem(ID id, System system, SystemManager* const systemManager) :
        system(system),
        id(id),
        systemManager(systemManager) { }

    void execute() {
        systemManager->getSystem(system)->remove(id);
    }
};

#endif //REMOVEIDFROMSYSTEM_H
