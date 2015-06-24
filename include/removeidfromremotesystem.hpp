#ifndef REMOVEIDFROMREMOTESYSTEM_H
#define REMOVEIDFROMREMOTESYSTEM_H

#include "icommand.hpp"

class RemoveIdFromRemoteSystem : public ICommand {
private:
    const string systemIdentifier;
    const ID id;
    unordered_map<ID, Client*>* clients;

public:
    RemoveIdFromRemoteSystem(ID id, const string& systemIdentifier, unordered_map<ID, Client*>* clients) :
        systemIdentifier(systemIdentifier),
        id(id),
        clients(clients) { }

    void execute() {
        for(auto it : *clients) {
            Client* client = it.first;
            client->removeIdFromSystem(id, systemIdentifier);
        }
    }
};

#endif //REMOVEIDFROMSYSTEM_H
