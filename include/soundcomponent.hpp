#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H

#include "sounddata.hpp"

//This class should *maybe* be split into different components
//Not all entities that make sounds can play walk sounds,
//Trees don't walk. With this component, trees could play
//walking-sounds. Trees don't need another string for walking.
//Waste of memory. Sufficient for now.
struct SoundComponent {
    //Paths to different sounds
    SoundData walk;
};

#endif //SOUNDCOMPONENT_H
