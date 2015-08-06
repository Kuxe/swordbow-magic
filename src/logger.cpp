#include "logger.hpp"
#include <iostream>
#include "platform.hpp"

//Default to flow-logging
Log::Level Logger::level = Log::INFO;

void Logger::log(const std::string& str, const Log::Level level) {
	if(level >= Logger::level) {

	switch(level) {
			case Log::INFO: {
			#if PLATFORM == PLATFORM_LINUX
				std::cout << "\033[1;32mINFO\033[0m: " << str << std::endl;
			#elif PLATFORM == PLATFORM_APPLE
				std::cout << "Log(INFO):\t" << str << std::endl;
			#elif PLATFORM == PLATFORM_WINDOWS
				std::cout << "Log(INFO):\t" << str << std::endl;
			#elif PLATFORM == UNKNOWN
				//Disable logging by doing nothing
			#endif
			} break;

			case Log::WARNING: {
			#if PLATFORM == PLATFORM_LINUX
				std::cout << "\033[1;33mWARNING\033[0m: " << str << std::endl;
			#elif PLATFORM == PLATFORM_APPLE
				std::cout << "Log(WARNING):\t" << str << std::endl;
			#elif PLATFORM == PLATFORM_WINDOWS
				std::cout << "Log(WARNING):\t" << str << std::endl;
			#elif PLATFORM == UNKNOWN
				//Disable logging by doing nothing
			#endif
			} break;

			case Log::ERROR: {
			#if PLATFORM == PLATFORM_LINUX
				std::cout << "\033[1;31mERROR\033[0m: " << str << std::endl;
			#elif PLATFORM == PLATFORM_APPLE
				std::cout << "Log(ERROR):\t" << str << std::endl;
			#elif PLATFORM == PLATFORM_WINDOWS
				std::cout << "Log(ERROR):\t" << str << std::endl;
			#elif PLATFORM == UNKNOWN
				//Disable logging by doing nothing
			#endif
			} break;

			case Log::UNDEFINED: {
				//Disable logging by doing nothing
			} break;
		}
	}
}

void Logger::log(std::ostringstream& oss, const Log::Level level) {
	Logger::log(oss.str(), level);
}