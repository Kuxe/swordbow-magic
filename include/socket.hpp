#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "ipaddress.hpp"
#include "socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h> //close()
#include <iostream>
#include "packet.hpp"
#include <fstream>

/** For portable serialization using cereal **/
#include <cereal/archives/portable_binary.hpp>

/** These are needed for cereal to know how to serialize std containers **/
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/utility.hpp> //for std::pair
#include <cereal/types/queue.hpp>

class Socket {
private:
    int socket = 0;
    unsigned char buffer[256];


public:
    bool open(unsigned short port) {
        //Create a socket and check for failure
        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(socket <= 0) {
            std::cout << "ERROR: Couldn't create socket" << std::endl;
            return false;
        }

        sockaddr_in address;
        address.sin_family = AF_INET; //mumbojumbo
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if(bind(socket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0 )
        {
            std::cout << "ERROR: Couldn't bind socket" << std::endl;
            close();
            return false;
        }

        //Dont wait until a message is recieved
        constexpr int nonBlocking = 1;
        if(fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
            std::cout << "ERORR: Failed to set non-blocking" << std::endl;
            close();
            return false;
        }

        return true;
    }

    void close() {
        ::close(socket);
    }
    bool isOpen() const {
        return socket != 0;
    }

    template<class T>
    bool send(const IpAddress& destination, const T& object) const {
        if(socket == 0) {
            std::cout << "ERROR: Cannot send because socket isn't open" << std::endl;
        }

        //Serialize object
        std::ostringstream oss;
        cereal::PortableBinaryOutputArchive pboa(oss);
        pboa(object);
        const auto& str = oss.str();
        const auto& size = str.length();

        //Create an address from destination address and port...
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(destination.getAddress());
        address.sin_port = htons((unsigned short)destination.getPort());

        //Send to that address
        int sendBytes = sendto(
            socket,
            str.c_str(),    //send serialized data
            size,           //serialized data-length
            0,
            (sockaddr*)&address,
            sizeof(sockaddr_in)
        );

        //Doesn't check if packet succesfully arrived (UDP duh)
        //but if the expected size of sent bytes matches the
        //size of the sent packet
        if(sendBytes != size) {
            std::cout << "ERROR: Failed to send packet.";
            return false;
        }
        return true;
    }

    //Checks if any message arrived, and if so,
    //save the sender to param sender
    //save the messagetype to param type
    //save the bytes read to param bytesread
    void receive(IpAddress& sender, unsigned char& type, int& bytesRead) {
        if(socket == 0) {
            std::cout << "Cannot receive because socket isn't open" << std::endl;
        }

        sockaddr_in from;
        socklen_t fromLength = sizeof(from);
        bytesRead = recvfrom(socket, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromLength);

        if(bytesRead > 0) {
            unsigned int fromAddress = ntohl(from.sin_addr.s_addr);
            unsigned int fromPort = ntohs(from.sin_port);
            sender = IpAddress(fromAddress, fromPort);

            printf("received packet from %d.%d.%d.%d:%d (%dbytes)\n",
                sender.getA(), sender.getB(), sender.getC(), sender.getD(),
                sender.getPort(), bytesRead
            );

            //Deserialize contents of buffer into (untyped) tmppacket
            std::string str((char*)&buffer[0], bytesRead);
            std::istringstream iss(str);
            cereal::PortableBinaryInputArchive pbia(iss);
            Packet<char> tmppacket;
            pbia(tmppacket);

            //Save packet metadata
            const auto& protocol = tmppacket.getProtocol();
            const auto& sequence = tmppacket.getSequence();
            type = tmppacket.getType();
            const auto& datasize = tmppacket.getDataSize();

            //Print packet metadata if NET_DEBUG is defined
            #ifdef NET_DEBUG
            std::cout << "\tprotocol: " << protocol << std::endl;
            std::cout << "\tsequence: " << (int)sequence << std::endl;
            std::cout << "\tdatatype: " << (int)type << std::endl;
            std::cout << "\tdatasize: " << datasize << std::endl;
            #endif //NET_DEBUG

            //Check if the packet is meant for swordbow-magic (protocol)
            if(protocol == stringhash("swordbow-magic")) {
                static int remoteSequence = -1;

                //TODO: Implement sequence-wrap-around and change type of
                //sequence to something with fewer bytes than unsigned int..
                if(sequence > remoteSequence) {
                    remoteSequence = sequence;

                    //TODO: Figure out what 'ack(knowledge)' is and implement congestion control
                    //TODO: Implement congestion avoidance

                } else {
                    std::cout << "Got old outdated packet, discarding it" << std::endl;
                }
            } else {
                std::cout << "got packet with illegal protocol " << protocol;
                std::cout << ", expected " << stringhash("swordbow-magic") << std::endl;
            }
        }
    }

    //Return deserialized buffer
    template<class T>
    T get(int bytesRead) const {
        T object;
        std::string str((char*)&buffer[0], bytesRead);
        std::istringstream iss(str);
        cereal::PortableBinaryInputArchive pbia(iss);
        pbia(object);
        return object;
    }
};

#endif //SOCKET_HPP
