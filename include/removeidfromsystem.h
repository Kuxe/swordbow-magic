#ifndef REMOVEIDFROMSYSTEM_H
#define REMOVEIDFROMSYSTEM_H

#include "icommand.h"

class RemoveIdFromSystem : public ICommand {
private:
    const string systemIdentifier;
    const ID id;
    SystemManager* const systemManager;

public:
    RemoveIdFromSystem(ID id, const string& systemIdentifier, SystemManager* const systemManager) :
        systemIdentifier(systemIdentifier),
        id(id),
        systemManager(systemManager) { }

    void execute() {
        systemManager->getSystem(systemIdentifier)->remove(id);
    }
};

#endif //REMOVEIDFROMSYSTEM_H
