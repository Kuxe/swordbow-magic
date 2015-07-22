#ifndef INPUTSYSTEM_HPP
#define INPUTSYSTEM_HPP

#include "isystem.hpp"
#include <unordered_set>
#include <queue>

class InputSystem : public ISystem {
private:
    std::unordered_set<ID> ids;
    std::queue<ID> activeIds;

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
    const System::Identifier getIdentifier() const;
    void activateId(ID id);
};

#endif //INPUTSYSTEM_HPP
