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
	
	void changeState(IClientState* state);
	void onChange(ClientDisconnectedState* state);
	void onChange(ClientReceiveInitialState* state);
	void onChange(ClientRunningState* state);
};

#endif //CLIENTRUNNINGSTATE_HPP