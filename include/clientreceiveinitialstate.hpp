#ifndef CLIENTRECEIVEINITIALSTATE_HPP
#define CLIENTRECEIVEINITIALSTATE_HPP

#include "iclientstate.hpp"
#include "messagetypes.hpp"
#include "ipaddress.hpp"

class Client;
class ClientReceiveInitialState : public IClientState {
private:
	Client* client;
	int receivedSmallContainers = 0;
	bool endPacketReceived = false;
public:
	ClientReceiveInitialState(Client* client);
	void step();
	
	void changeState(IClientState* state);
	void onChange(ClientDisconnectedState* state);
	void onChange(ClientReceiveInitialState* state);
	void onChange(ClientRunningState* state);

	std::string name() const override;
	void greet(IPacket* packet) override;
	void handle(const OutdatedData* data) override;
	void handle(const InitialComponentsData* data) override;
	void handle(const EndTransmittingInitialComponentsData*) override;
	void handle(const KeepAliveData*) override;

	void onEvent(const KeyEvent& evt);
	void onEvent(const MouseEvent& evt);
};

#endif //CLIENTRECEIVEINITIALSTATE_HPP