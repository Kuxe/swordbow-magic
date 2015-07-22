#ifndef PLATFORM_HPP
#define PLATFORM_HPP

/** One of these will be defined, indicating what OS is used **/
#define PLATFORM_WINDOWS  1
#define PLATFORM_APPLE      2
#define PLATFORM_LINUX     3
#define PLATFORM_UNKNOWN  4

#if defined(_WIN32)
#define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
#define PLATFORM PLATFORM_APPLE
#elif defined(__linux__)
#define PLATFORM PLATFORM_LINUX
#else
#define PLATFORM UNKNOWN
#endif

#endif //PLATFORM_HPP
