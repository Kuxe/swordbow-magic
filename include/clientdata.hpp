#ifndef CLIENTDATA_HPP
#define CLIENTDATA_HPP

#include "ipaddress.hpp"

typedef unsigned int ID;

//Intended as value in unordered_maps where key is the IpAddress
struct ClientData {
    uint16_t sequence;
    ID id;
};

#endif //CLIENTDATA_HPP
