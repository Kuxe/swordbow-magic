#ifndef MOVEDIFFSYSTEM_HPP
#define MOVEDIFFSYSTEM_HPP

#include "isystem.hpp"
#include <queue>

typedef unsigned int ID;

class MoveDiffSystem : public ISystem {
    std::queue<ID> ids;
public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    void activateId(ID id);
};

#endif //MOVEDIFFSYSTEM_HPP
