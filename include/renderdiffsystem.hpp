#ifndef RENDERDIFFSYSTEM_HPP
#define RENDERDIFFSYSTEM_HPP

#include "isystem.hpp"
#include <unordered_set>

typedef unsigned int ID;

class RenderDiffSystem : public ISystem {
    std::unordered_set<ID> ids;
public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    void activateId(ID id);
    void clear();

    auto begin() { return ids.begin(); }
    auto end() { return ids.end(); }
};

#endif //RENDERDIFFSYSTEM_HPP
