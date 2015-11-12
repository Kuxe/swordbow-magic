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

/** Used when server should tell all clients to play a sound **/
class PlaySound : public ICommand {
private:
    SoundData sound;
    std::unordered_map<IpAddress, ClientData>* clients;
    PacketManager* packetManager;
public:
    PlaySound(
        SoundData sound,
        std::unordered_map<IpAddress, ClientData>* clients,
        PacketManager* packetManager
    );
    void execute();
};

#endif //PLAYSOUND_HPP
