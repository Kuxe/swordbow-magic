#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <sstream>
#include <fstream>

/**	Logger logs strings and stringstreams to std::cout
	It has different levels of logging, with INFO being the most
	verbose and WARNING being middle-verbose and ERROR being least
	verbose. Info logs everything, WARNING log everything but INFO-logs
	and ERROR only log ERROR-logs.

	Logger works for Windows, OSX and Linux
**/

using Log = uint8_t;
struct Logger {
	static bool enabled;

	//Logfile will always log in verbose mode
	static std::ofstream logfile;
	static void openLogfile(const std::string path);
	static void closeLogfile();
	static void log(const std::string& str, const Log level = UNDEFINED);
	static void log(std::ostringstream& oss, const Log level = UNDEFINED);
	static void verbose(const std::string& str);
	static void verbose(std::ostringstream& oss);
	static void info(const std::string& str);
	static void info(std::ostringstream& oss);
	static void warning(const std::string& str);
	static void warning(std::ostringstream& oss);
	static void error(const std::string& str);
	static void error(std::ostringstream& oss);
	static void disable();
	static void enable();

	static Log level;
	constexpr static Log UNDEFINED = 0; //Default value
	constexpr static Log VERBOSE = 1; //These might bloat the log, things that occur 'almost' every gameloop (arbitrary...)
	constexpr static Log INFO = 2; //Simple info announcements, should not bloat the log
	constexpr static Log WARNING = 3; //For anything that might lead to an error or a bug
	constexpr static Log ERROR = 4; //Only happens if something is broke for sure
};


#endif //LOGGER_HPP