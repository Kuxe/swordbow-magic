#ifndef CLIENTRECEIVEINITIALSTATE_HPP
#define CLIENTRECEIVEINITIALSTATE_HPP

#include "iclientstate.hpp"

class Client;
class ClientReceiveInitialState : public IClientState {
private:
	Client* client;
	int receivedSmallContainers = 0;
public:
	ClientReceiveInitialState(Client* client);
	void receive();
	void step();
};

#endif //CLIENTRECEIVEINITIALSTATE_HPP