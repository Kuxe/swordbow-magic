#ifndef ACTIVATEIDONCLIENTS_HPP
#define ACTIVATEIDONCLIENTS_HPP

#include "icommand.hpp"
#include "client.hpp"
#include "messagetypes.hpp"
#include "packet.hpp"

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
    const System::Identifier system;
    const ID id;
    Socket* socket;
    std::unordered_map<unsigned int, ID>* clients;

public:
    ActivateIdOnClients(
            ID id,
            System::Identifier system,
            Socket* socket,
            std::unordered_map<unsigned int, ID>* clients) :
        system(system),
        id(id),
        socket(socket),
        clients(clients) { }

    void execute() {
        for(auto it : *clients) {
            constexpr unsigned short port = 47294;
            const std::pair<ID, System::Identifier> data {id, system};
            auto packet = Packet<std::pair<ID, System::Identifier>> {
        		stringhash("swordbow-magic"),
        		MESSAGE_TYPE::ACTIVATE_ID,
        		data,
        		sizeof(data)
        	};
        	socket->send({it.first, port}, &packet, sizeof(packet));
        }
    }
};

#endif //ACTIVATEIDONCLIENTS_HPP
