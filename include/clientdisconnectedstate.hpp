#ifndef CLIENTDISCONNECTEDSTATE_HPP
#define CLIENTDISCONNECTEDSTATE_HPP

#include "iclientstate.hpp"
#include "messagetypes.hpp"
#include "ipaddress.hpp"
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

	void accept(const OutdatedData&, const IpAddress& sender);
	void accept(const DisconnectData&, const IpAddress& sender);
	void accept(const BeginTransmittingInitialComponentsData& data, const IpAddress& sender);
	void accept(const KeepAliveData&, const IpAddress& sender);
	void accept(const auto& data, const IpAddress& sender);

	void onEvent(const KeyEvent& evt);
	void onEvent(const MouseEvent& evt);
};


#endif //CLIENTDISCONNECTEDSTATE_HPP