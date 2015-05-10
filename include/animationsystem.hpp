#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <unordered_set>
#include "isystem.hpp"

using std::unordered_set;

class AnimationSystem : public ISystem {
private:
    unordered_set<ID> ids;

public:
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
};

#endif //ANIMATIONSYSTEM_HPP