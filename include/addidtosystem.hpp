#ifndef ADDIDTOSYSTEM_H
#define ADDIDTOSYSTEM_H

#include "icommand.hpp"

class AddIdToSystem : public ICommand {
private:
    const string systemIdentifier;
    const ID id;
    SystemManager* const systemManager;

public:
    AddIdToSystem(ID id, const string& systemIdentifier, SystemManager* const systemManager) :
        systemIdentifier(systemIdentifier),
        id(id),
        systemManager(systemManager) { }

    void execute() {
        systemManager->getSystem(systemIdentifier)->add(id);
    }
};

#endif //ADDIDTOSYSTEM_H
