#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <unordered_map>

typedef unsigned int ID;

class Client;

template<class T, class Container = std::unordered_map<ID, T>>
class Components {
private:
    Container container;

public:
    constexpr void insert(const std::pair<const ID, const T&> pair) {
        container.insert(pair);
    }

    Components<T, Container>& operator=(Components<T, Container> components) {
        container.swap(components.container);
        return *this;
    }

    constexpr T& operator[](const ID id) {
        return container[id];
    }

    constexpr T& at(const ID id) {
        return container.at(id);
    }

    constexpr const T& at(const ID id) const {
        return container.at(id);
    }

    constexpr auto find(const ID id) {
        return container.find(id);
    }

    constexpr auto end() {
        return container.end();
    }

    constexpr auto erase(const ID id) {
        return container.erase(id);
    }
};

#endif //COMPONENTS_HPP
