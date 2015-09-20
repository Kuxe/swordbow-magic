#ifndef CONNECTION_HPP
#define CONNECTION_HPP
#include <sstream>
#include <unordered_map>

/** Each socket operates on multiple connections where
	each connection has its own remote sequence number
	This class keeps tracks of what packets are received
	form a single connection (ie server or client)
**/

struct Connection {
	uint16_t remoteSequence;

	//Not deserialized packets associated with their
	//sequence numbers. These packets have been
	//deserialized before getting into this container
	//but the deserialized content isnt stored because
	//I don't know how to store packet of different size
	//in a container without clipping data or using
	//buffers large enough to fit largest packet
	std::unordered_map<uint16_t, std::string> rawPackets;
};

#endif //CONNECTION_HPP