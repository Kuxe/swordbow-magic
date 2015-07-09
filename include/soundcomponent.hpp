#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H

#include <string>
#include <chrono>
#include "soundidentifiers.hpp"

//This class should *maybe* be split into different components
//Not all entities that make sounds can play walk sounds,
//Trees don't walk. With this component, trees could play
//walking-sounds. Trees don't need another string for walking.
//Waste of memory. Sufficient for now.
struct SoundComponent {
    struct SoundData {
        Sound::Identifier sound;
        std::chrono::high_resolution_clock::time_point startTime;
        unsigned int duration = 1000; //in milliseconds

        SoundData(Sound::Identifier sound = Sound::UNDEFINED) : sound(sound) {}
    };

    //Paths to different sounds
    SoundData walk;
};

#endif //SOUNDCOMPONENT_H
