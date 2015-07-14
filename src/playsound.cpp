#include "playsound.hpp"
#include "client.hpp"
#include "socket.hpp"
#include "messagetypes.hpp"
#include "packet.hpp"

PlaySound::PlaySound(
    SoundComponent::SoundData sound,
    unordered_map<unsigned int, ID>* clients,
    Socket* socket
    ) :
    sound(sound),
    clients(clients),
    socket(socket) { }

void PlaySound::execute() {
    for(auto it : *clients) {

        //chrono is nasty
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - sound.startTime).count();
        if(elapsed > sound.duration) {
            sound.startTime = std::chrono::high_resolution_clock::now();
            constexpr unsigned short port = 47294;
            using Type = Packet<SoundComponent::SoundData>;
            auto packet = Type {
                stringhash("swordbow-magic"),
                MESSAGE_TYPE::PLAY_SOUND,
                sound,
                sizeof(sound)
            };
            socket->send<Type>({it.first, port}, packet);
        }
    }
}
