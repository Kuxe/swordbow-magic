#ifndef ATTACKSYSTEM_HPP
#define ATTACKSYSTEM_HPP

#include "isystem.hpp"

#include <unordered_set>
#include <unordered_map>
#include <queue>

/** For network **/
#include "clientdata.hpp"
#include "ipaddress.hpp"

class HashGridSystem;
class Socket;
class AttackSystem : public ISystem {
private:
    std::unordered_set<ID> ids;
    std::unordered_map<IpAddress, ClientData>* clients;
    std::queue<ID> activeIds;
    HashGridSystem* hashgrid;
    Socket* socket;

public:
    AttackSystem(
        HashGridSystem* hashGrid,
        std::unordered_map<IpAddress, ClientData>* clients,
        Socket* socket);

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    bool activateId(ID id);
};



#endif //ATTACKSYSTEM_HPP
