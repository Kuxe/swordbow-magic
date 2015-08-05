#include "soundengine.hpp"
#include <iostream>
#include <chrono>
#include <string>

using std::cout;
using std::endl;

SoundEngine::SoundEngine() {
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        cout << "ERROR: Couldn't initialize SDL_audio (" << SDL_GetError() << ")! I don't know what will happen!" << endl;
    }
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 ) {
        cout << "ERROR: Couldn't initialize SDL_mixer " << Mix_GetError() << ")! I dont know what will happen!" << endl;
    }

    /** Sound paths are relative to the executable. If executable is here:
        /someFolder/<the executable is here>

        Then the resources are assumed to be in:
        /someFolder/resources/sounds/<image files here>

        If this isn't adhered to, sounds won't be found and segfaults will be inbound
    **/
    const std::string resourcePath("../resources/sounds/");
    std::pair<Music::Identifier, std::string> musicPairs [] {
        {Music::NATURE_SOUNDS, resourcePath + "naturesounds.ogg"}
    };

    //Load musics and store them in music map
    for(auto pair : musicPairs) {
        auto music = Mix_LoadMUS(pair.second.c_str());
        if(!music) cout << "ERROR: Couldn't load music " << pair.second << " (" << Mix_GetError() << ")" << endl;
        musics.insert({pair.first, music});
    }

    //Check for pusleaudio audio-driver and warn about it
    if(strcmp(SDL_GetCurrentAudioDriver(), "pulseaudio") == 0) {
        cout << "WARNING: " << SDL_GetCurrentAudioDriver() << " detected! Make sure to run this program with ALSA instead, otherwise sounds might break!";
        cout << " This might make shutdown of program VERY slow!" << endl;
    }

    //Load sounds and store them in sound map
    const std::pair<Sound::Identifier, std::string> soundPairs[] {
        {Sound::WALKING, resourcePath + "walking.wav"},
        {Sound::HURT, resourcePath + "hurt.wav"},
        {Sound::BLOODSPLATTER, resourcePath + "bloodsplatter.wav"}
    };
    unsigned char channelNumber = 0; //Will overflow if number of channels > 255

    for(auto pair : soundPairs) {
        auto sound = Mix_LoadWAV(pair.second.c_str());
        if(!sound) cout << "ERROR: Couldn't load sound " << pair.second << " (" << Mix_GetError() << ")" << endl;
        sounds.insert({pair.first, sound});
        channels.insert({sound, channelNumber++});
    }

    auto soundPathsSize = sizeof(soundPairs) / sizeof(std::pair<Sound::Identifier, const char*>);
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

void SoundEngine::playMusic(Music::Identifier music) const {
    Mix_PlayMusic(musics.at(music), -1);
}

void SoundEngine::playSound(SoundData& scSound) const {
    auto sound = sounds.at(scSound.sound); //Soundfile on soundPath
    auto channel = channels.at(sound); //Channel that the soundfile is played on
    Mix_PlayChannel(channel, sound, 0);
}

void SoundEngine::stopSound(SoundData& sound) const {
    Mix_HaltChannel(channels.at(sounds.at(sound.sound)));
}
