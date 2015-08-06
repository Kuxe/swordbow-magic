#ifndef ACTIVATEIDONCLIENTS_HPP
#define ACTIVATEIDONCLIENTS_HPP

#include "icommand.hpp"
#include "clientdata.hpp"
#include "systemidentifiers.hpp"
#include "messagetypes.hpp"
#include "packet.hpp"
#include "socket.hpp"

/** For logging **/
#include "logger.hpp"

class SystemManager;
typedef unsigned int ID;

/**
 *
 *  Similar to ActivateId but activates
 *  id on all remote clients. Obsolete class
 *  since activation of ids are implicit upon
 *  receiving a packet
 *
 **/
class ActivateIdOnClients : public ICommand {
private:
    const System::Identifier system;
    const ID id;
    Socket* socket;
    std::unordered_map<IpAddress, ClientData>* clients;

public:
    ActivateIdOnClients(
            ID id,
            System::Identifier system,
            Socket* socket,
            std::unordered_map<IpAddress, ClientData>* clients) :
        system(system),
        id(id),
        socket(socket),
        clients(clients) { }

    void execute() {
        Logger::log("Called obsolete ActivateIdOnClients.execute()", Log::WARNING);
        for(auto pair : *clients) {
            const std::pair<ID, System::Identifier> data {id, system};
            auto& client = pair.second;

            using Type = Packet<std::tuple<ID, System::Identifier>>;
            auto packet = Type {
        		stringhash("swordbow-magic"),
                client.sequence++,
        		MESSAGE_TYPE::ACTIVATE_ID,
        		data,
        		sizeof(data)
        	};
        	socket->send<Type>(pair.first, packet);
        }
    }
};

#endif //ACTIVATEIDONCLIENTS_HPP
