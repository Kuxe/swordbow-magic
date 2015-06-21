#ifndef SERVERNETWORKSYSTEM_HPP
#define SERVERNETWORKSYSTEM_HPP

#include "isystem.hpp"

typedef unsigned int ID;

class ServerNetworkSystem : public ISystem {
private:
public:
    ServerNetworkSystem();
    void add(ID id);
    void remove(ID id);
    void update();
    unsigned int count() const;
    const std::string getIdentifier() const;
    void activateId(ID id);
};


#endif //SERVERNETWORKSYSTEM
