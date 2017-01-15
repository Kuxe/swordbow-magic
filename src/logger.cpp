#include "logger.hpp"
#include <iostream>
#include <iomanip> //for std::put_time
#include <ctime>
#include "platform.hpp"


//Default to flow-logging
Log Logger::level = Logger::INFO;

//Disabled by default
bool Logger::enabled = false;

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

void Logger::log(const std::string& str, const Log level) {
	if(level >= Logger::level && Logger::enabled) {
		switch(level) {
			case Logger::VERBOSE: { verbose(str); } break;
			case Logger::INFO: { info(str); } break;
			case Logger::WARNING: { warning(str); } break;
			case Logger::ERROR: { error(str); } break;
			case Logger::UNDEFINED: { /*Disable logging by doing nothing */ } break;
		}
	}

	if(logfile.is_open()) {
		logfile << str << "\n";
		logfile.flush();
	}
}

void Logger::log(std::ostringstream& oss, const Log level) {
	Logger::log(oss.str(), level);
}

void Logger::verbose(const std::string& str) {
	#if PLATFORM == PLATFORM_LINUX
		std::cout << "\033[1;36mVERBOSE\033[0m: " << str << std::endl;
	#elif PLATFORM == PLATFORM_APPLE
		std::cout << "Log(VERBOSE):\t" << str << std::endl;
	#elif PLATFORM == PLATFORM_WINDOWS
		std::cout << "Log(VERBOSE):\t" << str << std::endl;
	#elif PLATFORM == UNKNOWN
		//Disable logging by doing nothing
	#endif
}
void Logger::verbose(std::ostringstream& oss) {
	Logger::verbose(oss.str());
}

void Logger::info(const std::string& str) {
	#if PLATFORM == PLATFORM_LINUX
		std::cout << "\033[1;32mINFO\033[0m: " << str << std::endl;
	#elif PLATFORM == PLATFORM_APPLE
		std::cout << "Log(INFO):\t" << str << std::endl;
	#elif PLATFORM == PLATFORM_WINDOWS
		std::cout << "Log(INFO):\t" << str << std::endl;
	#elif PLATFORM == UNKNOWN
		//Disable logging by doing nothing
	#endif
}
void Logger::info(std::ostringstream& oss) {
	Logger::info(oss.str());
}

void Logger::warning(const std::string& str) {
	#if PLATFORM == PLATFORM_LINUX
		std::cout << "\033[1;33mWARNING\033[0m: " << str << std::endl;
	#elif PLATFORM == PLATFORM_APPLE
		std::cout << "Log(WARNING):\t" << str << std::endl;
	#elif PLATFORM == PLATFORM_WINDOWS
		std::cout << "Log(WARNING):\t" << str << std::endl;
	#elif PLATFORM == UNKNOWN
		//Disable logging by doing nothing
	#endif
}
void Logger::warning(std::ostringstream& oss) {
	Logger::warning(oss.str());
}

void Logger::error(const std::string& str) {
	#if PLATFORM == PLATFORM_LINUX
		std::cout << "\033[1;31mERROR\033[0m: " << str << std::endl;
	#elif PLATFORM == PLATFORM_APPLE
		std::cout << "Log(ERROR):\t" << str << std::endl;
	#elif PLATFORM == PLATFORM_WINDOWS
		std::cout << "Log(ERROR):\t" << str << std::endl;
	#elif PLATFORM == UNKNOWN
		//Disable logging by doing nothing
	#endif
}
void Logger::error(std::ostringstream& oss) {
	Logger::error(oss.str());
}

void Logger::disable() {
	Logger::enabled = false;
}
void Logger::enable() {
	Logger::enabled = true;
}