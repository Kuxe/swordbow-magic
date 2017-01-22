#ifndef PACKETMANAGER_HPP
#define PACKETMANAGER_HPP

#include <unordered_map>
#include "ipaddress.hpp"
#include "connection.hpp"
#include "packet.hpp"
#include "socket.hpp"
#include "logger.hpp"
#include <tuple>

/** For portable serialization using cereal **/
#include "cereal/archives/portable_binary.hpp"

/** These are needed for cereal to know how to serialize std containers **/
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/tuple.hpp"
#include "cereal/types/chrono.hpp"
#include "cereal/types/utility.hpp" //for std::pair
#include "cereal/types/queue.hpp"

/** Class responsible for packets
	Resending lost packets, ordering of packtets,
	sequencenumbers of packets
**/
class PacketManager {
private:
	std::unordered_map<IpAddress, Connection> connections;
	std::string protocolName;
	Socket socket;

public:

	PacketManager(std::string protocolName) :
        protocolName(protocolName) { }

	void registerClient();
	void unregisterClient();

	template<typename DataType, MESSAGE_TYPE Message>
	Packet<DataType, Message> deserialize(const char* buffer, const int bytesRead) {
		try {
			std::string serializedPacket(buffer, bytesRead);
			std::istringstream iss(serializedPacket, std::ios::binary);
			cereal::PortableBinaryInputArchive pbia(iss);
		    Packet<DataType, Message> packet;
		    pbia(packet); 	//Crash here because client sends a packet with bool but the datatype of CONNECT-packet is std::pair<...>
		    				//There needs to be two different connect packets "CLIENT_CONNECT_TO_SERVER" and "SERVER_REPLY_TO_CONNECT"
		    return packet;
		} catch (cereal::Exception e) {
			std::ostringstream oss;
			oss << "Could not deserialize packet (" << e.what() << "), bytesRead: " << bytesRead << ", message number: " << Message << ")";
			Logger::error(oss);
		}
	}

	template<MESSAGE_TYPE Message>
	void accept(auto& acceptor, auto message, const char* buffer, int bytesRead, const IpAddress& sender) {
		//So get the data that is actually relevant, copy it to the message, then call the overloaded
		//method of an acceptor (overloaded on message)
		message.data = deserialize<decltype(message.data), Message>(buffer, bytesRead).getData();
		acceptor.accept(message, sender);
	}

	std::string serialize(const auto& object) {
        std::ostringstream oss;
        cereal::PortableBinaryOutputArchive pboa(oss);
        pboa(object);
        return oss.str();
	}

	/** Takes a buffer of data (that has been received on a socket) and the number of bytes read,
		deserializes the buffer into a untyped packet. Check in the untyped packet what MESSAGE_TYPE
		it is, cast the untyped packet into a typed packet and pass the typed packet to the
		packet acceptor (which likely is something like "ClientRunningState", who defines behaviour
		for each packet **/
	void deserializeAndApply(const IpAddress& sender, const char* buffer, const int bytesRead, auto& acceptor) {
		if(bytesRead <= 0) {
			return;
		}

		Logger::verbose("Deserializing and applying packet");

		try {
			//Deserialize contents of buffer into untyped packet
	        std::string serializedPacket(buffer, bytesRead);
			auto untypedPacket = deserialize<bool, MESSAGE_TYPE::UNKNOWN>(buffer, bytesRead);
	        validatePacket(sender, untypedPacket);
	        storePacket(sender, serializedPacket);

	        //1. Check type of packet
	        switch(untypedPacket.getType()) {
	        	case OUTDATED: { 
	        		accept<MESSAGE_TYPE::OUTDATED>(acceptor, OutdatedData(), buffer, bytesRead, sender);
	        	} break;
	        	case CONNECT_TO_SERVER: { 
	        		accept<MESSAGE_TYPE::CONNECT_TO_SERVER>(acceptor, ConnectToServerData(), buffer, bytesRead, sender);
	        	} break;
	        	case SERVER_REPLY_TO_CONNECT: { 
	        		accept<MESSAGE_TYPE::SERVER_REPLY_TO_CONNECT>(acceptor, ServerReplyToConnectData(), buffer, bytesRead, sender);
	        	} break;
	        	case DISCONNECT: { 
	        		accept<MESSAGE_TYPE::DISCONNECT>(acceptor, DisconnectData(), buffer, bytesRead, sender);
	        	} break;
	        	case INPUTDATA: { 
	        		accept<MESSAGE_TYPE::INPUTDATA>(acceptor, InputDataData(), buffer, bytesRead, sender);
	        	} break;
	        	case BEGIN_TRANSMITTING_INITIAL_COMPONENTS: {
	        		accept<MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>(acceptor, BeginTransmittingInitialComponentsData(), buffer, bytesRead, sender);
	        	} break;
	        	case INITIAL_COMPONENTS: { 
	        		accept<MESSAGE_TYPE::INITIAL_COMPONENTS>(acceptor, InitialComponentsData(), buffer, bytesRead, sender);
	        	} break;
	        	case END_TRANSMITTING_INITIAL_COMPONENTS: { 
	        		accept<MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>(acceptor, EndTransmittingInitialComponentsData(), buffer, bytesRead, sender); 
	        	} break;
	        	case MOVECOMPONENTSDIFF: {
	        		accept<MESSAGE_TYPE::MOVECOMPONENTSDIFF>(acceptor, MoveComponentsDiffData(), buffer, bytesRead, sender);
	        	} break;
	        	case RENDERCOMPONENTSDIFF: {
	        		accept<MESSAGE_TYPE::RENDERCOMPONENTSDIFF>(acceptor, RenderComponentsDiffData(), buffer, bytesRead, sender);
	        	} break;
	        	case PLAY_SOUND: {
	        		accept<MESSAGE_TYPE::PLAY_SOUND>(acceptor, PlaySoundData(), buffer, bytesRead, sender);
	        	} break;
	        	case REGISTER_ID_TO_SYSTEM: {
	        		accept<MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>(acceptor, RegisterIdToSystemData(), buffer, bytesRead, sender);
	        	} break;
	        	case REMOVE_ID: {
	        		accept<MESSAGE_TYPE::REMOVE_ID>(acceptor, RemoveIdData(), buffer, bytesRead, sender);
	        	} break;
	        	case REMOVE_ID_FROM_SYSTEM: {
	        		accept<MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>(acceptor, RemoveIdFromSystemData(), buffer, bytesRead, sender);
	        	} break;
	        	case REMOVE_ID_FROM_SYSTEMS: {
	        		accept<MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>(acceptor, RemoveIdFromSystemsData(), buffer, bytesRead, sender);
	        	} break;
	        	case ACTIVATE_ID: {
	        		accept<MESSAGE_TYPE::ACTIVATE_ID>(acceptor, ActivateIdData(), buffer, bytesRead, sender);
	        	} break;
	        	case CONGESTED_CLIENT: {
	        		accept<MESSAGE_TYPE::CONGESTED_CLIENT>(acceptor, CongestedClientData(), buffer, bytesRead, sender);
	        	} break;
	        	case NOT_CONGESTED_CLIENT: {
	        		accept<MESSAGE_TYPE::NOT_CONGESTED_CLIENT>(acceptor, NotCongestedClientData(), buffer, bytesRead, sender);
	        	} break;
	        	case KEEP_ALIVE: {
	        		accept<MESSAGE_TYPE::KEEP_ALIVE>(acceptor, KeepAliveData(), buffer, bytesRead, sender);
	        	} break;
	        }
        } catch(cereal::Exception e) {
        	std::ostringstream oss;
        	oss << "Could not deserialize and apply packet (" << e.what() << ")";
			Logger::error(oss);
		}
	}

	template<class T, MESSAGE_TYPE messageType>
	bool send(const IpAddress& ipAddress, const Packet<T, messageType>& packet) {
		//Save serialized packet on ipaddress
		const auto serializedPacket = serialize(packet);
        connections[ipAddress].rawPackets[packet.getSequence()] = serializedPacket;

		//Send serialized packet to the ipaddress via socket
		socket.send(ipAddress, serializedPacket);
	}

	/** Receives a packet, calls deserializeAndApply which takes care of applying the content of packet
		This method will likely be the *major* work done by the receive-thread on server and client **/
	void receive(auto& acceptor) {
		IpAddress sender;
		int bytesRead = 0;
		const char* buffer = socket.receive(sender, bytesRead);
		deserializeAndApply(sender, buffer, bytesRead, acceptor);
	}

	void validatePacket(const IpAddress& sender, const auto& untypedPacket) {
		auto protocol = untypedPacket.getProtocol();
		auto sequence = untypedPacket.getSequence();

		/**

		if(protocol == stringhash(protocolName)) {

			//Each sender has it's own remoteSequenceNumber
			const auto& remoteSequence = connections[sender].remoteSequence;

			//Only accept recent messages
			constexpr uint16_t MAX = 65535;
			const bool recent =
			(sequence - remoteSequence && sequence - remoteSequence < MAX/2) ||
			(remoteSequence > sequence && remoteSequence - sequence < MAX/2);

			if(recent) {
				connections[sender].remoteSequence = sequence;

			//TODO: Figure out what 'ack(knowledge)' is and implement congestion control
			//TODO: Implement congestion avoidance

			} else {
				type = MESSAGE_TYPE::OUTDATED; //0 for outdated
				}
		} else {
			Logger::log(
				"Got packet with illegal protocol " + std::to_string(protocol) +
				", expected " + std::to_string(stringhash(protocolName)) +
				"(" + protocolName + ")\n"
				);
		}

		*/
	}

	void storePacket(const IpAddress& sender, const std::string& serialziedPacket) {
		//TODO: Implement
	}

	bool open(const unsigned short port) {
		return socket.open(port);
	}

	void close() {
		socket.close();
	}

	//Should be called if same machine restarts a client, since that
    //client will start producing sequencenumbers starting on 0, 1, 2 ...
    //but the server, without calling this method, would compare 0, 1, 2 etc
    //towards an already present, possibly larger, number within
    //remoteSequnceNumbers which would result in deeming the newest packet
    //as an old packet, which is unwanted behaviour..
    void resetRemoteSequenceNumber(const IpAddress& ipAddress) {
        connections[ipAddress].remoteSequence = 0;
    }

};

#endif //PACKETMANAGER_HPP