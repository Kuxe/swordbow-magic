#ifndef HEALTHSYSTEM_HPP
#define HEALTHSYSTEM_HPP

#include <unordered_set>
#include "isystem.hpp"

typedef unsigned int ID;

class HealthSystem : public ISystem {
private:
    std::unordered_set<ID> ids;

public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    void activateId(ID id);
};


#endif //HEALTHSYSTEM_HPP
