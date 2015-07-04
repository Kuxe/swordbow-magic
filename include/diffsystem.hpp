#ifndef DIFFSYSTEM_HPP
#define DIFFSYSTEM_HPP

#include "isystem.hpp"
#include <queue>

typedef unsigned int ID;

class DiffSystem : public ISystem {
    std::queue<ID> ids;
public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
};

#endif //DIFFSYSTEM_HPP
