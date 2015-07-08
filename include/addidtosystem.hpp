#ifndef ADDIDTOSYSTEM_H
#define ADDIDTOSYSTEM_H

#include "icommand.hpp"

class AddIdToSystem : public ICommand {
private:
    const System system;
    const ID id;
    SystemManager* const systemManager;

public:
    AddIdToSystem(ID id, System system, SystemManager* const systemManager) :
        system(system),
        id(id),
        systemManager(systemManager) { }

    void execute() {
        systemManager->getSystem(system)->add(id);
    }
};

#endif //ADDIDTOSYSTEM_H
