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

	void changeState(IClientState* state);
	void onChange(ClientDisconnectedState* state);
	void onChange(ClientReceiveInitialState* state);
	void onChange(ClientRunningState* state);
};


#endif //CLIENTDISCONNECTEDSTATE_HPP