#include "soundsystem.hpp"
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;

SoundSystem::SoundSystem() {
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        cout << "ERROR: Couldn't initialize SDL_audio (" << SDL_GetError() << ")! I don't know what will happen!" << endl;
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 ) {
        cout << "ERROR: Couldn't initialize SDL_mixer " << Mix_GetError() << ")! I dont know what will happen!" << endl;
    }

    //Load musics and store them in music map
    for(auto musicPath : {
            "./resources/sounds/naturesounds.ogg",
        }) {
        auto music = Mix_LoadMUS(musicPath);
        if(!music) cout << "ERROR: Couldn't load music " << musicPath << " (" << Mix_GetError() << ")" << endl;
        musics.insert(make_pair(musicPath, music));
    }

    //Check for pusleaudio audio-driver and warn about it
    if(strcmp(SDL_GetCurrentAudioDriver(), "pulseaudio") == 0) {
        cout << "WARNING: " << SDL_GetCurrentAudioDriver() << " detected! Make sure to run this program with ALSA instead, otherwise sounds might break!";
        cout << " This might make shutdown of program VERY slow!" << endl;
    }

    //Load sounds and store them in sound map
    const string soundPaths[] {
        "./resources/sounds/walking.wav",
        "./resources/sounds/hurt.wav",
    };
    unsigned char channelNumber = 0; //Will overflow if number of channels > 255

    for(auto soundPath : soundPaths) {
        auto sound = Mix_LoadWAV(soundPath.c_str());
        if(!sound) cout << "ERROR: Couldn't load sound " << soundPath << " (" << Mix_GetError() << ")" << endl;
        sounds.insert(make_pair(soundPath, sound));
        channels.insert(make_pair(sound, channelNumber++));
    }

    auto soundPathsSize = sizeof(soundPaths) / sizeof(string);
    Mix_AllocateChannels(soundPathsSize); //One channel per sound

}

SoundSystem::~SoundSystem() {

    //Free all musics
    for(auto music : musics) {
        Mix_FreeMusic(get<1>(music));
    }

    //Free all sounds
    for(auto sound : sounds) {
        Mix_FreeChunk(get<1>(sound));
    }

    //Deallocate all channels
    Mix_AllocateChannels(0);

    Mix_CloseAudio();
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

void SoundSystem::playSound(SoundComponent::Sound& scSound) const {
    auto sound = sounds.at(scSound.path); //Soundfile on soundPath
    auto channel = channels.at(sound); //Channel that the soundfile is played on

    //Before playing sound make sure it is 'allowed' as in checking that
    //some amount of ms have passed (to prevent playing 1000 sounds / frame
    //while walking)

    //chrono is nasty
    if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - scSound.startTime).count() < scSound.duration) {
        return; //Sound hasn't finished playing just yet
    }

    scSound.startTime = std::chrono::high_resolution_clock::now();
    Mix_PlayChannel(channel, sound, 0);
}

void SoundSystem::stopSound(SoundComponent::Sound& sound) const {
    Mix_HaltChannel(channels.at(sounds.at(sound.path)));
}
