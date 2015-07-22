#ifndef STRINGHASH_HPP
#define STRINGHASH_HPP

#include <functional>
#include <string>

static uint16_t stringhash(const std::string& str) {
    std::hash<std::string> str_hash;
    return str_hash(str);
}

#endif //STRINGHASH_HPP
