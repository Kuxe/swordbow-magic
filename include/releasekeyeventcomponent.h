#ifndef RELEASEKEYEVENTCOMPONENT_H
#define RELEASEKEYEVENTCOMPONENT_H

#include <forward_list>
#include "icommand.h"

class ReleaseKeyEventComponent {
private:
    std::forward_list<ICommand*> commands;
public:

    ~ReleaseKeyEventComponent() {
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

#endif //RELEASEKEYEVENTCOMPONENT_H
