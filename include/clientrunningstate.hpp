#ifndef CLIENTRUNNINGSTATE_HPP
#define CLIENTRUNNINGSTATE_HPP

#include "iclientstate.hpp"

class Client;
class ClientRunningState : public IClientState {
private:
	Client* client;
public:
	ClientRunningState(Client* client);
	void receive();
	void step();
};

#endif //CLIENTRUNNINGSTATE_HPP