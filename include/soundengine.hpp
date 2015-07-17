#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <unordered_set>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include "sounddata.hpp"
#include "soundidentifiers.hpp"
#include "musicidentifiers.hpp"

typedef unsigned int ID;

//TODO: Figure out if SoundSystem really should be a system.
class SoundEngine {
private:
    std::unordered_map<Music::Identifier, Mix_Music*, std::hash<int>> musics;
    std::unordered_map<Sound::Identifier, Mix_Chunk*, std::hash<int>> sounds;
    std::unordered_map<Mix_Chunk*, int> channels;

public:
    SoundEngine();
    ~SoundEngine();
    void playMusic(Music::Identifier music) const;
    void playSound(SoundData& scSound) const;
    void stopSound(SoundData& scSound) const;
};

#endif //SOUNDSYSTEM_H
