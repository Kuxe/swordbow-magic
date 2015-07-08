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

            //Resolve keybinding, as in get event bound to the keypress
            //Note that indexing unordered_map with [] creates an element
            //with key as key... The value (forward_list) will be empty though.
            for(ICommand* command : cc[ic.bindings[ic.presses.front()]]) {
                command->execute();
            }
            ic.presses.pop();
        }
    }
}
unsigned int InputSystem::count() const {
    return ids.size();
}
const System InputSystem::getIdentifier() const {
    return System::INPUT;
}
void InputSystem::activateId(ID id) {
    if(ids.find(id) == ids.end()) {
        return;
    }
    activeIds.push(id);
}
