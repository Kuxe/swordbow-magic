#include "playsound.hpp"

/** For network **/
#include "clientdata.hpp"
#include "packetmanager.hpp"
#include "messagetypes.hpp"
#include "packet.hpp"

PlaySound::PlaySound(
    SoundData sound,
    std::unordered_map<IpAddress, ClientData>* clients,
    PacketManager* packetManager
    ) :
    sound(sound),
    clients(clients),
    packetManager(packetManager) { }

void PlaySound::execute() {

    const auto now = std::chrono::high_resolution_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - sound.startTime).count();
    if(elapsed > sound.duration) {
        sound.startTime = std::chrono::high_resolution_clock::now();

        for(auto& pair : *clients) {
            auto& clientData = pair.second;
            const auto packet = Packet<SoundData, MESSAGE_TYPE::PLAY_SOUND> {
                stringhash("swordbow-magic"),
                clientData.sequence++,
                sound,
                sizeof(sound)
            };
            packetManager->send(pair.first, packet);
        }
    }
}
