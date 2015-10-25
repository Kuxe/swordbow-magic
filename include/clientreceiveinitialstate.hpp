#ifndef CLIENTRECEIVEINITIALSTATE_HPP
#define CLIENTRECEIVEINITIALSTATE_HPP

#include "iclientstate.hpp"

class Client;
class ClientReceiveInitialState : public IClientState {
private:
	Client* client;
	int receivedSmallContainers = 0;
	bool endPacketReceived = false;
public:
	ClientReceiveInitialState(Client* client);
	void receive();
	void step();
	
	void changeState(IClientState* state);
	void onChange(ClientDisconnectedState* state);
	void onChange(ClientReceiveInitialState* state);
	void onChange(ClientRunningState* state);
};

#endif //CLIENTRECEIVEINITIALSTATE_HPP