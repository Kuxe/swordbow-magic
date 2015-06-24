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
        client->playSound(sound);
    }
}
