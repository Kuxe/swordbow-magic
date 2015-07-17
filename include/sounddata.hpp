#ifndef SOUNDDATA_HPP
#define SOUNDDATA_HPP

#include <string>
#include <chrono>
#include "soundidentifiers.hpp"

struct SoundData {
    Sound::Identifier sound;
    std::chrono::high_resolution_clock::time_point startTime;
    unsigned int duration = 1000; //in milliseconds

    SoundData(Sound::Identifier sound = Sound::UNDEFINED) : sound(sound) {}

    template<class Archive>
    void serialize(Archive& ar) {
        ar(sound, startTime, duration);
    }
};

#endif //SOUNDDATA_HPP
