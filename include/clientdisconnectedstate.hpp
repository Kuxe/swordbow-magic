#ifndef CLIENTDISCONNECTEDSTATE_HPP
#define CLIENTDISCONNECTEDSTATE_HPP

#include "iclientstate.hpp"
#include <mutex>

class Client;
class ClientDisconnectedState : public IClientState {
private:
	Client* client;
	std::mutex forceDisconnectMutex;
	volatile bool forceDisconnectBool = false;
	void forceDisconnect();
	void pollForceDisconnect();
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