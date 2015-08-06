#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <unordered_set>
#include "isystem.hpp"

class AnimationSystem : public ISystem {
private:
    std::unordered_set<ID> ids;

public:

    enum Animation {
        bloodsplatter,
    };

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const System::Identifier getIdentifier() const;
    void activateId(ID id);
};

#endif //ANIMATIONSYSTEM_HPP
