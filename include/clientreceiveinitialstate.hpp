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

	void handle(IPacket* data) override;
	void handle(const OutdatedData* data);
	void handle(InitialComponentsData* data);
	void handle(const EndTransmittingInitialComponentsData*);
	void handle(const KeepAliveData*);

	void onEvent(const KeyEvent& evt);
	void onEvent(const MouseEvent& evt);
};

#endif //CLIENTRECEIVEINITIALSTATE_HPP