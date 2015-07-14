#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <ostream>
#include <istream>

template<class Object, unsigned int BUFFER_SIZE = 32768>
class Serializer {
    char buffer[BUFFER_SIZE];

    Serializer operator <<(std::ostream const Object& object) {

    }
};


#endif //SERIALIZER_HPP
