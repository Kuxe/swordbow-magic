#ifndef PACKET_HPP
#define PACKET_HPP

#include <type_traits>
#include "stringhash.hpp"
#include "messagetypes.hpp"
#include "packethandler.hpp"

/** MESSAGE TO SELF: If I ever get the idea that this packet-class should contain ip address,
    DONT make it a member! Make it a feature of the Data instead. Otherwise I am at risk making
    the ip address class making its way everywhere in the program **/

/** Sole purpose of IPacket is to provide a function givetype in which
    the realizing child of IPacket may pass *this to the PacketHandler,
    such that the PacketHandler will know the underlying type of IPacket
    (the IPacket is telling its type!) **/
struct IPacket {
    virtual void greet(PacketHandler* ph) = 0;
};

//Representing objects as unsigned int is probably the easiest way
//but template is here to allow user to pass any type in the packet
//caution: objects with pointers wont work as data, obviously,
//since memory pointed to by pointers aren't the same across
//machines...
template<class Data, MESSAGE_TYPE Message>
class Packet : public IPacket {
private:
    uint16_t protocol;
    uint16_t sequence;
    unsigned int datasize;
    MESSAGE_TYPE type;

    //Dont move data any further up - it will cause invalid read of
    //protocol, sequence, type and datasize before the packet was
    //typecasted to the correct type
    Data data;
public:
    constexpr Packet() {}
    constexpr Packet(const unsigned int protocol, uint16_t sequence, const Data& data, unsigned int datasize) :
        protocol(protocol), sequence(sequence), type(Message), data(data), datasize(datasize) {
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
    constexpr MESSAGE_TYPE getType() const { return type; }
    constexpr const Data& getData() const { return data; }
    constexpr Data& getData() { return data; }
    constexpr unsigned int getDataSize() { return datasize; }

    /** FIXME: This cant ever work because PacketHandlers expect "ConnectToServerData" or such,
        which is never available from here (only the enum "MESSAGE_TYPE" or actual raw-data is available here **/ 
    void greet(PacketHandler* ph) override {
        static_assert(!std::is_same<decltype(data), bool>::value, "Packet: data is wrongly of type bool, you want data to be of type found in messagetypes.hpp");
        ph->handle(&data);
    }

};

#endif //PACKET_HPP
