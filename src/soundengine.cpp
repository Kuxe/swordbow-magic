#include "soundengine.hpp"
#include <iostream>
#include <chrono>

using std::cout;
using std::endl;

SoundEngine::SoundEngine() {
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
        musics.insert({musicPath, music});
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
        "./resources/sounds/bloodsplatter.wav"
    };
    unsigned char channelNumber = 0; //Will overflow if number of channels > 255

    for(auto soundPath : soundPaths) {
        auto sound = Mix_LoadWAV(soundPath.c_str());
        if(!sound) cout << "ERROR: Couldn't load sound " << soundPath << " (" << Mix_GetError() << ")" << endl;
        sounds.insert(make_pair(soundPath, sound));
        channels.insert({sound, channelNumber++});
    }

    auto soundPathsSize = sizeof(soundPaths) / sizeof(string);
    Mix_AllocateChannels(soundPathsSize); //One channel per sound

}

SoundEngine::~SoundEngine() {

    //Free all musics
    for(auto music : musics) {
        Mix_FreeMusic(std::get<1>(music));
    }

    //Free all sounds
    for(auto sound : sounds) {
        Mix_FreeChunk(std::get<1>(sound));
    }

    //Deallocate all channels
    Mix_AllocateChannels(0);

    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SoundEngine::playMusic(const string& musicPath) const {
    Mix_PlayMusic(musics.at(musicPath), -1);
}

void SoundEngine::playSound(SoundComponent::Sound& scSound) const {
    auto sound = sounds.at(scSound.path); //Soundfile on soundPath
    auto channel = channels.at(sound); //Channel that the soundfile is played on
    Mix_PlayChannel(channel, sound, 0);
}

void SoundEngine::stopSound(SoundComponent::Sound& sound) const {
    Mix_HaltChannel(channels.at(sounds.at(sound.path)));
}
