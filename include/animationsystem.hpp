#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

#include <unordered_set>
#include "isystem.hpp"
#include "animationcomponent.hpp"
#include <unordered_map>

class Client;
class Socket;
class AnimationSystem : public ISystem {
private:
    std::unordered_set<ID> ids;
    std::unordered_map<unsigned int, ID>* clients;
    Socket* socket;

public:

    AnimationSystem(std::unordered_map<unsigned int, ID>* clients, Socket* socket);

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
