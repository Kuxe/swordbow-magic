#ifndef CLIENTRUNNINGSTATE_HPP
#define CLIENTRUNNINGSTATE_HPP

#include "iclientstate.hpp"
#include "messagetypes.hpp"
#include "ipaddress.hpp"

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

	void accept(const OutdatedData& data, const IpAddress& sender);
	void accept(const ServerReplyToConnectData& data, const IpAddress& sender);
	void accept(MoveComponentsDiffData& data, const IpAddress& sender);
	void accept(RenderComponentsDiffData& data, const IpAddress& sender);
	void accept(PlaySoundData& data, const IpAddress& sender);
	void accept(const RegisterIdToSystemData& data, const IpAddress& sender);
	void accept(const RemoveIdData& data, const IpAddress& sender);
	void accept(const RemoveIdFromSystemData& data, const IpAddress& sender);
	void accept(const RemoveIdFromSystemsData& data, const IpAddress& sender);
	void accept(const ActivateIdData& data, const IpAddress& sender);
	void accept(const KeepAliveData& data, const IpAddress& sender);
	void accept(const auto& data, const IpAddress& sender);
};

#endif //CLIENTRUNNINGSTATE_HPP