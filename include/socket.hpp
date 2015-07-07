#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "ipaddress.hpp"

class Socket {
private:
    int socket = 0;
    unsigned char buffer[256];

public:
    ~Socket();
    bool open(unsigned short port);
    void close();
    bool isOpen() const;
    bool send(const IpAddress& destination, const void* data, int size);

    //Checks if any message arrived, and if so,
    //save the sender to param sender
    //save the messagetype to param type
    //save the bytes read to param bytesread
    void receive(IpAddress& sender, unsigned char& type, int& bytesRead);

    //get the most recent data received
    constexpr unsigned char* getBuffer() {
        return buffer;
    }
};

#endif //SOCKET_HPP
