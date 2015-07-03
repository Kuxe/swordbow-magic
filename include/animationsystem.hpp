#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <unordered_set>
#include "isystem.hpp"
#include "animationcomponent.hpp"
#include <unordered_map>

class Client;
class AnimationSystem : public ISystem {
private:
    std::unordered_set<ID> ids;
    std::unordered_map<Client*, ID>* clients;

public:

    AnimationSystem(std::unordered_map<Client*, ID>* clients);

    enum Animation {
        bloodsplatter,
    };

    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
};

#endif //ANIMATIONSYSTEM_HPP
