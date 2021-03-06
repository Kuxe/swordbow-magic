#include "movediffsystem.hpp"
#include "componentmanager.hpp"

void MoveDiffSystem::add(ID id) {
    ids.insert(id);
}
void MoveDiffSystem::remove(ID id) {
    ids.erase(id);
}
void MoveDiffSystem::update() {
    //ids.clear();
}
unsigned int MoveDiffSystem::count() const {
    return ids.size();
}

void MoveDiffSystem::clear() {
	ids.clear();
}

const System::Identifier MoveDiffSystem::getIdentifier() const {
    return System::MOVEDIFF;
}
bool MoveDiffSystem::activateId(ID id) {
    add(id);
    return true;
}
