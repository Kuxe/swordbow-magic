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
class CommandComponent {
public:
    /*enum Command {
        WALK_UP,
        WALK_RIGHT,
        WALK_DOWN,
        WALK_LEFT,
        ATTACK,
    };*/
    int command;

    unordered_map<int, forward_list<ICommand*> > commands;
};


#endif //COMMANDCOMPONENT_HPP
