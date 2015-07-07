#include "socket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h> //close()
#include <iostream>
#include "packet.hpp"

#define NET_DEBUG

using namespace std;

Socket::~Socket() {

}

bool Socket::open(unsigned short port) {
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

void Socket::close() {
    ::close(socket);
}

bool Socket::isOpen() const {
    return socket != 0;
}

bool Socket::send(const IpAddress& destination, const void* data, int size) {
    if(socket == 0) {
        std::cout << "ERROR: Cannot send because socket isn't open" << std::endl;
    }

    //Create an address from destination address and port...
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(destination.getAddress());
    address.sin_port = htons((unsigned short)destination.getPort());

    //Send to that address
    int sendBytes = sendto(
        socket,
        (const char*)data,
        size,
        0,
        (sockaddr*)&address,
        sizeof(sockaddr_in)
    );

    //Doesn't check if packet succesfully arrived (UDP duh)
    //but if the expected size of sent bytes matches the
    //size of the sent packet
    if(sendBytes != size) {
        std::cout << "ERROR: Failed to send packet" << std::endl;
        return false;
    }
    return true;
}

void Socket::receive(IpAddress& sender, unsigned char& type, int& bytesRead) {
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

        #ifdef NET_DEBUG
        printf("received packet from %d.%d.%d.%d:%d (%dbytes)\n",
            sender.getA(), sender.getB(), sender.getC(), sender.getD(),
            sender.getPort(), bytesRead
        );
        #endif //NET_DEBUG

        const auto& packet = (Packet<char>*)buffer;

        //Save packet metadata
        const auto& protocol = packet->getProtocol();
        const auto& sequence = packet->getSequence();
        type = packet->getType();
        const auto& datasize = packet->getDataSize();

        //Print packet metadata if NET_DEBUG is defined
        #ifdef NET_DEBUG
        cout << "\tprotocol: " << protocol << endl;
        cout << "\tsequence: " << (int)sequence << endl;
        cout << "\tdatatype: " << (int)type << endl;
        cout << "\tdatasize: " << datasize << endl;
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
                cout << "Got old outdated packet, discarding it" << endl;
            }
        } else {
            cout << "got packet with illegal protocol " << protocol;
            cout << ", expected " << stringhash("swordbow-magic") << endl;
        }
    }
}



//
