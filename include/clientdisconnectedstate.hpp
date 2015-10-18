#ifndef CLIENTDISCONNECTEDSTATE_HPP
#define CLIENTDISCONNECTEDSTATE_HPP

#include "iclientstate.hpp"

class Client;
class ClientDisconnectedState : public IClientState {
private:
	Client* client;
public:
	ClientDisconnectedState(Client* client);
	void receive();
	void step();
};


#endif //CLIENTDISCONNECTEDSTATE_HPP