#ifndef ATTACKSYSTEM_HPP
#define ATTACKSYSTEM_HPP

#include <unordered_set>
#include <unordered_map>
#include <queue>
#include "isystem.hpp"

class HashGridSystem;
class Client;
class AttackSystem : public ISystem {
private:
    std::unordered_set<ID> ids;
    unordered_map<Client*, ID>* clients;
    queue<ID> activeIds;
    HashGridSystem* hashgrid;

public:
    AttackSystem(HashGridSystem* hashGrid, std::unordered_map<Client*, ID>* clients);

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
};



#endif //ATTACKSYSTEM_HPP
