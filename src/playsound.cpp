#include "playsound.hpp"
#include "client.hpp"

PlaySound::PlaySound(
    const SoundComponent::Sound& sound,
    unordered_map<Client*, ID>* clients
    ) :
    sound(sound),
    clients(clients) { }

void PlaySound::execute() {
    for(auto it : *clients) {
        Client* client = it.first;

        //chrono is nasty
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - sound.startTime).count();
        if(elapsed > sound.duration) {
            sound.startTime = std::chrono::high_resolution_clock::now();
            client->playSound(sound);
        }
    }
}
