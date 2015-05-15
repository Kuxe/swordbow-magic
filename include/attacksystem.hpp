#ifndef ATTACKSYSTEM_HPP
#define ATTACKSYSTEM_HPP

#include <unordered_set>
#include <queue>
#include "isystem.hpp"

using std::unordered_set;

class HashGridSystem;
class AttackSystem : public ISystem {
private:
    unordered_set<ID> ids;
    queue<ID> activeIds;
    HashGridSystem* hashgrid;

public:
    AttackSystem(HashGridSystem* hashGrid);

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
};



#endif //ATTACKSYSTEM_HPP
