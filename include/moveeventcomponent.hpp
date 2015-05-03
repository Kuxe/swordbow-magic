#ifndef MOVEEVENTCOMPONENT_H
#define MOVEEVENTCOMPONENT_H

#include <forward_list>
#include "icommand.hpp"

class MoveEventComponent {
private:
    forward_list<ICommand*> commands;
public:

    //Im not sure about this DTOR... This WILL break
    //if something allocated on stack is passed as
    //argument to addCommand()... For now, just make
    //sure to only allocate on heap when using commands
    ~MoveEventComponent() {
        for(auto command : commands) {
            delete command;
        }
    }

    void addCommand(ICommand* command) { commands.push_front(command); }
    void happen() /*It's happening */ {
        for(auto command : commands) {
            command->execute();
        }
    };
};


#endif //MOVEEVENTCOMPONENT_H
