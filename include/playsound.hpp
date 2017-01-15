#ifndef PLAYSOUND_HPP
#define PLAYSOUND_HPP

#include "icommand.hpp"
#include "sounddata.hpp"
#include <unordered_map>

/** For network **/
#include "ipaddress.hpp"
#include "clientdata.hpp"

typedef unsigned int ID;

class PacketManager;

//FIXME: Weird compilation where compiler nags about "PlaySoundA" is not a class.. which it isn't, but in source code I've never refered to PlaySoundA so it's strange.

/** Used when server should tell all clients to play a sound **/
class PlaySound : public ICommand {
/*private:
    SoundData sound;
    std::unordered_map<IpAddress, ClientData>* clients;
    PacketManager* packetManager;
public:
    PlaySound(
        SoundData sound,
        std::unordered_map<IpAddress, ClientData>* clients,
        PacketManager* packetManager
    );
    void execute();*/
};

#endif //PLAYSOUND_HPP
