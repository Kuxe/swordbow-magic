#ifndef PLAYSOUND_HPP
#define PLAYSOUND_HPP

#include "icommand.hpp"
#include <string>
#include "soundcomponent.hpp"
#include <unordered_map>

typedef unsigned int ID;

class Client;

/** Used when server should tell all clients to play a sound **/
class PlaySound : public ICommand {
private:
    SoundComponent::Sound sound;
    std::unordered_map<Client*, ID>* clients;
public:
    PlaySound(
        const SoundComponent::Sound& soundPath,
        std::unordered_map<Client*, ID>* clients
    );
    void execute();
};

#endif //PLAYSOUND_HPP
