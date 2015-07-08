#ifndef HEALTHSYSTEM_HPP
#define HEALTHSYSTEM_HPP

#include <unordered_set>
#include "isystem.hpp"

using std::unordered_set;

typedef unsigned int ID;

class HealthSystem : public ISystem {
private:
    unordered_set<ID> ids;

public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System getIdentifier() const;
    void activateId(ID id);
};


#endif //HEALTHSYSTEM_HPP
