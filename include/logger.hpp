#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <string>
#include <sstream>

namespace Log {
	enum Level {
		UNDEFINED,	//Default value
		INFO,		//Simple info announcements, eg "Rendering" or "ID removed"
		WARNING,	//For anything that might lead to an error or a bug
		ERROR		//Only happens if something is broke for sure
	};
};

/**	Logger logs strings and stringstreams to std::cout
	It has different levels of logging, with INFO being the most
	verbose and WARNING being middle-verbose and ERROR being least
	verbose. Info logs everything, WARNING log everything but INFO-logs
	and ERROR only log ERROR-logs.

	Logger works for Windows, OSX and Linux
**/
struct Logger {
public:
	static bool enabled;
	static Log::Level level;
	static void log(const std::string& str, const Log::Level level = Log::UNDEFINED);
	static void log(std::ostringstream& oss, const Log::Level level = Log::UNDEFINED);
	static void disable();
	static void enable();
};


#endif //LOGGER_HPP