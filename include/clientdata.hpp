#ifndef CLIENTDATA_HPP
#define CLIENTDATA_HPP

#include "timer.hpp"

typedef unsigned int ID;

//Intended as value in unordered_maps where key is the IpAddress
struct ClientData {
    uint16_t sequence;
    ID id;
    bool congested = false; 
    Timer congestionTimer;
};

#endif //CLIENTDATA_HPP
