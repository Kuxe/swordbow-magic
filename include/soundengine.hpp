#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include "soundcomponent.hpp"

typedef unsigned int ID;

//TODO: Figure out if SoundSystem really should be a system.
class SoundEngine {
private:
    std::unordered_map<string, Mix_Music*> musics;
    std::unordered_map<string, Mix_Chunk*> sounds;
    std::unordered_map<Mix_Chunk*, int> channels;

public:
    SoundEngine();
    ~SoundEngine();
    void playMusic(const string& musicPath) const;
    void playSound(SoundComponent::Sound& scSound) const;
    void stopSound(SoundComponent::Sound& scSound) const;
};

#endif //SOUNDSYSTEM_H
