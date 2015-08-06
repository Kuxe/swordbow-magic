#include "ipaddress.hpp"
#include <string>

//For outputting IpAddress to stream
std::ostream& operator << (std::ostream& os, const IpAddress& ip) {
    return os << std::string(
		std::to_string(ip.getA()) + "." +
		std::to_string(ip.getB()) + "." +
		std::to_string(ip.getC()) + "." +
		std::to_string(ip.getD()) + ":" +
		std::to_string(ip.getPort())
	);
}