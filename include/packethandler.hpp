#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

#include "logger.hpp"
#include "messagetypes.hpp"

/** A PacketHandlar is someone who is able to poll packets out of the PacketManager **/
struct IPacket;
struct PacketHandler {

	virtual void greet(IPacket* packet) = 0;

	void handle(auto* data) {
		std::ostringstream oss;
		/** Each possible packet must have virtual void handle(T* data) declared here with empty body such that
			any concrete PacketHandler actually uses their own implementations of handle for the given data **/ 
		oss << "PacketHandler received data for which no overload exists. Did you forget to declare a virtual overloaded method for typeid: " << typeid(decltype(data)).name() << "?";
		Logger::warning(oss);
	}

	virtual void handle(ConnectToServerData* data) { }; //This is what I must do for all packets in messagetypes.hpp
};

#endif //PACKET_HANDLER_HPP