#ifndef INPUTSYSTEM_HPP
#define INPUTSYSTEM_HPP

#include "isystem.hpp"
#include <unordered_set>
#include <queue>

using std::unordered_set;
using std::queue;

class InputSystem : public ISystem {
private:
    unordered_set<ID> ids;
    queue<ID> activeIds;

public:
    enum Action {
        WALK_UP,
        WALK_RIGHT,
        WALK_DOWN,
        WALK_LEFT,
        ATTACK,
    };

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
};

#endif //INPUTSYSTEM_HPP
