#include "inputsystem.hpp"
#include "componentmanager.hpp"
#include "icommand.hpp"

#include <iostream>

using std::cout;
using std::endl;

void InputSystem::add(ID id) {
    ids.insert(id);
}
void InputSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        cout << "ERROR: Tried to remove unpresent ID from InputSystem. Don't know what will happen from now on!" << endl;
        return;
    }
    ids.erase(id);
}

void InputSystem::update() {
    while(!activeIds.empty()) {
        auto id = activeIds.front(); activeIds.pop();
        auto& ic = componentManager->inputComponents.at(id);
        auto& cc = componentManager->commandComponents.at(id);

        //For all keypresses registered on this entity...
        while(!ic.presses.empty()) {
            auto key = ic.presses.front(); ic.presses.pop();

            //Resolve keybinding, as in get action bound to the keypress
            //Note that indexing unordered_map with [] creates an element
            //with key as key... The value (forward_list) will be empty though.
            auto action = ic.bindings[key];

            //Resolve action, as in do whatever the entity should do on the action
            for(ICommand* command : cc.commands[action]) {
                command->execute();
            }
        }
    }
}
unsigned int InputSystem::count() const {
    return ids.size();
}
const string InputSystem::getIdentifier() const {
    return "InputSystem";
}
void InputSystem::activateId(ID id) {
    if(ids.find(id) == ids.end()) {
        return;
    }
    activeIds.push(id);
}
