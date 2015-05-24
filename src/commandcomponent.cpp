#include "commandcomponent.hpp"
#include "icommand.hpp"

void CommandComponent::execute(const Event& event) {
    for(auto command : commands[event]) {
        command->execute();
    }
}
