#include "soundsystem.hpp"
#include <iostream>

using std::cout;
using std::endl;

SoundSystem::SoundSystem() {
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        cout << "ERROR: Couldn't initialize SDL_audio (" << SDL_GetError() << ")! I don't know what will happen!" << endl;
    }
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        cout << "ERROR: Couldn't initialize SDL_mixer " << Mix_GetError() << ")! I dont know what will happen!" << endl;
    }

    //Load musics and store them in music map
    for(auto musicPath : {
            "./resources/sounds/naturesounds.ogg",
        }) {
        auto music = Mix_LoadMUS(musicPath);
        if(!music) cout << "ERROR: Couldn't load music " << musicPath << endl;
        musics.insert(make_pair(musicPath, music));
    }
}

SoundSystem::~SoundSystem() {

    //Free all musics
    for(auto music : musics) {
        Mix_FreeMusic(get<1>(music));
    }

    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SoundSystem::add(ID id) {
    ids.insert(id);
}
void SoundSystem::remove(ID id) {
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
void SoundSystem::activateId(ID id) {

}

void SoundSystem::playMusic(const string& musicPath) const {
    Mix_PlayMusic(musics.at(musicPath), -1);
}
