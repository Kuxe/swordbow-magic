#ifndef ATTACKSYSTEM_HPP
#define ATTACKSYSTEM_HPP

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "isystem.hpp"

class HashGridSystem;
class Client;
class Socket;
class AttackSystem : public ISystem {
private:
    std::unordered_set<ID> ids;
    std::unordered_map<unsigned int, ID>* clients;
    std::queue<ID> activeIds;
    HashGridSystem* hashgrid;
    Socket* socket;

public:
    AttackSystem(
        HashGridSystem* hashGrid,
        std::unordered_map<unsigned int, ID>* clients,
        Socket* socket);

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System getIdentifier() const;
    void activateId(ID id);
};



#endif //ATTACKSYSTEM_HPP
