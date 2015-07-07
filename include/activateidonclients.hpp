#ifndef ACTIVATEIDONCLIENTS_HPP
#define ACTIVATEIDONCLIENTS_HPP

#include "icommand.hpp"
#include "client.hpp"

class SystemManager;
typedef unsigned int ID;

/**
 *
 *  Similar to ActivateId but activates
 *  id on all remote clients
 *
 **/
class ActivateIdOnClients : public ICommand {
private:
    const string systemIdentifier;
    const ID id;
    SystemManager* const systemManager;
    std::unordered_map<Client*, ID>* clients;

public:
    ActivateIdOnClients(
            ID id,
            const string& systemIdentifier,
            SystemManager* const systemManager,
            std::unordered_map<Client*, ID>* clients) :
        systemIdentifier(systemIdentifier),
        id(id),
        systemManager(systemManager),
        clients(clients) { }

    void execute() {
        for(auto it : *clients) {
            Client* client = it.first;
            //TODO: Figure out how to activate ids on clients
            //client->activateId(id, systemIdentifier);
        }
    }
};

#endif //ACTIVATEIDONCLIENTS_HPP
