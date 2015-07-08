#ifndef ANIMATIONCOMPONENT_HPP
#define ANIMATIONCOMPONENT_HPP

#include <vector>
#include <chrono>
#include "imageidentifiers.hpp"

using std::vector;

class AnimationComponent {
public:
    class Animation {
    public:
        vector<Image> frames;
        unsigned char currentFrame = 0;
        std::chrono::high_resolution_clock::time_point startTime;
        unsigned short duration = 150; //duration per frame in milliseconds
        bool looping = true;
        bool firstLoop = true;
    };

    //Groups animations in different directions together
    class AnimationGroup {
    public:
        Animation north;
        Animation northEast;
        Animation east;
        Animation southEast;
        Animation south;
        Animation southWest;
        Animation west;
        Animation northWest;
    };

    AnimationGroup idle;
    AnimationGroup walking;
    Animation bloodsplatter;

    Animation* state;
};

#endif //ANIMATIONCOMPONENT_HPP
