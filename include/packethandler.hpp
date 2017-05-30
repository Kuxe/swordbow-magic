#ifndef PACKET_HANDLER_HPP
#define PACKET_HANDLER_HPP

#include "logger.hpp"
#include "messagetypes.hpp"

/** A PacketHandlar is someone who is able to poll packets out of the PacketManager **/
struct IPacket;
struct PacketHandler {
	virtual void greet(IPacket* packet) = 0;
	virtual void handle(const UnknownData* data) {
		Logger::warning("PacketHandler got packet UnknownData without overriden method to handle it");
	}
	
	virtual void handle(const OutdatedData* data) {
		Logger::warning("PacketHandler got packet OutdatedData without overriden method to handle it");
	}
	
	virtual void handle(const ConnectToServerData* data) {
		Logger::warning("PacketHandler got packet ConnectToServerData without overriden method to handle it");
	}
	
	virtual void handle(const ServerReplyToConnectData* data) {
		Logger::warning("PacketHandler got packet ServerReplyToConnectData without overriden method to handle it");
	}
	
	virtual void handle(const DisconnectData* data) {
		Logger::warning("PacketHandler got packet DisconnectData without overriden method to handle it");
	}
	
	virtual void handle(const InputDataData* data) {
		Logger::warning("PacketHandler got packet InputDataData without overriden method to handle it");
	}
	
	virtual void handle(const BeginTransmittingInitialComponentsData * data) {
		Logger::warning("PacketHandler got packet BeginTransmittingInitialComponentsData without overriden method to handle it");
	}
	
	virtual void handle(const InitialComponentsData* data) {
		Logger::warning("PacketHandler got packet InitialComponentsData without overriden method to handle it");
	}
	
	virtual void handle(const EndTransmittingInitialComponentsData * data) {
		Logger::warning("PacketHandler got packet EndTransmittingInitialComponentsData without overriden method to handle it");
	}
	
	virtual void handle(const MoveComponentsDiffData* data) {
		Logger::warning("PacketHandler got packet MoveComponentsDiffData without overriden method to handle it");
	}
	
	virtual void handle(const RenderComponentsDiffData* data) {
		Logger::warning("PacketHandler got packet RenderComponentsDiffData without overriden method to handle it");
	}
	
	virtual void handle(const PlaySoundData * data) {
		Logger::warning("PacketHandler got packet PlaySoundData without overriden method to handle it");
	}
	
	virtual void handle(const RegisterIdToSystemData* data) {
		Logger::warning("PacketHandler got packet RegisterIdToSystemData without overriden method to handle it");
	}
	
	virtual void handle(const RemoveIdData * data) {
		Logger::warning("PacketHandler got packet RemoveIdData without overriden method to handle it");
	}
	
	virtual void handle(const RemoveIdFromSystemData* data) {
		Logger::warning("PacketHandler got packet RemoveIdFromSystemData without overriden method to handle it");
	}
	
	virtual void handle(const RemoveIdFromSystemsData * data) {
		Logger::warning("PacketHandler got packet RemoveIdFromSystemsData without overriden method to handle it");
	}
	
	virtual void handle(const ActivateIdData* data) {
		Logger::warning("PacketHandler got packet ActivateIdData without overriden method to handle it");
	}
	
	virtual void handle(const CongestedClientData* data) {
		Logger::warning("PacketHandler got packet CongestedClientData without overriden method to handle it");
	}
	
	virtual void handle(const NotCongestedClientData* data) {
		Logger::warning("PacketHandler got packet NotCongestedClientData without overriden method to handle it");
	}
	
	virtual void handle(const KeepAliveData * data) {
		Logger::warning("PacketHandler got packet KeepAliveDatawithout overriden method to handle it");
	}
	
};

#endif //PACKET_HANDLER_HPP