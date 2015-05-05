#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <unordered_set>
#include <unordered_map>
#include <string>
#include "isystem.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>

using std::unordered_set;
using std::unordered_map;
using std::string;

typedef unsigned int ID;

class SoundSystem : public ISystem {
private:
    unordered_set<ID> ids;
    unordered_map<string, Mix_Music*> musics;

public:
    SoundSystem();
    ~SoundSystem();
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
    void playMusic(const string& musicPath) const;
};

#endif //SOUNDSYSTEM_H
