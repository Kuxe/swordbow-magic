#include "logger.hpp"
#include <iostream>

//Default to flow-logging
Log::Level Logger::level = Log::INFO;

void Logger::log(const std::string& str, const Log::Level level) {
	if(level >= Logger::level) {
		std::cout << "Log(" << level << "):\t" << str << std::endl;
	}
}

void Logger::log(std::ostringstream& oss, const Log::Level level) {
	Logger::log(oss.str(), level);
}