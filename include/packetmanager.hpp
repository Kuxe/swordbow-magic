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

/** In-house concurrent queue suitable for producer-consumer pattern **/
#include "queue.hpp"

/** Class responsible for packets
	Resending lost packets, ordering of packtets,
	sequencenumbers of packets
**/
class PacketManager {
private:
	std::unordered_map<IpAddress, Connection> connections;
	std::string protocolName;
	Socket socket;
	Queue<std::pair<IpAddress, std::string>> q;
	static constexpr size_t QUEUE_SIZE = 1024;
public:

	PacketManager(std::string protocolName) :
        protocolName(protocolName), q(QUEUE_SIZE) { }

	void registerClient();
	void unregisterClient();

	template<typename DataType, MESSAGE_TYPE Message>
	Packet<DataType, Message> deserialize(const std::string& serializedPacket) {
		try {
			std::istringstream iss(serializedPacket, std::ios::binary);
			cereal::PortableBinaryInputArchive pbia(iss);
		    Packet<DataType, Message> packet;
		    pbia(packet); 	//Crash here because client sends a packet with bool but the datatype of CONNECT-packet is std::pair<...>
		    				//There needs to be two different connect packets "CLIENT_CONNECT_TO_SERVER" and "SERVER_REPLY_TO_CONNECT"
		    return packet;
		} catch (const cereal::Exception& e) {
			std::ostringstream oss;
			oss << "Could not deserialize packet (" << e.what() << "), message number: " << Message << ")";
			Logger::error(oss);
		}
	}

	/** If the deserializedPacket contains one member whose name is data and
		of type IpAddress then copy sender into data otherwise do nothing **/
	void setIpAddressIfPossible(auto* data, const IpAddress& sender) { return; }
	void setIpAddressIfPossible(ContainIP* cip, const IpAddress& sender) {
		Logger::verbose("setIpAddressIfPossible ContainIP called! ITS WORKING!");
		cip->ip = sender;
	}

	template<MESSAGE_TYPE Message>
	void accept(PacketHandler* ph, auto message, const std::string& serializedPacket, const IpAddress& sender) {
		//So get the data that is actually relevant, copy it to the message, then call the overloaded
		//method of an acceptor (overloaded on message)
		auto deserializedPacket = deserialize<decltype(message.data), Message>(serializedPacket);

		//If packet inherits from "ContainIP" then assign "sender" to the packet ip
		setIpAddressIfPossible(&deserializedPacket, sender);

		//FIXME 2017-05-29: ph->greet(&deserializedPacket); will internally call ph.handle(&data) where handle is function defined for auto
		//or with virtual overloaded non-auto argument, so server will treat any packets it get as IPacket unless there is explicit
		//virtual handle(MyPacketData) in PacketHandler (in addition to server actually overriding such a method)
		ph->greet(&deserializedPacket);
	}

	std::string serialize(const auto& object) {
        std::ostringstream oss;
        cereal::PortableBinaryOutputArchive pboa(oss);
        pboa(object);
        return oss.str();
	}

	/** Takes a serialized packet and it's untyped packet. Check in the untyped packet what MESSAGE_TYPE
		it is, cast the untyped packet into a typed packet and pass the typed packet to the
		packet acceptor (which likely is something like "ClientRunningState", who defines behaviour
		for each packet **/
	void apply(PacketHandler* ph, const IpAddress& sender, const std::string& serializedPacket, const Packet<bool, MESSAGE_TYPE::UNKNOWN>& untypedPacket) {
		try {
	        //1. Check type of packet
	        switch(untypedPacket.getType()) {
	        	case OUTDATED: { 
	        		accept<MESSAGE_TYPE::OUTDATED>(ph, OutdatedData(), serializedPacket, sender);
	        	} break;
	        	case CONNECT_TO_SERVER: {
	        		accept<MESSAGE_TYPE::CONNECT_TO_SERVER>(ph, ConnectToServerData(), serializedPacket, sender);
	        	} break;
	        	case SERVER_REPLY_TO_CONNECT: { 
	        		accept<MESSAGE_TYPE::SERVER_REPLY_TO_CONNECT>(ph, ServerReplyToConnectData(), serializedPacket, sender);
	        	} break;
	        	case DISCONNECT: { 
	        		accept<MESSAGE_TYPE::DISCONNECT>(ph, DisconnectData(), serializedPacket, sender);
	        	} break;
	        	case INPUTDATA: { 
	        		accept<MESSAGE_TYPE::INPUTDATA>(ph, InputDataData(), serializedPacket, sender);
	        	} break;
	        	case BEGIN_TRANSMITTING_INITIAL_COMPONENTS: {
	        		accept<MESSAGE_TYPE::BEGIN_TRANSMITTING_INITIAL_COMPONENTS>(ph, BeginTransmittingInitialComponentsData(), serializedPacket, sender);
	        	} break;
	        	case INITIAL_COMPONENTS: { 
	        		accept<MESSAGE_TYPE::INITIAL_COMPONENTS>(ph, InitialComponentsData(), serializedPacket, sender);
	        	} break;
	        	case END_TRANSMITTING_INITIAL_COMPONENTS: { 
	        		accept<MESSAGE_TYPE::END_TRANSMITTING_INITIAL_COMPONENTS>(ph, EndTransmittingInitialComponentsData(), serializedPacket, sender);
	        	} break;
	        	case MOVECOMPONENTSDIFF: {
	        		accept<MESSAGE_TYPE::MOVECOMPONENTSDIFF>(ph, MoveComponentsDiffData(), serializedPacket, sender);
	        	} break;
	        	case RENDERCOMPONENTSDIFF: {
	        		accept<MESSAGE_TYPE::RENDERCOMPONENTSDIFF>(ph, RenderComponentsDiffData(), serializedPacket, sender);
	        	} break;
	        	case PLAY_SOUND: {
	        		accept<MESSAGE_TYPE::PLAY_SOUND>(ph, PlaySoundData(), serializedPacket, sender);
	        	} break;
	        	case REGISTER_ID_TO_SYSTEM: {
	        		accept<MESSAGE_TYPE::REGISTER_ID_TO_SYSTEM>(ph, RegisterIdToSystemData(), serializedPacket, sender);
	        	} break;
	        	case REMOVE_ID: {
	        		accept<MESSAGE_TYPE::REMOVE_ID>(ph, RemoveIdData(), serializedPacket, sender);
	        	} break;
	        	case REMOVE_ID_FROM_SYSTEM: {
	        		accept<MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEM>(ph, RemoveIdFromSystemData(), serializedPacket, sender);
	        	} break;
	        	case REMOVE_ID_FROM_SYSTEMS: {
	        		accept<MESSAGE_TYPE::REMOVE_ID_FROM_SYSTEMS>(ph, RemoveIdFromSystemsData(), serializedPacket, sender);
	        	} break;
	        	case ACTIVATE_ID: {
	        		accept<MESSAGE_TYPE::ACTIVATE_ID>(ph, ActivateIdData(), serializedPacket, sender);
	        	} break;
	        	case CONGESTED_CLIENT: {
	        		accept<MESSAGE_TYPE::CONGESTED_CLIENT>(ph, CongestedClientData(), serializedPacket, sender);
	        	} break;
	        	case NOT_CONGESTED_CLIENT: {
	        		accept<MESSAGE_TYPE::NOT_CONGESTED_CLIENT>(ph, NotCongestedClientData(), serializedPacket, sender);
	        	} break;
	        	case KEEP_ALIVE: {
	        		accept<MESSAGE_TYPE::KEEP_ALIVE>(ph, KeepAliveData(), serializedPacket, sender);
	        	} break;
	        }
        } catch(const cereal::Exception& e) {
        	std::ostringstream oss;
        	oss << "PacketManager::apply(): Could not apply packet (" << e.what() << ")";
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

	/** Check if any packet is received and if so store it (still serialized) in queue **/
	void receive() {
		try {
			IpAddress sender;
			int bytesRead = 0;
			const char* buffer = socket.receive(sender, bytesRead);
			if(bytesRead > 0) {
				q.add({sender, std::string(buffer, bytesRead)});
			}
		} catch(const std::exception& e) {
			Logger::error(std::string("PacketManager::receive() failed with exception: " + std::string(e.what())));
		}
	}

	/** Loop through all received serialized packets, deserialize the packet and give it to the PacketHandler **/
	void poll(PacketHandler* ph) {
		while(!q.empty()) {
			const std::pair<IpAddress, std::string> p = q.take();
			const auto untypedPacket = deserialize<bool, MESSAGE_TYPE::UNKNOWN>(p.second);
			if(validatePacket(p.first, untypedPacket)) {
				apply(ph, p.first, p.second, untypedPacket);
			} else {
				Logger::warning("PacketManager::poll() validation failed");
			}
		}
	}

	bool validatePacket(const IpAddress& sender, const auto& untypedPacket) {
		auto protocol = untypedPacket.getProtocol();
		auto sequence = untypedPacket.getSequence();
		//TODO: Figure out what 'ack(knowledge)' is and implement congestion control
		//TODO: Implement congestion avoidance
		return true;
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