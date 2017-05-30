#ifndef CLIENTDISCONNECTEDSTATE_HPP
#define CLIENTDISCONNECTEDSTATE_HPP

#include "iclientstate.hpp"
#include "messagetypes.hpp"
#include "ipaddress.hpp"
#include "packethandler.hpp"
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
	void step();

	void changeState(IClientState* state);
	void onChange(ClientDisconnectedState* state);
	void onChange(ClientReceiveInitialState* state);
	void onChange(ClientRunningState* state);

	std::string name() const override;
	void greet(IPacket* packet) override;
	void handle(const OutdatedData*) override;
	void handle(const DisconnectData*) override;
	void handle(const BeginTransmittingInitialComponentsData* data) override;
	void handle(const KeepAliveData*) override;

	void onEvent(const KeyEvent& evt);
	void onEvent(const MouseEvent& evt);
};


#endif //CLIENTDISCONNECTEDSTATE_HPP