#include "soundsystem.hpp"
#include <iostream>

using std::cout;
using std::endl;

SoundSystem::SoundSystem() {
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        cout << "ERROR: Couldn't initialize SDL_audio (" << SDL_GetError() << ")! I don't know what will happen!" << endl;
    }
}

SoundSystem::~SoundSystem() {
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SoundSystem::add(unsigned long long int* id) {
    ids.insert(id);
}
void SoundSystem::remove(unsigned long long int* id) {
    ids.erase(id);
}
void SoundSystem::update() {

}
unsigned int SoundSystem::count() const {
    return ids.size();
}
const string SoundSystem::getIdentifier() const {
    return "SoundSystem";
}
void SoundSystem::activateId(unsigned long long int* id) {

}
