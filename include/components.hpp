#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include <unordered_map>
#include <vector>

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
    constexpr Components() { }
    constexpr Components(const size_t size) :
        container(size) { }

    //Copy constructor
    constexpr Components(const Components<T, Container>& components) : container(components.container) { }

    void insert(const std::pair<const ID, const T&> pair) {
        std::ostringstream oss;
        oss << "Adding " << typeid(T).name() << " to id " << pair.first;
        Logger::verbose(oss);
        container.insert(pair);
    }

    void sync(const Components<T>& components) {
        try {
            for(const auto& it : components) {
                container[it.first] = components.get(it.first);
            }
        } catch(const std::exception& e) {
            Logger::error("Could not sync components");
        }
    }

    const T& get(const ID id) const {
        return container.at(id);
    }

    Components<T, Container>& operator=(Components<T, Container> components) {
        container.swap(components.container);
        return *this;
    }

    T& operator[](const ID id) {
        if(container.find(id) == container.end()) {
            std::ostringstream oss;
            oss << "Creating " << typeid(T).name() << " on " << id;
            Logger::verbose(oss);
        }
        return container[id];
    }

    const T& operator[](const ID id) const {
        if(container.find(id) == container.end()) {
            std::ostringstream oss;
            oss << "Creating " << typeid(T).name() << " on " << id;
            Logger::verbose(oss);
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
            Logger::verbose(oss);
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
            Logger::error(oss);
            throw oor;
        }
    }

    constexpr auto find(const ID id) {
        return container.find(id);
    }

    constexpr bool contains(const ID id) const {
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
        Logger::verbose(oss);
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

    /** Split the component into several containers no more components than componentsPerContainer
        but there may be less components in each container. There can never be more components in
        a container than there are IDs, hence the type of parameter is ID **/
    std::vector<Components<T, Container>> split(ID componentsPerContainer) const {
        const int numContainers = (size() / componentsPerContainer) + ((size() % componentsPerContainer) > 0 ? 1 : 0);
        std::vector<Components<T>> containers(numContainers, Components<T, Container>());
        auto it(begin());
        for(auto& smallerContainer : containers) {
            for(int i = 0; i < componentsPerContainer && it != end(); i++) {
                smallerContainer.insert(*it++);
            }
        }
        return containers;
    }

    /*const std::vector<Components<const T&>> split(ID componentsPerContainer) const {
        const int numContainers = (size() / componentsPerContainer) + ((size() % componentsPerContainer) > 0 ? 1 : 0);
        std::vector<Components<T, Container>> containers(numContainers, Components<T, Container>());
        auto it(begin());
        for(auto& smallerContainer : containers) {
            for(int i = 0; i < componentsPerContainer && it != end(); i++) {
                smallerContainer.insert(*it++);
            }
        }
        return containers;
    }*/

    template<class Archive>
    constexpr void serialize(Archive& ar) {
        ar(container);
    }
};

#endif //COMPONENTS_HPP
