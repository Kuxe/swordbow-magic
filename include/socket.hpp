#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "ipaddress.hpp"
#include "socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h> //close()
#include "logger.hpp"
#include "packet.hpp"
#include <fstream>
#include <chrono>
#include <mutex>
#include <unordered_map>

#include "platform.hpp"

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
#endif //PLATFORM == PLATFORM_WINDOWS



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

#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif //PLATFORM == PLATFORM_WINDOWS

class Socket {
private:
    int socket = 0;
    unsigned char buffer[4096];
    std::string protocolName;
    std::mutex mutex;

    std::unordered_map<IpAddress, unsigned int> remoteSequenceNumbers;


public:
    Socket(std::string protocolName) :
        protocolName(protocolName) {
            #if PLATFORM == PLATFORM_WINDOWS
                WSADATA WsaData;
                return WSAStartup(MAKEWORD(2,2), &WsaData) == NO_ERROR;
            #endif //PLATFORM == PLATFORM_WINDOWS
    }

    ~Socket() {
        #if PLATFORM == PLATFORM_WINDOWS
            WSACleanup();
        #endif //PLATFORM == PLATFORM_WINDOWS
    }

    bool open(unsigned short port) {
        //Create a socket and check for failure
        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(socket <= 0) {
            Logger::log("Couldn't create socket", Log::ERROR);
            return false;
        }

        sockaddr_in address;
        address.sin_family = AF_INET; //mumbojumbo
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if(bind(socket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0 )
        {
            Logger::log("Couldn't bind socket", Log::ERROR);
            close();
            return false;
        }

        //Dont wait until a message is recieved

        #if PLATFORM == PLATFORM_LINUX || PLATFORM == PLATFORM_APPLE
            constexpr int nonBlocking = 1;
            if(fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
                Logger::log("Failed to set non-blocking", Log::ERROR);
                close();
                return false;
            }

        #elif PLATFORM == PLATFORM_WINDOWS
            DWORD nonBlocking = 1;
            if(ioctlsocket(socket, FIONBIO, &nonBlocking) != 0) {
                Logger::log("Failed to set non-blocking", Log::ERROR);
                close();
                return false;
            }
        #endif


        return true;
    }

    void close() {
        #if PLATFORM == PLATFORM_MAX || PLATFORM == PLATFORM_LINUX
            ::close(socket);
        #elif PLATFORM == PLATFORM_WINDOWS
            closesocket(socket);
        #endif
    }
    bool isOpen() const {
        return socket != 0;
    }

    template<class T>
    bool send(const IpAddress& destination, const T& object) {
        if(socket == 0) {
            Logger::log("Cannot send because socket isn't open", Log::ERROR);
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

        mutex.lock();

        //Send to that address
        int sendBytes = sendto(
            socket,
            str.c_str(),    //send serialized data
            size,           //serialized data-length
            0,
            (sockaddr*)&address,
            sizeof(sockaddr_in)
        );

        mutex.unlock();

        //Doesn't check if packet succesfully arrived (UDP duh)
        //but if the expected size of sent bytes matches the
        //size of the sent packet
        if(sendBytes != size) {
            Logger::log("Failed to send packet", Log::ERROR);
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
            Logger::log("Cannot receive because socket isn't open", Log::WARNING);
        }

        sockaddr_in from;
        socklen_t fromLength = sizeof(from);

        mutex.lock();
        bytesRead = recvfrom(socket, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromLength);
        mutex.unlock();

        if(bytesRead > 0) {
            unsigned int fromAddress = ntohl(from.sin_addr.s_addr);
            unsigned int fromPort = ntohs(from.sin_port);
            sender = IpAddress(fromAddress, fromPort);

            //Print ip address of sender, bytes in packet
            const std::string packetLog(
                "Received packet from " +
                std::to_string(sender.getA()) + "." +
                std::to_string(sender.getB()) + "." +
                std::to_string(sender.getC()) + "." +
                std::to_string(sender.getD()) + ":" +
                std::to_string(sender.getPort()) + " " +
                "(" + std::to_string(bytesRead) + "bytes)"
            );

            Logger::log(packetLog, Log::INFO);

            //Deserialize contents of buffer into (untyped) tmppacket
            std::string str((char*)buffer, bytesRead);
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
            const std::string packetMetadatalog(
                "\tprotocol: " + std::to_string(protocol) +
                "\n\tsequence: " + std::to_string((int)sequence) +
                "\n\tdatatype: " + std::to_string((int)type) +
                "\n\tdatasize: " + std::to_string(datasize) + "\n"
            );

            //Check if the packet is meant for swordbow-magic (protocol)
            if(protocol == stringhash(protocolName)) {

                //Each sender has it's own remoteSequenceNumber
                const auto& remoteSequence = remoteSequenceNumbers[sender];

                //Only accept recent messages
                constexpr uint16_t MAX = 65535;
                const bool recent =
                    (sequence - remoteSequence && sequence - remoteSequence < MAX/2) ||
                    (remoteSequence > sequence && remoteSequence - sequence < MAX/2);

                if(recent) {
                    remoteSequenceNumbers[sender] = sequence;

                    //TODO: Figure out what 'ack(knowledge)' is and implement congestion control
                    //TODO: Implement congestion avoidance

                } else {
                    type = 0; //0 for outdated
                }
            } else {
                Logger::log(
                    "Got packet with illegal protocol " + std::to_string(protocol) +
                    ", expected " + std::to_string(stringhash(protocolName)) +
                    "(" + protocolName + ")\n"
                );
            }
        }
    }

    //Return deserialized buffer
    template<class T>
    T get(int bytesRead) {
        T object;
        std::string str((char*)buffer, bytesRead);
        std::istringstream iss(str);
        cereal::PortableBinaryInputArchive pbia(iss);
        pbia(object);
        return object;
    }

    //Should be called if same machine restarts a client, since that
    //client will start producing sequencenumbers starting on 0, 1, 2 ...
    //but the server, without calling this method, would compare 0, 1, 2 etc
    //towards an already present, possibly larger, number within
    //remoteSequnceNumbers which would result in deeming the newest packet
    //as an old packet, which is unwanted behaviour..
    void resetRemoteSequenceNumber(const IpAddress& ipAddress) {
        remoteSequenceNumbers[ipAddress] = 0;
    }
};

#endif //SOCKET_HPP
