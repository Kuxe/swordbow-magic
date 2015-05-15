#ifndef ATTACKCOMPONENT_HPP
#define ATTACKCOMPONENT_HPP

#include "spatialindexer.hpp"
#include <chrono>

class AttackComponent {
public:
    SpatialIndexer::Rect impactArea; //Relative to objects position
    int damage = 1;
    std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();
    unsigned int delay = 1000; //In milliseconds

};



#endif //ATTACKCOMPONENT_HPP
