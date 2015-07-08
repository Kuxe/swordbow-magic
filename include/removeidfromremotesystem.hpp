#ifndef REMOVEIDFROMREMOTESYSTEM_H
#define REMOVEIDFROMREMOTESYSTEM_H

#include "icommand.hpp"
#include "systemidentifiers.hpp"

class RemoveIdFromRemoteSystem : public ICommand {
private:
    const System system;
    const ID id;
    unordered_map<ID, Client*>* clients;

public:
    RemoveIdFromRemoteSystem(ID id, System system, unordered_map<ID, Client*>* clients) :
        system(system),
        id(id),
        clients(clients) { }

    void execute() {
        for(auto it : *clients) {
            it.first->removeIdFromSystem(id, system);
        }
    }
};

#endif //REMOVEIDFROMSYSTEM_H
