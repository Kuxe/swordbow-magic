#ifndef PRESSKEYEVENTCOMPONENT_H
#define PRESSKEYEVENTCOMPONENT_H

#include <forward_list>
#include "icommand.hpp"

class PressKeyEventComponent {
private:
    std::forward_list<ICommand*> commands;
public:

    ~PressKeyEventComponent() {
        for(auto command : commands) {
            delete command;
        }
    }

    void addCommand(ICommand* command) { commands.push_front(command); }
    void happen() {
        for(auto command : commands) {
            command->execute();
        }
    }
};

#endif //PRESSKEYEVENTCOMPONENT_H
