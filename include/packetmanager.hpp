#ifndef PACKETMANAGER_HPP

#include <unordered_map>
#include "ipaddress.hpp"
#include "connection.hpp"
#include "packet.hpp"
#include "ipacketacceptor.hpp"
#include "socket.hpp"
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

	template<typename Data, MESSAGE_TYPE Message>
	Packet<Data, Message> deserialize(const char* buffer, const int bytesRead) {
		try {
			std::string serializedPacket(static_cast<const char*>(buffer), bytesRead);
			std::istringstream iss(serializedPacket);
			cereal::PortableBinaryInputArchive pbia(iss);
		    Packet<Data, Message> packet;
		    pbia(packet);
		    return packet;
		} catch (cereal::Exception e) {
			std::ostringstream oss;
			oss << "Could not deserialize packet (" << e.what() << ", typeid of Data: " << typeid(Data).name() << ", message number: " << Message << ")";
			Logger::log(oss, Log::ERROR);
		}
	}

	template<typename Data, MESSAGE_TYPE Message>
	void accept(auto& acceptor, const char* buffer, int bytesRead, const IpAddress& sender) {
		auto typedPacket = deserialize<Data, Message>(buffer, bytesRead);
		try {
			acceptor.accept(typedPacket, sender);
		} catch (std::exception e) {
			std::ostringstream oss;
			oss << "Could not accept packet (" << e.what() << ")";
			Logger::log(oss, Log::ERROR);
		}
	}

	template<class T>
	std::string serialize(const T& object) {
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

		try {
			//Deserialize contents of buffer into untyped packet
	        std::string serializedPacket(buffer, bytesRead);
			auto untypedPacket = deserialize<char, MESSAGE_TYPE::UNKNOWN>(buffer, bytesRead);
	        validatePacket(sender, untypedPacket);
	        storePacket(sender, serializedPacket);

	        //1. Check type of packet
	        switch(untypedPacket.getType()) {
	        	case OUTDATED: {
	        		accept<OUTDATED_TYPE, MESSAGE_TYPE::OUTDATED>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case CONNECT: {
	        		accept<CONNECT_TYPE, MESSAGE_TYPE::CONNECT>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case DISCONNECT: {
	        		accept<DISCONNECT_TYPE, MESSAGE_TYPE::DISCONNECT>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case INPUTDATA: {
	        		accept<INPUTDATA_TYPE, MESSAGE_TYPE::INPUTDATA>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case BEGIN_TRANSMITTING_INITIAL_COMPONENTS: {
	        		accept<BEGIN_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case INITIAL_COMPONENTS: {
	        		accept<INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::INITIAL_COMPONENTS>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case END_TRANSMITTING_INITIAL_COMPONENTS: {
	        		accept<END_TRANSMITTING_INITIAL_COMPONENTS_TYPE, MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case MOVECOMPONENTSDIFF: {
	        		accept<MOVECOMPONENTSDIFF_TYPE, MESSAGE_TYPE::MOVECOMPONENTSDIFF>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case RENDERCOMPONENTSDIFF: {
	        		accept<RENDERCOMPONENTSDIFF_TYPE, MESSAGE_TYPE::RENDERCOMPONENTSDIFF>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case PLAY_SOUND: {
	        		accept<PLAY_SOUND_TYPE, MESSAGE_TYPE::PLAY_SOUND>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case REGISTER_ID_TO_SYSTEM: {
	        		accept<REGISTER_ID_TO_SYSTEM_TYPE, MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case REMOVE_ID: {
	        		accept<REMOVE_ID_TYPE, MESSAGE_TYPE::REMOVE_ID>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case REMOVE_ID_FROM_SYSTEM: {
	        		accept<REMOVE_ID_FROM_SYSTEM_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case REMOVE_ID_FROM_SYSTEMS: {
	        		accept<REMOVE_ID_FROM_SYSTEMS_TYPE, MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case ACTIVATE_ID: {
	        		accept<ACTIVATE_ID_TYPE, MESSAGE_TYPE::ACTIVATE_ID>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case CONGESTED_CLIENT: {
	        		accept<CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::CONGESTED_CLIENT>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case NOT_CONGESTED_CLIENT: {
	        		accept<NOT_CONGESTED_CLIENT_TYPE, MESSAGE_TYPE::NOT_CONGESTED_CLIENT>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        	case KEEP_ALIVE: {
	        		accept<KEEP_ALIVE_TYPE, MESSAGE_TYPE::KEEP_ALIVE>(acceptor, buffer, bytesRead, sender);
	        	} break;
	        }
        } catch(cereal::Exception e) {
        	std::ostringstream oss;
        	oss << "Could not deserialize and apply packet (" << e.what() << ")";
			Logger::log(oss, Log::ERROR);
		}
	}

	template<class T, MESSAGE_TYPE messageType>
	bool send(const IpAddress& ipAddress, const Packet<T, messageType>& packet) {
		//Serialize the packet
        std::ostringstream oss;
        cereal::PortableBinaryOutputArchive pboa(oss);
        pboa(packet);

		//Save serialized packet on ipaddress
		const std::string serializedPacket = oss.str();
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