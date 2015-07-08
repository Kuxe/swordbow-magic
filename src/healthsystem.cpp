#include "healthsystem.hpp"
#include <iostream>
#include "healthcomponent.hpp"
#include "componentmanager.hpp"
#include "systemmanager.hpp"

using std::cout;
using std::endl;

void HealthSystem::add(ID id) {
    ids.insert(id);
}
void HealthSystem::remove(ID id) {
    if(ids.find(id) == ids.end()) {
        cout << "WARNING: Tried to remove unpresent ID from HealthSystem. Anything can happen!" << endl;
        return;
    }
    ids.erase(id);
}
void HealthSystem::update() {
    for(auto id : ids) {
        auto& hc = componentManager->healthComponents.at(id);

        //Check if id is dead
        if(hc.health <= 0) {
            systemManager->getSystem(System::REMOVE)->activateId(id);
        }
    }
}
unsigned int HealthSystem::count() const {
    return ids.size();
}
const System HealthSystem::getIdentifier() const {
    return System::HEALTH;
}
void HealthSystem::activateId(ID id) {

}
