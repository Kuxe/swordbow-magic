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

	void accept(const OutdatedData& data, const IpAddress& sender);
	void accept(InitialComponentsData& data, const IpAddress& sender);
	void accept(const EndTransmittingInitialComponentsData&, const IpAddress& sender);
	void accept(const KeepAliveData&, const IpAddress& sender);
	void accept(const auto& data, const IpAddress& sender);

	void onEvent(const KeyEvent& evt);
	void onEvent(const MouseEvent& evt);
};

#endif //CLIENTRECEIVEINITIALSTATE_HPP