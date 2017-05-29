#ifndef IPADDRESS_HPP
#define IPADDRESS_HPP

#include "hash_combine.hpp"

//Forward declare
struct IpAddressHash;

class IpAddress {
private:
    friend std::hash<IpAddress>;
    friend std::ostream& operator<<(std::ostream&, const IpAddress& ipAddress);
    unsigned int address = 0;
    unsigned short port = 0;

public:
    constexpr IpAddress() { }

    constexpr IpAddress(
        unsigned char a,
        unsigned char b,
        unsigned char c,
        unsigned char d,
        unsigned short port) :
        IpAddress((a << 24) | (b << 16) | (c << 8) | d, port) { }

    constexpr IpAddress(
        unsigned int address,
        unsigned short port) :
        address(address),
        port(port) { }

    constexpr IpAddress(const IpAddress& other) :
        address(other.address),
        port(other.port) { }

    constexpr unsigned int getAddress() const { return address; }
    constexpr unsigned char getA() const { return address >> 24; }
    constexpr unsigned char getB() const { return address >> 16; }
    constexpr unsigned char getC() const { return address >> 8; }
    constexpr unsigned char getD() const { return address; }
    constexpr unsigned short getPort() const { return port; }

    constexpr bool operator==(const IpAddress& other) const {
        return address == other.address && port == other.port;
    }

    constexpr bool operator!=(const IpAddress& other) const {
        return !(*this == other);
    }

    template<class Archive>
    void serialize(Archive& ar) {
        ar(address, port);
    }

};

//Hash method for using IpAddress as key in unordered_map
namespace std {
    template<>
    struct hash<IpAddress> {
        std::size_t operator()(const IpAddress& ipa) const {
            std::size_t s = 0;
            hash_combine(s, ipa.address);
            hash_combine(s, ipa.port);
            return s;
        }
    };
};

#endif //IPADDRESS_HPP
