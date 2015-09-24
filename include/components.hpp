#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <unordered_map>

/** For logging **/
#include "logger.hpp"
#include <ostream>
#include <typeinfo>

typedef unsigned int ID;

template<class T, class Container = std::unordered_map<ID, T>>
class Components {
private:
    Container container;

public:
    void insert(const std::pair<const ID, const T&> pair) {
        std::ostringstream oss;
        oss << "Adding " << typeid(T).name() << " to id " << pair.first;
        Logger::log(oss, Log::INFO);
        container.insert(pair);
    }

    void sync(Components<T>& components) {
        for(auto it : components) {
            container[it.first] = components[it.first];
        }
    }

    Components<T, Container>& operator=(Components<T, Container> components) {
        container.swap(components.container);
        return *this;
    }

    T& operator[](const ID id) {
        if(container.find(id) == container.end()) {
            std::ostringstream oss;
            oss << "Creating " << typeid(T).name() << " on " << id;
            Logger::log(oss, Log::INFO);
        }
        return container[id];
    }

    const T& operator[](const ID id) const {
        if(container.find(id) == container.end()) {
            std::ostringstream oss;
            oss << "Creating " << typeid(T).name() << " on " << id;
            Logger::log(oss, Log::INFO);
        }
        return container[id];
    }

    T& at(const ID id) {
        //Log before the error before throwing it
        try {
            return container.at(id);
        } catch (std::out_of_range oor) {
            std::ostringstream oss;
            oss << "Tried accesing absent " << typeid(T).name() << " on id " << id;
            Logger::log(oss, Log::ERROR);
            throw oor;
        }
    }

    const T& at(const ID id) const {
        //Log before the error before throwing it
        try {
            return container.at(id);
        } catch (std::out_of_range oor) {
            std::ostringstream oss;
            oss << "Tried accesing absent " << typeid(T).name() << " on id " << id;
            Logger::log(oss, Log::ERROR);
            throw oor;
        }
    }

    constexpr auto find(const ID id) {
        return container.find(id);
    }

    constexpr bool contains(const ID id) {
        return container.find(id) != container.end();
    }

    constexpr auto begin() const {
        return container.begin();
    }

    constexpr auto end() const {
        return container.end();
    }

    auto erase(const ID id) {
        std::ostringstream oss;
        oss << "Erasing " << typeid(T).name() << " from id " << id;
        Logger::log(oss, Log::INFO);
        return container.erase(id);
    }

    constexpr void clear() {
        container.clear();
    }

    constexpr auto size() const {
        return container.size();
    }

    constexpr auto empty() const {
        return container.empty();
    }

    template<class Archive>
    constexpr void serialize(Archive& ar) {
        ar(container);
    }
};

#endif //COMPONENTS_HPP
