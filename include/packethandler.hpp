#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

/** A PacketHandlar is someone who is able to poll packets out of the PacketManager **/
struct IPacket;
struct PacketHandler {
	virtual void handle(IPacket* packet) = 0;
};

#endif //PACKET_HANDLER_HPP