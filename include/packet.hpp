#ifndef PACKET_HPP
#define PACKET_HPP

#include <type_traits>

#include "stringhash.hpp"

//Representing objects as unsigned int is probably the easiest way
//but template is here to allow user to pass any type in the packet
//caution: objects with pointers wont work as data, obviously,
//since memory pointed to by pointers aren't the same across
//machines...
template<class Data = unsigned int>
class Packet {
private:
    uint16_t protocol;
    uint16_t sequence;
    unsigned int datasize;
    unsigned char type;

    //Dont move data any further up - it will cause invalid read of
    //protocol, sequence, type and datasize before the packet was
    //typecasted to the correct type
    Data data;
public:
    constexpr Packet() {}
    constexpr Packet(const unsigned int protocol, uint16_t sequence, unsigned char type, const Data& data, unsigned int datasize) :
        protocol(protocol), sequence(sequence), type(type), data(data), datasize(datasize) {
            static_assert(
                !std::is_pointer<Data>::value,
                "Sending pointers over network can't work. Try to copy the data pointed to by the pointer instead"
            );
        }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(protocol, sequence, datasize, type, data);
    }

    constexpr uint16_t getProtocol() const { return protocol; }
    constexpr uint16_t getSequence() const { return sequence; }
    constexpr unsigned char getType() const { return type; }
    constexpr const Data& getData() const { return data; }
    constexpr Data& getData() { return data; }
    constexpr unsigned int getDataSize() { return datasize; }
};

#endif //PACKET_HPP
