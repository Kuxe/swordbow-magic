#include "playsound.hpp"

/** For network **/
#include "clientdata.hpp"
#include "socket.hpp"
#include "messagetypes.hpp"
#include "packet.hpp"

PlaySound::PlaySound(
    SoundData sound,
    std::unordered_map<IpAddress, ClientData>* clients,
    Socket* socket
    ) :
    sound(sound),
    clients(clients),
    socket(socket) { }

void PlaySound::execute() {
    for(auto& pair : *clients) {
        auto& clientData = pair.second;

        //chrono is nasty
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - sound.startTime).count();
        if(elapsed > sound.duration) {
            sound.startTime = std::chrono::high_resolution_clock::now();
            using Type = Packet<SoundData>;
            auto packet = Type {
                stringhash("swordbow-magic"),
                clientData.sequence++,
                MESSAGE_TYPE::PLAY_SOUND,
                sound,
                sizeof(sound)
            };
            socket->send<Type>(pair.first, packet);
        }
    }
}
