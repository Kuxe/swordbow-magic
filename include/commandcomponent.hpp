#ifndef COMMANDCOMPONENT_HPP
#define COMMANDCOMPONENT_HPP

#include <unordered_map>
#include <forward_list>

//Forward declaration could work instead of inclusion but that leads to
//undefined behaviour when deleting a pointer to ICommand
#include "icommand.hpp"

//This component allows each entity to do it's own thing
//on a certain event (command).
//If an entity should WALK_UP, it iterates over
//the linked list (forward_list) on that key, executing the
//ICommands (such as addIdToSystem)
struct CommandComponent {
    enum Event {
        MOVE_UP,
        MOVE_RIGHT,
        MOVE_DOWN,
        MOVE_LEFT,
        ATTACK,
        ON_DEATH,
        ON_MOVE
    };

    CommandComponent() { }

    std::unordered_map<unsigned int, std::forward_list<ICommand*> > commands;

    virtual ~CommandComponent() {
        for(auto list : commands) {
            for(auto command : list.second) {
                delete command;
            }
        }
    }

    inline std::forward_list<ICommand*>& operator [](unsigned int index) {
        return commands[index];
    }
    void execute(const Event& event);
};


#endif //COMMANDCOMPONENT_HPP
