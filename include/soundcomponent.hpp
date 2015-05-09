#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H

#include <string>
#include <chrono>

using std::string;

//This class should *maybe* be split into different components
//Not all entities that make sounds can play walk sounds,
//Trees don't walk. With this component, trees could play
//walking-sounds. Trees don't need another string for walking.
//Waste of memory. Sufficient for now.
class SoundComponent {
public:
    class Sound {
    public:
        string path;
        std::chrono::high_resolution_clock::time_point startTime;
        unsigned int duration = 1000; //in milliseconds
    };

    //Paths to different sounds
    Sound walk;
};

#endif //SOUNDCOMPONENT_H
