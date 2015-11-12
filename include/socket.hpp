#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "ipaddress.hpp"
#include "socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h> //close()
#include "logger.hpp"
#include <fstream>
#include <chrono>
#include <unordered_map>

#include "platform.hpp"

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
#endif //PLATFORM == PLATFORM_WINDOWS

#if PLATFORM == PLATFORM_WINDOWS
    typedef int socklen_t;
#endif //PLATFORM == PLATFORM_WINDOWS

class Socket {
private:
    int socket = 0;
    char buffer[8192];

public:
    Socket() {
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
            constexpr int nonBlocking = 0;
            if(fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
                Logger::log("Failed to set non-blocking", Log::ERROR);
                close();
                return false;
            }

        #elif PLATFORM == PLATFORM_WINDOWS
            DWORD nonBlocking = 0;
            if(ioctlsocket(socket, FIONBIO, &nonBlocking) != 0) {
                Logger::log("Failed to set non-blocking", Log::ERROR);
                close();
                return false;
            }
        #endif

        return true;
    }

    void close() {
        #if PLATFORM == PLATFORM_APPLE || PLATFORM == PLATFORM_LINUX
            ::close(socket);
        #elif PLATFORM == PLATFORM_WINDOWS
            closesocket(socket);
        #endif
    }
    bool isOpen() const {
        return socket != 0;
    }

    bool send(const IpAddress& destination, const std::string& serializedObject) {
        if(!isOpen()) {
            Logger::log("Cannot send because socket isn't open", Log::ERROR);
            return false;
        }

        //Create an address from destination address and port...
        sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(destination.getAddress());
        address.sin_port = htons((unsigned short)destination.getPort());

        const auto& size = serializedObject.length();

        //Send to that address
        int sendBytes = sendto(
            socket,
            serializedObject.c_str(),    //send serialized data
            size,                       //serialized data-length
            0,
            (sockaddr*)&address,
            sizeof(sockaddr_in)
        );

        //Doesn't check if packet succesfully arrived (UDP duh)
        //but if the expected size of sent bytes matches the
        //size of the sent packet
        if(sendBytes != size) {
            switch(errno) {
                case EWOULDBLOCK: {
                    Logger::log("Can't send packet, error EWOULDBLOCK (Socket is non-blocking and requested operation would block)", Log::ERROR);
                } break;
                case EINTR: {
                    Logger::log("Can't send packet, error EINTR (A signal interrupted sendto() before any data was transmitted)", Log::ERROR);
                } break;
                case EMSGSIZE: {
                    std::ostringstream oss;
                    oss << "Can't send packet, error EMSGSIZE (Packet size to large, size: " << size << "bytes)";
                    Logger::log(oss, Log::ERROR);
                } break;
                case EIO: {
                    Logger::log("Can't send packet, error EIO (An I/O error occurred while reading from or writing to the file system)", Log::ERROR);
                } break;
                case ENOBUFS: {
                    Logger::log("Can't send packet, error ENOBUFS (Output queue for network interface is full)", Log::ERROR);
                } break;
                case ENOMEM: {
                    Logger::log("Can't send packet, error ENOMEM (Out of memory)", Log::ERROR);
                } break;
                default: {
                    std::ostringstream oss;
                    oss << "Failed to send packet for unknown reason (errorcode: " << errno << ")";
                    Logger::log(oss, Log::ERROR);
                } break;
            }
            return false;
        }
        return true;
    }

    //Checks if any message arrived, and if so,
    //save the sender to param sender
    //save the bytes read to param bytesread
    //return the buffer
    const char* receive(IpAddress& sender, int& bytesRead) {
        if(socket == 0) {
            Logger::log("Cannot receive because socket isn't open", Log::WARNING);
        }

        sockaddr_in from;
        socklen_t fromLength = sizeof(from);

        //FIXME: recvfrom should be blocking
        bytesRead = recvfrom(socket, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromLength);

        /** Should there be a bytes > 0 check before this part? **/
        unsigned int fromAddress = ntohl(from.sin_addr.s_addr);
        unsigned int fromPort = ntohs(from.sin_port);
        sender = IpAddress(fromAddress, fromPort);

        std::ostringstream oss;
        oss << "Recieved packet from " << sender << " (" << std::to_string(bytesRead) << "bytes)";
        Logger::log(oss, Log::VERBOSE);
        /** END OF POSSIBLE IF-CHECK PART **/

        return buffer;
    }
};

#endif //SOCKET_HPP
