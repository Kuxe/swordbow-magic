#ifndef REMOVESYSTEM_HPP
#define REMOVESYSTEM_HPP

#include "isystem.hpp"
#include <unordered_set>
#include <queue>

typedef unsigned int ID;

class EntityManager;
class RemoveSystem : public ISystem {
private:
    std::unordered_set<ID> ids;
    std::queue<ID> activeIds;
    EntityManager* entityManager;

public:
    RemoveSystem(EntityManager* entityManager);

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    void activateId(ID id);
};

#endif //REMOVALSYSTEM_HPP
