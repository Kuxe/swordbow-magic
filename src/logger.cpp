#include "logger.hpp"
#include <iostream>
#include <iomanip> //for std::put_time
#include <ctime>
#include "platform.hpp"


//Default to flow-logging
Log::Level Logger::level = Log::INFO;

//Enabled by default
bool Logger::enabled = true;

std::ofstream Logger::logfile;

void Logger::openLogfile(const std::string path = "logs.txt") {
	logfile.open(path, std::ios_base::app);
	auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
	logfile << "\n--** NEW SESSION " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " **--\n";
}

void Logger::closeLogfile() {
	if(logfile.is_open()) {
		logfile.close();
	}
}

void Logger::log(const std::string& str, const Log::Level level) {
	if(level >= Logger::level && Logger::enabled) {

		switch(level) {
			case Log::VERBOSE: {
			#if PLATFORM == PLATFORM_LINUX
				std::cout << "\033[1;36mVERBOSE\033[0m: " << str << std::endl;
			#elif PLATFORM == PLATFORM_APPLE
				std::cout << "Log(VERBOSE):\t" << str << std::endl;
			#elif PLATFORM == PLATFORM_WINDOWS
				std::cout << "Log(VERBOSE):\t" << str << std::endl;
			#elif PLATFORM == UNKNOWN
				//Disable logging by doing nothing
			#endif
			} break;

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

	if(logfile.is_open()) {
		logfile << str << "\n";
		logfile.flush();
	}
}

void Logger::log(std::ostringstream& oss, const Log::Level level) {
	Logger::log(oss.str(), level);
}

void Logger::disable() {
	Logger::enabled = false;
}
void Logger::enable() {
	Logger::enabled = true;
}