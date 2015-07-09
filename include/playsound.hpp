#ifndef PLAYSOUND_HPP
#define PLAYSOUND_HPP

#include "icommand.hpp"
#include "soundcomponent.hpp"
#include <unordered_map>

typedef unsigned int ID;

class Client;
class Socket;

/** Used when server should tell all clients to play a sound **/
class PlaySound : public ICommand {
private:
    SoundComponent::SoundData sound;
    std::unordered_map<unsigned int, ID>* clients;
    Socket* socket;
public:
    PlaySound(
        SoundComponent::SoundData sound,
        std::unordered_map<unsigned int, ID>* clients,
        Socket* socket
    );
    void execute();
};

#endif //PLAYSOUND_HPP
