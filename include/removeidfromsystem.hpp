#ifndef REMOVEIDFROMSYSTEM_H
#define REMOVEIDFROMSYSTEM_H

#include "icommand.hpp"
#include "systemidentifiers.hpp"

class RemoveIdFromSystem : public ICommand {
private:
    const System::Identifier system;
    const ID id;
    SystemManager* const systemManager;

public:
    RemoveIdFromSystem(ID id, System::Identifier system, SystemManager* const systemManager) :
        system(system),
        id(id),
        systemManager(systemManager) { }

    void execute() {
        systemManager->getSystem(system)->remove(id);
    }
};

#endif //REMOVEIDFROMSYSTEM_H
