#include "inputsystem.hpp"
#include "componentmanager.hpp"
#include "icommand.hpp"

/** For logging **/
#include "logger.hpp"

void InputSystem::add(ID id) {
    ids.insert(id);
}
void InputSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        Logger::log("Tried to remove unpresent ID from InputSystem", Log::WARNING);
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
const System::Identifier InputSystem::getIdentifier() const {
    return System::INPUT;
}
bool InputSystem::activateId(ID id) {
    if(ids.find(id) != ids.end()) {
        activeIds.push(id);
        return true;
    }
    return false;
}
