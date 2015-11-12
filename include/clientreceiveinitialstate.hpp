#ifndef CLIENTRECEIVEINITIALSTATE_HPP
#define CLIENTRECEIVEINITIALSTATE_HPP

#include "iclientstate.hpp"
#include "ipacketacceptor.hpp"

class Client;
class ClientReceiveInitialState : public IClientState, IPacketAcceptor {
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

	void accept(Packet<OUTDATED_TYPE, 								MESSAGE_TYPE::OUTDATED>& packet, const IpAddress& sender);
	void accept(Packet<CONNECT_TYPE, 								MESSAGE_TYPE::CONNECT>& packet, const IpAddress& sender);
	void accept(Packet<DISCONNECT_TYPE,								MESSAGE_TYPE::DISCONNECT>& packet, const IpAddress& sender);
	void accept(Packet<INPUTDATA_TYPE,								MESSAGE_TYPE::INPUTDATA>& packet, const IpAddress& sender);
	void accept(Packet<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE,	MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender);
	void accept(Packet<INITIAL_COMPONENTS_TYPE,						MESSAGE_TYPE::INITIAL_COMPONENTS>& packet, const IpAddress& sender);
	void accept(Packet<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE,	MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender);
	void accept(Packet<MOVECOMPONENTSDIFF_TYPE,						MESSAGE_TYPE::MOVECOMPONENTSDIFF>& packet, const IpAddress& sender);
	void accept(Packet<RENDERCOMPONENTSDIFF_TYPE,					MESSAGE_TYPE::RENDERCOMPONENTSDIFF>& packet, const IpAddress& sender);
	void accept(Packet<PLAY_SOUND_TYPE,								MESSAGE_TYPE::PLAY_SOUND>& packet, const IpAddress& sender);
	void accept(Packet<REGISTER_ID_TO_SYSTEM_TYPE,					MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>& packet, const IpAddress& sender);
	void accept(Packet<REMOVE_ID_TYPE,								MESSAGE_TYPE::REMOVE_ID>& packet, const IpAddress& sender);
	void accept(Packet<REMOVE_ID_FROM_SYSTEM_TYPE, 					MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>& packet, const IpAddress& sender);
	void accept(Packet<REMOVE_ID_FROM_SYSTEMS_TYPE, 				MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>& packet, const IpAddress& sender);
	void accept(Packet<ACTIVATE_ID_TYPE, 							MESSAGE_TYPE::ACTIVATE_ID>& packet, const IpAddress& sender);
	void accept(Packet<CONGESTED_CLIENT_TYPE, 						MESSAGE_TYPE::CONGESTED_CLIENT>& packet, const IpAddress& sender);
	void accept(Packet<NOT_CONGESTED_CLIENT_TYPE, 					MESSAGE_TYPE::NOT_CONGESTED_CLIENT>& packet, const IpAddress& sender);
	void accept(Packet<KEEP_ALIVE_TYPE, 							MESSAGE_TYPE::KEEP_ALIVE>& packet, const IpAddress& sender);
};

#endif //CLIENTRECEIVEINITIALSTATE_HPP