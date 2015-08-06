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
    constexpr void insert(const std::pair<const ID, const T&> pair) {
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

    constexpr T& operator[](const ID id) {
        return container[id];
    }

    constexpr const T& operator[](const ID id) const {
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

    constexpr const T& at(const ID id) const {
        return container.at(id);
    }

    constexpr auto find(const ID id) {
        return container.find(id);
    }

    constexpr auto begin() const {
        return container.begin();
    }

    constexpr auto end() const {
        return container.end();
    }

    constexpr auto erase(const ID id) {
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
