#ifndef IPACKETACCEPTOR_HPP
#define IPACKETACCEPTOR_HPP

#include "messagetypes.hpp"
#include "packet.hpp"
#include "ipaddress.hpp"

class IPacketAcceptor {
public:
	//This is kinda visitor pattern but lazier.
	//Enforces all implementors to specialize behavor for each possible packet
	virtual void accept(Packet<OUTDATED_TYPE, 								MESSAGE_TYPE::OUTDATED>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<CONNECT_TYPE, 								MESSAGE_TYPE::CONNECT>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<DISCONNECT_TYPE,								MESSAGE_TYPE::DISCONNECT>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<INPUTDATA_TYPE,								MESSAGE_TYPE::INPUTDATA>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE,	MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<INITIAL_COMPONENTS_TYPE,						MESSAGE_TYPE::INITIAL_COMPONENTS>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE,	MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<MOVECOMPONENTSDIFF_TYPE,						MESSAGE_TYPE::MOVECOMPONENTSDIFF>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<RENDERCOMPONENTSDIFF_TYPE,					MESSAGE_TYPE::RENDERCOMPONENTSDIFF>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<PLAY_SOUND_TYPE,								MESSAGE_TYPE::PLAY_SOUND>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<REGISTER_ID_TO_SYSTEM_TYPE,					MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<REMOVE_ID_TYPE,								MESSAGE_TYPE::REMOVE_ID>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<REMOVE_ID_FROM_SYSTEM_TYPE, 					MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<REMOVE_ID_FROM_SYSTEMS_TYPE, 				MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<ACTIVATE_ID_TYPE, 							MESSAGE_TYPE::ACTIVATE_ID>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<CONGESTED_CLIENT_TYPE, 						MESSAGE_TYPE::CONGESTED_CLIENT>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<NOT_CONGESTED_CLIENT_TYPE, 					MESSAGE_TYPE::NOT_CONGESTED_CLIENT>& packet, const IpAddress& sender) = 0;
	virtual void accept(Packet<KEEP_ALIVE_TYPE, 							MESSAGE_TYPE::KEEP_ALIVE>& packet, const IpAddress& sender) = 0;
};

#endif //IPACKETACCEPTOR_HPP