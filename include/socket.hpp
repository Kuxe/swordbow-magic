#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "ipaddress.hpp"
#include <fcntl.h>
#include <unistd.h> //close()
#include "logger.hpp"
#include <fstream>
#include <chrono>
#include <unordered_map>

#include "platform.hpp"

#if PLATFORM == PLATFORM_LINUX
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
    #include <winsock2.h>
    typedef int socklen_t;
#endif //PLATFORM == PLATFORM_WINDOWS

class Socket {
private:
    int socket = 0;
    char buffer[8192];

public:
    bool open(unsigned short port) {

        #if PLATFORM == PLATFORM_WINDOWS
            WSADATA WsaData;
            if(WSAStartup(MAKEWORD(2,2), &WsaData) != NO_ERROR) {
                return false;
            }
        #endif //PLATFORM == PLATFORM_WINDOWS

        //Create a socket and check for failure
        socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(socket <= 0) {
            Logger::error("Couldn't create socket");
            return false;
        }

        sockaddr_in address;
        address.sin_family = AF_INET; //mumbojumbo
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if(bind(socket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0 ) {
            Logger::log("Couldn't bind socket");
            close();
            return false;
        }

        //Dont wait until a message is recieved

        #if PLATFORM == PLATFORM_LINUX || PLATFORM == PLATFORM_APPLE
            constexpr int nonBlocking = 1;
            if(fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1) {
                Logger::log("Failed to set non-blocking");
                close();
                return false;
            }

        #elif PLATFORM == PLATFORM_WINDOWS
            DWORD nonBlocking = 1;
            if(ioctlsocket(socket, FIONBIO, &nonBlocking) != 0) {
                Logger::log("Failed to set non-blocking");
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
            WSACleanup();
        #endif
    }
    bool isOpen() const {
        return socket != 0;
    }

    bool send(const IpAddress& destination, const std::string& serializedObject) {
        if(!isOpen()) {
            Logger::log("Cannot send because socket isn't open");
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
                    Logger::error("Can't send packet, error EWOULDBLOCK (Socket is non-blocking and requested operation would block)");
                } break;
                case EINTR: {
                    Logger::error("Can't send packet, error EINTR (A signal interrupted sendto() before any data was transmitted)");
                } break;
                case EMSGSIZE: {
                    std::ostringstream oss;
                    oss << "Can't send packet, error EMSGSIZE (Packet size to large, size: " << size << "bytes)";
                    Logger::error(oss);
                } break;
                case EIO: {
                    Logger::error("Can't send packet, error EIO (An I/O error occurred while reading from or writing to the file system)");
                } break;
                case ENOBUFS: {
                    Logger::error("Can't send packet, error ENOBUFS (Output queue for network interface is full)");
                } break;
                case ENOMEM: {
                    Logger::error("Can't send packet, error ENOMEM (Out of memory)");
                } break;
                default: {

                    #if PLATFORM == PLATFORM_WINDOWS
                    {
                        std::wstringstream wss;
                        wchar_t *s = NULL;
                        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                            NULL, WSAGetLastError(),
                            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                            (LPWSTR)&s, 0, NULL);
                        std::wstring ws(s);
                        std::string str(ws.begin(), ws.end()-1);
                        std::ostringstream oss;
                        oss << "Can't send packet, windows-specific error: " << str;
                        Logger::error(oss);
                        LocalFree(s);
                    }
                    #endif //PLATFORM == PLATFORM_WINDOWS


                    std::ostringstream oss;
                    oss << "Failed to send packet for unknown reason. Sent " << sendBytes << " bytes but should've sent " << size << " bytes (errorcode: " << errno << ")";
                    Logger::error(oss);
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
            Logger::warning("Cannot receive because socket isn't open");
        }

        sockaddr_in from;
        socklen_t fromLength = sizeof(from);

        //FIXME: recvfrom should be blocking (why? I do not understand why as of 2017-01-22)
        bytesRead = recvfrom(socket, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromLength);

        /** Should there be a bytes > 0 check before this part? **/
        unsigned int fromAddress = ntohl(from.sin_addr.s_addr);
        unsigned int fromPort = ntohs(from.sin_port);
        sender = IpAddress(fromAddress, fromPort);
        /** END OF POSSIBLE IF-CHECK PART **/

        return buffer;
    }
};

#endif //SOCKET_HPP
