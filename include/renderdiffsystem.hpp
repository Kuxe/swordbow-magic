#ifndef RENDERDIFFSYSTEM_HPP
#define RENDERDIFFSYSTEM_HPP

#include "isystem.hpp"
#include <queue>

typedef unsigned int ID;

class RenderDiffSystem : public ISystem {
    std::queue<ID> ids;
public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System getIdentifier() const;
    void activateId(ID id);
};

#endif //RENDERDIFFSYSTEM_HPP
