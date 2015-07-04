#include "diffsystem.hpp"
#include "componentmanager.hpp"


void DiffSystem::add(ID id) {
    ids.push(id);
}
void DiffSystem::remove(ID id) {

}
void DiffSystem::update() {
    componentManager->moveComponentsDiff.clear();
    while(!ids.empty()) {
        const auto id = ids.front(); ids.pop();
        componentManager->moveComponentsDiff.insert({id, componentManager->moveComponents.at(id)});
    }
}
unsigned int DiffSystem::count() const {
    return ids.size();
}
const string DiffSystem::getIdentifier() const {
    return "DiffSystem";
}
void DiffSystem::activateId(ID id) {
    add(id);
}
