#ifndef COMMANDCOMPONENT_HPP
#define COMMANDCOMPONENT_HPP

#include <unordered_map>
#include <forward_list>

using std::unordered_map;
using std::forward_list;

class ICommand;

//This component allows each entity to do it's own thing
//on a certain event (command).
//If an entity should WALK_UP, it iterates over
//the linked list (forward_list) on that key, executing the
//ICommands (such as addIdToSystem)
struct CommandComponent {
    unordered_map<unsigned int, forward_list<ICommand*> > commands;
    constexpr inline forward_list<ICommand*>& operator [](unsigned int index) {
        return commands[index];
    }
};


#endif //COMMANDCOMPONENT_HPP
