#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <unordered_set>
#include "isystem.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>

using std::unordered_set;

typedef unsigned int ID;

class SoundSystem : public ISystem {
private:
    unordered_set<ID> ids;

public:
    SoundSystem();
    ~SoundSystem();
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const string getIdentifier() const;
    void activateId(ID id);
};

#endif //SOUNDSYSTEM_H
